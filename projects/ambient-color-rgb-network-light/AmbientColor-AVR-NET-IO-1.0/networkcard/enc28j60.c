/*
,-----------------------------------------------------------------------------------------.
| this file implements the driver for the microchip enc28j60 ethernet controller
| - some ideas are based on the enc28j60 driver of the procyon avrlib ;)
|
| BUGS:
| - sometimes enc28j60 is not correctly initialised (not receiving any packets)
|
| Author   : Simon Schulz / avr{AT}auctionant.de
| this version 2007'0819 improved by eProfi (PHLCON 3472, skip select_bank for regs 1b..1f, 
|   enc28j60_select_bank  inlined into  enc28j60_read_address  and  enc28j60_write_address)
|-----------------------------------------------------------------------------------------
| License:
| This program is free software; you can redistribute it and/or modify it under
| the terms of the GNU General Public License as published by the Free Software
| Foundation; either version 2 of the License, or (at your option) any later
| version.
| This program is distributed in the hope that it will be useful, but
|
| WITHOUT ANY WARRANTY;
|
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License along with
| this program; if not, write to the Free Software Foundation, Inc., 51
| Franklin St, Fifth Floor, Boston, MA 02110, USA
|
| http://www.gnu.de/gpl-ger.html
`-----------------------------------------------------------------------------------------*/
#include "enc28j60.h"

#if USE_ENC28J60

volatile unsigned char enc28j60_current_bank;
volatile unsigned int  enc28j60_next_packet_ptr;
unsigned char enc28j60_revision;
#include <avr/pgmspace.h>

//store enc28j60 config in program memory
//syntax: <REGx>, <REGVALx>, <REGy>, <REGVALy>,...
unsigned char enc28j60_config[] PROGMEM = {
	//setup bank0 (config stored in progmem, see above)
	//tx buffer:
	ENC28J60_REG_ETXSTL, LO8(ENC28J60_TX_BUFFER_START), //start lo
	ENC28J60_REG_ETXSTH, HI8(ENC28J60_TX_BUFFER_START), //start hi
	ENC28J60_REG_ETXNDL, LO8(ENC28J60_TX_BUFFER_END  ), //end lo
	ENC28J60_REG_ETXNDH, HI8(ENC28J60_TX_BUFFER_END  ), //end hi
	//rx buffer
	ENC28J60_REG_ERXSTL, LO8(ENC28J60_RX_BUFFER_START), //start lo
	ENC28J60_REG_ERXSTH, HI8(ENC28J60_RX_BUFFER_START), //start hi
	ENC28J60_REG_ERXNDL, LO8(ENC28J60_RX_BUFFER_END  ), //end lo
	ENC28J60_REG_ERXNDH, HI8(ENC28J60_RX_BUFFER_END  ), //end hi
	//rx ptr:
	//ENC28J60_REG_ERDPTL, lo8(ENC28J60_RX_BUFFER_START+1),
	//ENC28J60_REG_ERDPTH, hi8(ENC28J60_RX_BUFFER_START+1),
	
	//setup bank1: (see microchip datasheet p.50)
	ENC28J60_REG_ERXFCON, 0x00,
	
	
	//setup bank2: (see microchip datasheet p.36)
	//1.) clear the MARST bit in MACON2.
	ENC28J60_REG_MACON2, 	0x00,
	//2.) mac rx enable, activate pause control frame support
	ENC28J60_REG_MACON1, 	((1<<ENC28J60_BIT_MARXEN)|(1<<ENC28J60_BIT_RXPAUS)|(1<<ENC28J60_BIT_TXPAUS)),
	//3.) setup MACON3: auto padding of small packets, add crc, enable frame length check:
	ENC28J60_REG_MACON3, 	((1<<ENC28J60_BIT_PADCFG0)|(1<<ENC28J60_BIT_TXCRCEN)|(1<<ENC28J60_BIT_FRMLNEN)),
	//4.) don't set up MACON4 (use default)
	//5.) setup maximum framelenght to 1518:
	ENC28J60_REG_MAMXFLL, LO8(1518),
	ENC28J60_REG_MAMXFLH, HI8(1518),
	//6.) set up back-to-back gap: 0x15 for full duplex / 0x12 for half duplex
	ENC28J60_REG_MABBIPG, 0x12, //half duplex
	//7.) setup non-back-to-back gap: use 0x12
	ENC28J60_REG_MAIPGL,  0x12,
	//8.) setup non-back-to-back gap high byte: 0x0C for half duplex:
	ENC28J60_REG_MAIPGH,  0x0C, //half duplex
	//9.) don't change MACLCON1+2 / MACLCON2 might be changed for networks with long wires !

	//setup bank3:
	//10.) programm mac address: BYTE BACKWARD !
	ENC28J60_REG_MAADR5, MYMAC1,
	ENC28J60_REG_MAADR4, MYMAC2,
	ENC28J60_REG_MAADR3, MYMAC3,
	ENC28J60_REG_MAADR2, MYMAC4,
	ENC28J60_REG_MAADR1, MYMAC5,
	ENC28J60_REG_MAADR0, MYMAC6
	};


