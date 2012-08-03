#include <avr/io.h>
#include "enc28j60.h"
#include "lowlevel.h"

static uint8_t Enc28j60Bank;
static uint16_t NextPacketPtr;

#define ENC28J60_CONTROL_PORT    PORTB
#define ENC28J60_CONTROL_DDR     DDRB
#define ENC28J60_CONTROL_CS      PORTB2
#define ENC28J60_CONTROL_SO      PORTB5
#define ENC28J60_CONTROL_SI      PORTB6
#define ENC28J60_CONTROL_SCK     PORTB7

// set CS to 0 = active
#define CSACTIVE ENC28J60_CONTROL_PORT &= ~(1 << ENC28J60_CONTROL_CS)
// set CS to 1 = passive
#define CSPASSIVE ENC28J60_CONTROL_PORT |= (1 << ENC28J60_CONTROL_CS)
//
#define waitspi() while(!(SPSR&(1<<SPIF)))

//*****************************************************************************
uint8_t Enc28j60ReadOp(uint8_t op, uint8_t address)
{
   CSACTIVE;
   // issue read command
   SPDR = op | (address & ADDR_MASK);
   waitspi();
   // read data
   SPDR = 0x00;
   waitspi();
   // do dummy read if needed (for mac and mii, see datasheet page 29)
   if(address & 0x80)
   {
      SPDR = 0x00;
      waitspi();
   }
   // release CS
   CSPASSIVE;
   return(SPDR);
}

//*****************************************************************************
void Enc28j60WriteOp(uint8_t op, uint8_t address, uint8_t data)
{
   CSACTIVE;
   // issue write command
   SPDR = op | (address & ADDR_MASK);
   waitspi();
   // write data
   SPDR = data;
   waitspi();
   CSPASSIVE;
}

//*****************************************************************************
void Enc28j60ReadBuffer(uint16_t len, uint8_t* data)
{
   CSACTIVE;
   // issue read command
   SPDR = ENC28J60_READ_BUF_MEM;
   waitspi();
   while(len)
   {
      len--;
      // read data
      SPDR = 0x00;
      waitspi();
      *data = SPDR;
      data++;
   }
   *data='\0';
   CSPASSIVE;
}

//*****************************************************************************
void Enc28j60WriteBuffer(uint16_t len, uint8_t* data)
{
   CSACTIVE;
   // issue write command
   SPDR = ENC28J60_WRITE_BUF_MEM;
   waitspi();
   while(len)
   {
      len--;
      // write data
      SPDR = *data;
      data++;
      waitspi();
   }
   CSPASSIVE;
}

//*****************************************************************************
void Enc28j60SetBank(uint8_t address)
{
   // set the bank (if needed)
   if((address & BANK_MASK) != Enc28j60Bank)
   {
      // set the bank
      Enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
      Enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
      Enc28j60Bank = (address & BANK_MASK);
   }
}

//*****************************************************************************
uint8_t Enc28j60Read(uint8_t address)
{
   // set the bank
   Enc28j60SetBank(address);
   // do the read
   return Enc28j60ReadOp(ENC28J60_READ_CTRL_REG, address);
}