//initialise spi & enc28j60
void enc28j60_init(void)
{
	unsigned char i;
	unsigned int timeout=0;

	//set bank to invalid value -> bank set will update in any case:
	enc28j60_current_bank = 0xFF;

	//set up port directions:
	ENC28J60_DDR |= (1<<ENC28J60_PIN_CS); 
	
	//RoBue:
	//PORTB0 + B3 als zusaetzliche Ausgabe
	ENC28J60_DDR |= (1<<PB0) | (1<<PB3);
	
	//deselect enc28j60:
	ENC28J60_CS_HI();

	//SPI init
	// initialize I/O
	PORTB |= (1<<ENC28J60_PIN_SCK); //sck = hi

	//spi = output
	DDRB |= (1<<ENC28J60_PIN_SS)|(1<<ENC28J60_PIN_CS)|(1<<ENC28J60_PIN_MOSI)|(1<<ENC28J60_PIN_SCK); //SS,MOSI,SCK = OUT
	DDRB &= ~(1<<ENC28J60_PIN_MISO); //MISO = IN

	//SPI init:
	// - master mode
	// - positive clock phase
	// - msb first
	// - maximum SPI speed (fosc/2)
	// - enable spi
	SPCR = (0<<SPIE)|(1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(0<<SPR0);
	SPSR = (1<<SPI2X);

	delay(200000);
	enc28j60_spi_write_word (ENC28J60_SOFT_RESET, ENC28J60_SOFT_RESET);
	// check CLKRDY bit to see if reset is complete
	delay(200000);
	
	//wait for ready flag
	while((!(enc28j60_read_address(ENC28J60_REG_ESTAT) & 0x01)) && (timeout<65000)){timeout++;};
	if(timeout>=65000){timeout=0;}

	//set up rx pointer:
	enc28j60_next_packet_ptr = ENC28J60_RX_BUFFER_START;
	//enc28j60_next_packet_ptr = 0x0602;

	//copy config from progmem to enc28j60:
	for(i=0; i<2*22; i+=2){
		enc28j60_write_address(pgm_read_byte(&enc28j60_config[i+0]),pgm_read_byte(&enc28j60_config[i+1]));
	}

	//setup phy:
	enc28j60_write_phy(ENC28J60_PHY_PHCON2, (1<<ENC28J60_BIT_HDLDIS)); //=no loopback of transmitted frames

	//enable interrups
	enc28j60_write_address(ENC28J60_REG_EIE, (1<<ENC28J60_EIE_INTIE)|(1<<ENC28J60_EIE_PKTIE));

	//enable rx
	//enc28j60_write_address(ENC28J60_REG_ECON1, (1<<ENC28J60_BIT_RXEN));
	enc28j60_spi_write_word(ENC28J60_OP_BFS|ENC28J60_REG_ECON1, (1<<ENC28J60_BIT_RXEN));
	
	//set up leds: LEDB: link status, LEDA: RX&TX activity, stretch 40ms, stretch enable
	enc28j60_write_phy(ENC28J60_PHY_PHLCON, 0x374A); //cave: Table3-3: reset value is 0x3422, do not modify the reserved "3"!! 
													 //RevA Datasheet page 9: write as '0000', see RevB Datasheet: write 0011!
	//read silicon revision
	//was geht hier vor?? Trotzdem die Bank 2 ist (0x40), gibt er beim Lesen 
	//der RevID keine neue Bank aus, trotzdem kommt der Wert 05.
	//Vielleicht ist es gar keine Rev05??
	enc28j60_revision = enc28j60_read_address(ENC28J60_REG_ECON1);   //um die aktuelle Banknummer zu sehen
	enc28j60_revision = enc28j60_read_address(ENC28J60_REG_EREVID);
}

void enc28j60_send_packet(unsigned int len, unsigned char *buffer)
{
	unsigned int tmp;
	//still sending ?
	//wait max 50*10 = 500ms:
	for(tmp=0; tmp<50; tmp++){
		if (!(enc28j60_read_address(ENC28J60_REG_ECON1) & (1<<ENC28J60_BIT_TXRTS))){
			//send finished -> tx packet
			break;
		}
		//tx not finished yet, wait 10ms
		_delay_ms(1);
	}
	
	//send anyway...

	//setup write pointer:
	enc28j60_write_address(ENC28J60_REG_EWRPTL, (ENC28J60_TX_BUFFER_START&0xFF));
	enc28j60_write_address(ENC28J60_REG_EWRPTH, (ENC28J60_TX_BUFFER_START)>>8);
	
	//set tx end pointer to [start+len]:
	enc28j60_write_address(ENC28J60_REG_ETXNDL, (ENC28J60_TX_BUFFER_START+len)&0xFF);
	enc28j60_write_address(ENC28J60_REG_ETXNDH, (ENC28J60_TX_BUFFER_START+len)>>8);

	//start buffer write command
	enc28j60_spi_write_word(ENC28J60_OP_WRITE_BUF_MEM, 0x00);

	//copy buffer to enc28j60:
	enc28j60_write_buffer(buffer, len);

	//bad silicon workaround:
	//reset tx logic:
	enc28j60_spi_write_word(ENC28J60_OP_BFS | ENC28J60_REG_ECON1, (1<<ENC28J60_BIT_TXRST));
	enc28j60_spi_write_word(ENC28J60_OP_BFC | ENC28J60_REG_ECON1, (1<<ENC28J60_BIT_TXRST));

	//activate transmission
	enc28j60_spi_write_word(ENC28J60_OP_BFS | ENC28J60_REG_ECON1, (1<<ENC28J60_BIT_TXRTS)|(1<<ENC28J60_BIT_RXEN));
}

unsigned int enc28j60_receive_packet(unsigned int maxlen, unsigned char *buffer)
{
	unsigned int rxstat;
	unsigned int len;

	//packet in buffer ?	
	if ((enc28j60_read_address(ENC28J60_REG_EIR) & (1<<ENC28J60_BIT_PKTIF)) == 0){
		//double check!	
		//errata says that PKTIF does not work as it should 
		//->check packetcount too:
		if (enc28j60_read_address(ENC28J60_REG_EPKTCNT) == 0)
			return 0;
	}

	//set read pointer to next packet;
	enc28j60_write_address(ENC28J60_REG_ERDPTL, (enc28j60_next_packet_ptr));
	enc28j60_write_address(ENC28J60_REG_ERDPTH, (enc28j60_next_packet_ptr)>>8);

	//now read the transmit status vector
	//read next packet ptr
	enc28j60_next_packet_ptr  = enc28j60_spi_read_byte(ENC28J60_OP_READ_BUF_MEM, 0);
	enc28j60_next_packet_ptr |= enc28j60_spi_read_byte(ENC28J60_OP_READ_BUF_MEM, 0)<<8;

	//read packet length
	len  = enc28j60_spi_read_byte(ENC28J60_OP_READ_BUF_MEM, 0);
	len |= enc28j60_spi_read_byte(ENC28J60_OP_READ_BUF_MEM, 0)<<8;
	
	//read rx stat
	rxstat  = enc28j60_spi_read_byte(ENC28J60_OP_READ_BUF_MEM, 0);
	rxstat |= enc28j60_spi_read_byte(ENC28J60_OP_READ_BUF_MEM, 0)<<8;

	//limit read bytecount
	if (len>maxlen)
		len = maxlen;

	//transfer packet from enc28j60 to our buffer
	enc28j60_read_buffer(buffer,len);

	//mark packet as processed (free mem)

	//ERRATA says we need to check packet pointer:
	if ((enc28j60_next_packet_ptr- 1 < ENC28J60_RX_BUFFER_START) || (enc28j60_next_packet_ptr- 1 > ENC28J60_RX_BUFFER_END)){
		enc28j60_write_address(ENC28J60_REG_ERXRDPTL, LO8(ENC28J60_RX_BUFFER_END));
		enc28j60_write_address(ENC28J60_REG_ERXRDPTH, HI8(ENC28J60_RX_BUFFER_END));
	}else{
		enc28j60_write_address(ENC28J60_REG_ERXRDPTL, LO8(enc28j60_next_packet_ptr- 1));
		enc28j60_write_address(ENC28J60_REG_ERXRDPTH, HI8(enc28j60_next_packet_ptr- 1));
	}

	//decrement packet counter:
	enc28j60_spi_write_word(ENC28J60_OP_BFS|ENC28J60_REG_ECON2, (1<<ENC28J60_BIT_PKTDEC));

	return len;
}

//read phy register
unsigned int enc28j60_read_phy(unsigned char address)
{
	unsigned int res;

	//see microchip datasheet p.21
	//set address to MIREGADR:
	enc28j60_write_address(ENC28J60_REG_MIREGADR, address);

	//set MICMD.MIIRD bit:
	enc28j60_write_address(ENC28J60_REG_MICMD, (1<<ENC28J60_BIT_MIIRD));

	//now read op is enabled, wait until PHY read finished:
	while(enc28j60_read_address(ENC28J60_REG_MISTAT) & (1<<ENC28J60_BIT_MISTAT_BUSY)){}

	//abort read
	enc28j60_write_address(ENC28J60_REG_MICMD, 0x00);
	
	//read result
	res  = enc28j60_read_address(ENC28J60_REG_MIRDH)<<8;
	res |= enc28j60_read_address(ENC28J60_REG_MIRDL);

	return res;
}

//write phy register
void enc28j60_write_phy(unsigned char address, unsigned int data)
{
	//see microchip datasheet p.21
	//set address to MIREGADR:
	enc28j60_write_address(ENC28J60_REG_MIREGADR, address);

	//send the data
	enc28j60_write_address(ENC28J60_REG_MIWRL, data&0xFF);
	enc28j60_write_address(ENC28J60_REG_MIWRH, data>>8  );

	//wait until PHY write finished:
	while(enc28j60_read_address(ENC28J60_REG_MISTAT) & (1<<ENC28J60_BIT_MISTAT_BUSY)){}
}

//read enc28j60 buffer
void enc28j60_read_buffer(unsigned char *buffer, unsigned int len)
{
	//select enc28j60
	ENC28J60_CS_LO();

	//send read cmd & wait for finish:
	SPDR = ENC28J60_OP_READ_BUF_MEM;
	while(!(SPSR & (1<<SPIF)));

	//read data & store in buffer:
	while(len--){
		//send dummy byte & read data
		SPDR = 0x00;
		while(!(SPSR & (1<<SPIF)));
		*buffer++ = SPDR;
	}

	//deselect enc28j60
	ENC28J60_CS_HI();
}

//write enc28j60 buffer
void enc28j60_write_buffer(unsigned char *buffer, unsigned int len)
{
	//select enc28j60
	ENC28J60_CS_LO();

	//send write cmd & wait for finish:
	SPDR = ENC28J60_OP_WRITE_BUF_MEM;
	while(!(SPSR & (1<<SPIF)));

	do{
		//send data
		SPDR = *buffer++;
		len--;
		while(!(SPSR & (1<<SPIF)));
	}while(len);


	//deselect enc28j60
	ENC28J60_CS_HI();
}

//read address
unsigned char enc28j60_read_address(unsigned char address)
{
	//hier Vereinfachung, da die Register 1b..1f in allen Bänken erreichbar sind, braucht die Bank nicht umgeschaltet werden
	//do it easy: for registers 1b..1f it is not necessary to change bank as they are available in all banks
	register unsigned char enc28j60_new_bank=address & 0x60;
	if(((address&0x1f)<0x1b) && (enc28j60_new_bank != enc28j60_current_bank)){ //Regs 0x1b..0x1f appear in every page, so switching is not necessary
		enc28j60_current_bank = enc28j60_new_bank;

		//clear bank sel bits:  //BF 03
		enc28j60_spi_write_word((ENC28J60_OP_BFC | ENC28J60_REG_ECON1),3<<ENC28J60_BIT_ECON1_BSEL0); 
		//setup bank selection bits: //e.g. 9F 00  for Bank 0
		enc28j60_spi_write_word((ENC28J60_OP_BFS | ENC28J60_REG_ECON1),enc28j60_new_bank>>5);
	}
	//and read the data:
	return enc28j60_spi_read_byte(ENC28J60_OP_READ_CTRL_REG, address);
}

//write address
void enc28j60_write_address(unsigned char address, unsigned char data)
{
	//select bank:
	register unsigned char enc28j60_new_bank=address & 0x60,addr_masked=address&0x1f;
	if((addr_masked < 0x1b) && (enc28j60_new_bank != enc28j60_current_bank)){ //Registers 0x1b..0x1f appear in every page, so switching is not necessary
		enc28j60_current_bank = enc28j60_new_bank;

		//clear bank sel bits:  //BF 03
		enc28j60_spi_write_word((ENC28J60_OP_BFC | ENC28J60_REG_ECON1),((1<<ENC28J60_BIT_ECON1_BSEL0) | (1<<ENC28J60_BIT_ECON1_BSEL1))); 
		//setup bank selection bits: //e.g. 9F 00  for Bank 0
		enc28j60_spi_write_word((ENC28J60_OP_BFS | ENC28J60_REG_ECON1),enc28j60_new_bank>>5);
	}
	//and write data
	enc28j60_spi_write_word((ENC28J60_OP_WRITE_CTRL_REG | addr_masked), data);
}


//read a byte (adds a extra dummy read if (address&0x80)
unsigned char enc28j60_spi_read_byte(unsigned char op, unsigned char address)
{
	unsigned char res;

	ENC28J60_CS_LO(); //select enc28j60

	SPDR = op | (address & 0x1F); //start read cmd

	while(!(SPSR & (1<<SPIF))); //wait for completion

	SPDR = 0x00; //send dummy data (read)

	while(!(SPSR & (1<<SPIF))); //wait for completion

	//if we read MAC* or MI* registers we need to add an dummy read here !
	if(address & 0x80){  //use bit7 of address for this
		SPDR = 0x00;
		while(!((SPSR) & (1<<SPIF)));
	}

	res = SPDR; //read data

	ENC28J60_CS_HI();//deselect enc28j60

	return res;
}

//spi write word
void enc28j60_spi_write_word(unsigned char hi, unsigned char lo)
{

	ENC28J60_CS_LO(); //select enc28j60
	SPDR = hi; //shift hi data out
	while(!(SPSR & (1<<SPIF))); //wait for completion

	SPDR = lo; //shift lo data out
	while(!(SPSR & (1<<SPIF))); //wait for completion

	ENC28J60_CS_HI(); //deselect enc28j60
}

#endif //USE_ENC28J60