//*****************************************************************************
void Enc28j60Write(uint8_t address, uint8_t data)
{
   // set the bank
   Enc28j60SetBank(address);
   // do the write
   Enc28j60WriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

//*****************************************************************************
void Enc28j60PhyWrite(uint8_t address, uint16_t data)
{
   // set the PHY register address
   Enc28j60Write(MIREGADR, address);
   // write the PHY data
   Enc28j60Write(MIWRL, data);
   Enc28j60Write(MIWRH, data>>8);
   // wait until the PHY write completes
   while(Enc28j60Read(MISTAT) & MISTAT_BUSY)
   {
      DelayUs(15);
   }
}


//**************************************************************************************
// Flash the 2 RJ45 LEDs twice to show that the interface works
void InitPhy (void)
{
	/* Magjack leds configuration, see enc28j60 datasheet, page 11 */
	// LEDA=green LEDB=yellow
	//
	// 0x880 is PHLCON LEDB=on, LEDA=on
	// enc28j60PhyWrite(PHLCON,0b0000 1000 1000 00 00);
	Enc28j60PhyWrite(PHLCON,0x880);
	DelayMs(500);
	//
	// 0x990 is PHLCON LEDB=off, LEDA=off
	// enc28j60PhyWrite(PHLCON,0b0000 1001 1001 00 00);
	Enc28j60PhyWrite(PHLCON,0x990);
	DelayMs(500);
	//
	// 0x880 is PHLCON LEDB=on, LEDA=on
	// enc28j60PhyWrite(PHLCON,0b0000 1000 1000 00 00);
	Enc28j60PhyWrite(PHLCON,0x880);
	DelayMs(500);
	//
	// 0x990 is PHLCON LEDB=off, LEDA=off
	// enc28j60PhyWrite(PHLCON,0b0000 1001 1001 00 00);
	Enc28j60PhyWrite(PHLCON,0x990);
	DelayMs(500);
	//
   // 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
   // enc28j60PhyWrite(PHLCON,0b0000 0100 0111 01 10);
   Enc28j60PhyWrite(PHLCON,0x476);
	DelayMs(100);
}


//*****************************************************************************
void Enc28j60Init(uint8_t* macaddr)
{
   // initialize I/O
   // cs as output:
   ENC28J60_CONTROL_DDR |= (1 << ENC28J60_CONTROL_CS);
   CSPASSIVE; // ss=0
   //	
   ENC28J60_CONTROL_DDR |= (1 << ENC28J60_CONTROL_SO) | (1 << ENC28J60_CONTROL_SCK); // mosi, sck output
   ENC28J60_CONTROL_DDR &= ~(1 << ENC28J60_CONTROL_SI); // MISO is input
   //
   ENC28J60_CONTROL_PORT &= ~(1 << ENC28J60_CONTROL_SO); // MOSI low
   ENC28J60_CONTROL_PORT &= ~(1 << ENC28J60_CONTROL_SCK); // SCK low
   //
   // initialize SPI interface
   // master mode and Fosc/2 clock:
   SPCR = (1 << SPE) | (1 << MSTR);
   SPSR |= (1 << SPI2X);
   
   // perform system reset
   Enc28j60WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
   DelayMs(50);
   // check CLKRDY bit to see if reset is complete
   // The CLKRDY does not work. See Rev. B4 Silicon Errata point. Just wait.
   //while(!(enc28j60Read(ESTAT) & ESTAT_CLKRDY));
   // do bank 0 stuff
   // initialize receive buffer
   // 16-bit transfers, must write low byte first
   // set receive buffer start address
   NextPacketPtr = RXSTART_INIT;
   // Rx start
   Enc28j60Write(ERXSTL, RXSTART_INIT & 0xFF);
   Enc28j60Write(ERXSTH, RXSTART_INIT >> 8);
   // set receive pointer address
   Enc28j60Write(ERXRDPTL, RXSTART_INIT & 0xFF);
   Enc28j60Write(ERXRDPTH, RXSTART_INIT >> 8);
   // RX end
   Enc28j60Write(ERXNDL, RXSTOP_INIT & 0xFF);
   Enc28j60Write(ERXNDH, RXSTOP_INIT >> 8);
   // TX start
   Enc28j60Write(ETXSTL, TXSTART_INIT & 0xFF);
   Enc28j60Write(ETXSTH, TXSTART_INIT >> 8);
   // TX end
   Enc28j60Write(ETXNDL, TXSTOP_INIT & 0xFF);
   Enc28j60Write(ETXNDH, TXSTOP_INIT >> 8);
   // do bank 1 stuff, packet filter:
   // For broadcast packets we allow only ARP packtets
   // All other packets should be unicast only for our mac (MAADR)
   //
   // The pattern to match on is therefore
   // Type     ETH.DST
   // ARP      BROADCAST
   // 06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
   // in binary these poitions are:11 0000 0011 1111
   // This is hex 303F->EPMM0=0x3f,EPMM1=0x30
   Enc28j60Write(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN);
   Enc28j60Write(EPMM0, 0x3f);
   Enc28j60Write(EPMM1, 0x30);
   Enc28j60Write(EPMCSL, 0xf9);
   Enc28j60Write(EPMCSH, 0xf7);
   //
   //
   // do bank 2 stuff
   // enable MAC receive
   Enc28j60Write(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
   // bring MAC out of reset
   Enc28j60Write(MACON2, 0x00);
   // enable automatic padding to 60bytes and CRC operations
   Enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);
   // set inter-frame gap (non-back-to-back)
   Enc28j60Write(MAIPGL, 0x12);
   Enc28j60Write(MAIPGH, 0x0C);
   // set inter-frame gap (back-to-back)
   Enc28j60Write(MABBIPG, 0x12);
   // Set the maximum packet size which the controller will accept
   // Do not send packets longer than MAX_FRAMELEN:
   Enc28j60Write(MAMXFLL, MAX_FRAMELEN & 0xFF);	
   Enc28j60Write(MAMXFLH, MAX_FRAMELEN >> 8);
   // do bank 3 stuff
   // write MAC address
   // NOTE: MAC address in ENC28J60 is byte-backward
   Enc28j60Write(MAADR5, macaddr[0]);
   Enc28j60Write(MAADR4, macaddr[1]);
   Enc28j60Write(MAADR3, macaddr[2]);
   Enc28j60Write(MAADR2, macaddr[3]);
   Enc28j60Write(MAADR1, macaddr[4]);
   Enc28j60Write(MAADR0, macaddr[5]);
   // no loopback of transmitted frames
   Enc28j60PhyWrite(PHCON2, PHCON2_HDLDIS);
   // switch to bank 0
   Enc28j60SetBank(ECON1);
   // enable interrutps
   Enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE);
   // enable packet reception
   Enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
}

//*****************************************************************************
uint8_t Enc28j60getrev(void)     // read the revision of the chip
{
	return(Enc28j60Read(EREVID));
}

//*****************************************************************************
void Enc28j60PacketSend(uint16_t len, uint8_t* packet)
{
	// Set the write pointer to start of transmit buffer area
	Enc28j60Write(EWRPTL, TXSTART_INIT & 0xFF);
	Enc28j60Write(EWRPTH, TXSTART_INIT >> 8);
	// Set the TXND pointer to correspond to the packet size given
	Enc28j60Write(ETXNDL, (TXSTART_INIT + len) & 0xFF);
	Enc28j60Write(ETXNDH, (TXSTART_INIT + len) >> 8);
	// write per-packet control byte (0x00 means use macon3 settings)
	Enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);
	// copy the packet into the transmit buffer
	Enc28j60WriteBuffer(len, packet);
	// send the contents of the transmit buffer onto the network
	Enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
   // Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
	if((Enc28j60Read(EIR) & EIR_TXERIF))
   {
      Enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
   }
}

//*****************************************************************************
// Gets a packet from the network receive buffer, if one is available.
// The packet will be headed by an ethernet header.
//      maxlen  The maximum acceptable length of a retrieved packet.
//      packet  Pointer where packet data should be stored.
// Returns: Packet length in bytes if a packet was retrieved, zero otherwise.
uint16_t Enc28j60PacketReceive(uint16_t maxlen, uint8_t* packet)
{
	uint16_t rxstat;
	uint16_t len;
	// check if a packet has been received and buffered
	//if( !(enc28j60Read(EIR) & EIR_PKTIF) ){
        // The above does not work. See Rev. B4 Silicon Errata point 6.
	if( Enc28j60Read(EPKTCNT) ==0 )
   {
		return(0);
   }

	// Set the read pointer to the start of the received packet
	Enc28j60Write(ERDPTL, NextPacketPtr);
	Enc28j60Write(ERDPTH, NextPacketPtr >> 8);
	
   // read the next packet pointer
	NextPacketPtr  = Enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	NextPacketPtr |= Enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0) << 8;

	// read the packet length (see datasheet page 43)
	len  = Enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	len |= Enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0) << 8;
   len -= 4; //remove the CRC count
	
   // read the receive status (see datasheet page 43)
	rxstat  = Enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	rxstat |= Enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0) << 8;

	// limit retrieve length
   if (len>maxlen-1)
   {
      len=maxlen-1;
   }
   // check CRC and symbol errors (see datasheet page 44, table 7-3):
   // The ERXFCON.CRCEN is set by default. Normally we should not
   // need to check this.
   if ((rxstat & 0x80)==0)
   {
      // invalid
      len=0;
   }
   else
   {
      // copy the packet from the receive buffer
      Enc28j60ReadBuffer(len, packet);
   }
   
   // Move the RX read pointer to the start of the next received packet
	// This frees the memory we just read out
	Enc28j60Write(ERXRDPTL, NextPacketPtr);
	Enc28j60Write(ERXRDPTH, NextPacketPtr >> 8);
	
   // decrement the packet counter indicate we are done with this packet
	Enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
	return(len);
}

