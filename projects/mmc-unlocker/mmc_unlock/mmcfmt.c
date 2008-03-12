/*** MMC Unlocker *************************************************************
 *
 *	File Name  : mmcfmt.c
 *	Title      : Multi-Media Card Unlocker
 *  Description: Reset locked MMC cards (and wipes all data) allowing them
 *                to be used again
 *	Author     : Muhammad J. A. Galadima
 *	Created    : 2004 / 01 / 27
 *	Modified   : 2005 / 08 / 31
 *               2008 / 03 / 05
 *			Dropped serial port speed to 9600 because it didn't
 *                      work at 115200 with the internal osc any more (thought
 *                      I broke my STK (haven't used it in a while). It still
 *                      works at 57600 but I'll leave it at 9600 to be safe
 *	Version    : 0.2
 *	Target MCU : STK500 (can be used w/any AVRs with enough pins for
                 the MMC card (4) plus any switches/LEDs you want)
 *	
 *	
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version 2
 *	of the License, or (at your option) any later version.
 *	
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *****************************************************************************/

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/signal.h>
#include <avr/pgmspace.h>

#include <ctype.h>
#include <inttypes.h>
#include <string.h>


#define MMC_PORT	PORTC
#define MMC_PIN		PINC
#define MMC_DDR		DDRC
#define MMC_CS      PC0
#define MMC_MISO    PC2
#define MMC_MOSI    PC1
#define MMC_CLK     PC6

#define LED_PORT	PORTB
#define LED_PIN		PINB
#define LED_DDR		DDRB
#define SW_PORT		PORTA
#define SW_PIN		PINA
#define SW_DDR		DDRA
#define BAUDRATE 9600
#define BAUD_REG	((uint16_t)((F_CPU / (16.0 * (BAUDRATE))) + 0.5) - 1)	// if above .5 mark, round up; replace 16 with 8 for double
#define BAUD_H  	((uint8_t)(0xFF&(BAUD_REG>>8)))
#define BAUD_L		((uint8_t)(0xFF&BAUD_REG))

#define spi_response()	spi_byte(0xFF)					// read response
#define spi_busywait()	while(spi_byte(0xFF) == 0)		// wait for non-zero response to continue



uint8_t spi_cmd[5];	// 6 is CRC, which we won't be using here (or should be generated indide spi_command())
uint8_t reg_csd[16];
uint8_t reg_cid[16];
uint16_t block_len;
uint32_t block_cnt;


/*** SIG_UART_RECV ************************************************************
 *	interrupt on receive byte; for now just echo if any char received
 *****************************************************************************/
SIGNAL (SIG_UART_RECV) {
	uint8_t temp;
	temp = UDR;			// read
	LED_PORT = ~temp;	// show the current command on the LEDs
	if(temp=='\r' || temp == '\n') {
		printf_P(PSTR("\n\r"));
	}
	else
		UDR = temp;			// write	(just echo what the user types)
}

uint8_t uart_tx(uint8_t uart_tx) {
	while(!(UCSRA & _BV(UDRE)));		// wait for empty tx buffer
	UDR = uart_tx;						// put data in buffer, init send
	
	return 0;
}
// uint8_t uart_tx(uint8_t uart_tx)


uint8_t uart_rx(void) {
	while(!(UCSRA & _BV(RXC)));		// wait for full rx buffer
	return UDR;                   /* return the new c */
}
// uint8_t uart_rx(void)


/*** delay ********************************************************************
 *	rough delay; 65k loops, 4 instr each, +over head: 65536*4 = 262144, 
 *	round up tp 300000 clks (time: 300000/F_CPU seconds)
 *****************************************************************************/
void delay(void) {
	uint8_t i, j;
	for(i=0; i<255; i++) {
		for(j=0; j<255; j++) {
			asm volatile("nop"::);
			asm volatile("nop"::);
			asm volatile("nop"::);
			asm volatile("nop"::);
		}
	}
}
// void delay(void)

/*** show_resp ****************************************************************
 *	show value in response on LEDs and wait for keypress; for debugging only
 *****************************************************************************/
void show_resp(uint8_t retval) {
	LED_PORT = ~(retval);				// light leds 0-7 according to error,
										//	and show that system is waiting (led7)
	loop_until_bit_is_clear(SW_PIN, 0);	// wait for sw0 to be pressed
	delay();							// debounce delay
	loop_until_bit_is_set(SW_PIN, 0);	// wait for sw0 to be released
	delay();							// debounce delay
	LED_PORT = 0xFF;					// clear all leds
}
// void show_resp(uint8_t retval)

/*** spi_byte *****************************************************************
 *	Send one, receive one (happens simultaneously)
 *****************************************************************************/
uint8_t spi_byte(uint8_t spi_tx) {
	uint8_t i, spi_rx = 0;
	
	for(i=8; i>0; i--) {
	    i--;
		
		if( bit_is_set(spi_tx,i) )
		    MMC_PORT |= _BV(MMC_MOSI);  // set mosi pin
		else
		    MMC_PORT &= ~_BV(MMC_MOSI);  // clr mosi pin

		// read MISO
	    spi_rx <<= 1;		// x2, shift left to create space for next
		if( bit_is_set(MMC_PIN,MMC_MISO) )
			spi_rx++;

		MMC_PORT |= _BV(MMC_CLK);	// clk high
	    MMC_PORT &= ~_BV(MMC_CLK);	// clk low
 		
	    i++;
    }
	
	return spi_rx;
}
// uint8_t spi_byte(uint8_t spi_tx)


/*** spi_command **************************************************************
 *	Send contents of command structure, get (first) response byte
 *****************************************************************************/
uint8_t spi_command(void) {
	uint8_t retval, i;
	
	MMC_PORT &= ~_BV(MMC_CS);	// cs low (select, spi mode)
	
	spi_byte(0xFF);
	// send command struct
	spi_byte(spi_cmd[0] | 0x40);	// make 2 MSBs '01' (01xxxxxx, where x represents command bit)
	spi_byte(spi_cmd[4]);			// address
	spi_byte(spi_cmd[3]);			//    "
	spi_byte(spi_cmd[2]);			//    "
	spi_byte(spi_cmd[1]);			//    "
	spi_byte(0x95);					// CRC (from spec; calculate?)
//	spi_byte(0xFF);
	
	i=0;
	do {							// Flush Ncr (1-8 bytes) before response
		retval = spi_byte(0xff);
		i++;
	} while(i<8 && retval == 0xFF);
	
	return retval;	// return R1 response (or first byte of any other command)
}
// uint8_t spi_command(void)


/*** get_slice ****************************************************************
 *	get the requested portion of the given register
 *****************************************************************************/
uint16_t get_slice(uint8_t *ptr_reg, uint8_t start, uint8_t stop) {
	uint8_t upper, lower, lower_s, count, i;
	uint32_t rval=0;
	
	lower = stop / 8.0;
	upper = start / 8.0;
	
	// merge all used bytes into on large var
	count = upper - lower;
	for(i=0; i<=count; i++) {
		rval <<= 8;
		rval |= ptr_reg[upper-i];
	}
	
	// shift data down so it starts at zero
	lower_s = stop - (lower * 8);
	rval >>= lower_s;
	
	// clear upper bits (all bits before start)
	start -= stop;
	start++;
	//for(i=start; i<32; i++)
	for(i=start; i<16; i++)	// clear bits 15-start
		rval &= ~(1<<i);
	rval &= 0x0000FFFF;			// clear bits 31-16
	
	return (uint16_t)rval;
}
// uint16_t get_slice(uint8_t ptr_reg[], uint8_t start, uint8_t stop)

/*** get_regs *****************************************************************
 *	read the contents of the CSD + CID registers from the card
 *****************************************************************************/
void get_regs(void) {
	uint8_t i, c_size_mult, read_bl_len;
	
	spi_cmd[0] = 9;
	printf_P(PSTR("\n\rResponse to CMD9: 0 / %d"), spi_command());
	while(spi_byte(0xFF) != 0xFE);		// wait for data start token; add time-out?
	for(i=0; i<16; i++)
		reg_csd[15-i] = spi_byte(0xFF);	// fill in from MSB
		
	printf_P(PSTR("\n\r Contents of CSD Register:"));	// defaults from SanDisk 32MB card / read values
	printf_P(PSTR("\n\r  CSD_STRUCTURE:	1 / %x"), get_slice(reg_csd,127,126) );
	printf_P(PSTR("\n\r  MMC_PROT:		1 / %x"), get_slice(reg_csd,125,122) );
	printf_P(PSTR("\n\r  Reserved:		0 / %x"), get_slice(reg_csd,121,120) );
	printf_P(PSTR("\n\r  TAAC:			26 / %x"), get_slice(reg_csd,119,112) );
	printf_P(PSTR("\n\r  NSAC:			0 / %x"), get_slice(reg_csd,111,104) );
	printf_P(PSTR("\n\r  TRAN_SPEED:		2a / %x"), get_slice(reg_csd,103,96) );
	printf_P(PSTR("\n\r  CCC:			1ff / %x"), get_slice(reg_csd,95,84) );
	read_bl_len=get_slice(reg_csd,83,80);
	printf_P(PSTR("\n\r  READ_BL_LEN:		9 / %x"), read_bl_len );
	printf_P(PSTR("\n\r  READ_BL_PARTIAL:	1 / %x"), get_slice(reg_csd,79,79) );
	printf_P(PSTR("\n\r  WRITE_BLK_MISALIGN:	0 / %x"), get_slice(reg_csd,78,78) );
	printf_P(PSTR("\n\r  READ_BLK_MISALIGN:	0 / %x"), get_slice(reg_csd,77,77) );
	printf_P(PSTR("\n\r  DSR_IMP:		0 / %x"), get_slice(reg_csd,76,76) );
	printf_P(PSTR("\n\r  Reserved:		0 / %x"), get_slice(reg_csd,75,74) );
	block_cnt=get_slice(reg_csd,73,62);	// this var is c_size, not block count; just using the block cnt to save space (instead of creating new var)
	printf_P(PSTR("\n\r  C_SIZE:		- / %x"), block_cnt );
	printf_P(PSTR("\n\r  VDD_R_CURR_MIN:	4 / %x"), get_slice(reg_csd,61,59) );
	printf_P(PSTR("\n\r  VDD_R_CURR_MAX:	4 / %x"), get_slice(reg_csd,58,56) );
	printf_P(PSTR("\n\r  VDD_W_CURR_MIN:	5 / %x"), get_slice(reg_csd,55,53) );
	printf_P(PSTR("\n\r  VDD_W_CURR_MAX:	5 / %x"), get_slice(reg_csd,52,50) );
	c_size_mult=get_slice(reg_csd,49,47);
	printf_P(PSTR("\n\r  C_SIZE_MULT:		- / %x"), c_size_mult );
	printf_P(PSTR("\n\r  SECTOR_SIZE:		0 / %x"), get_slice(reg_csd,46,42) );
	printf_P(PSTR("\n\r  ERASE_GRP_SIZE:	- / %x"), get_slice(reg_csd,41,37) );
	printf_P(PSTR("\n\r  WP_GRP_SIZE:		1f / %x"), get_slice(reg_csd,36,32) );
	printf_P(PSTR("\n\r  WP_GRP_ENABLE:	1 / %x"), get_slice(reg_csd,31,31) );
	printf_P(PSTR("\n\r  DEFAULT_ECC:		0 / %x"), get_slice(reg_csd,30,29) );
	printf_P(PSTR("\n\r  R2W_FACTOR:		4 / %x"), get_slice(reg_csd,28,26) );
	printf_P(PSTR("\n\r  WRITE_BL_LEN:		9 / %x"), get_slice(reg_csd,25,22) );
	printf_P(PSTR("\n\r  WRITE_BL_PARTIAL:	0 / %x"), get_slice(reg_csd,21,21) );
	printf_P(PSTR("\n\r  Reserved:		0 / %x"), get_slice(reg_csd,20,16) );
	printf_P(PSTR("\n\r  Reserved:		0 / %x"), get_slice(reg_csd,15,15) );
	printf_P(PSTR("\n\r  COPY:			1 / %x"), get_slice(reg_csd,14,14) );
	printf_P(PSTR("\n\r  PERM_WRITE_PROTECT:	0 / %x"), get_slice(reg_csd,13,13) );
	printf_P(PSTR("\n\r  TEMP_WRITE_PROTECT:	0 / %x"), get_slice(reg_csd,12,12) );
	printf_P(PSTR("\n\r  Reserved:		0 / %x"), get_slice(reg_csd,11,10) );
	printf_P(PSTR("\n\r  ECC:			0 / %x"), get_slice(reg_csd,9,8) );
	printf_P(PSTR("\n\r  CRC:			- / %x"), get_slice(reg_csd,7,1) );
	printf_P(PSTR("\n\r  Not Used (Always 1):	1 / %x"), get_slice(reg_csd,0,0) );
	
	block_cnt++;
	block_cnt *= (1<<(c_size_mult+2));
	block_len = (1<<read_bl_len);
	printf_P(PSTR("\n\r Card Size:  %ld blocks @ %db = %ldmb\n\r"),
		block_cnt, block_len, block_cnt*block_len/1000000 );
	
	
	spi_cmd[0] = 10;
	printf_P(PSTR("\n\rResponse to CMD10: 0 / %d"), spi_command());
	while(spi_byte(0xFF) != 0xFE);		// wait for data start token; add time-out?
	for(i=0; i<16; i++)
		reg_cid[15-i] = spi_byte(0xFF);	// fill in from MSB
	printf_P(PSTR("\n\r Contents of CID Register:"));
	printf_P(PSTR("\n\r  Product Name:	"));
	for(i=0; i<7; i++)
		uart_tx(reg_cid[12-i]);
	printf_P(PSTR("\n\r  Mfg Month:		%d"), get_slice(reg_cid,15,12));		// ??? card returns 12/2005
	printf_P(PSTR("\n\r  Mfg Year:		%d"), 1996+get_slice(reg_cid,11,8));	// ??? .. unless it's 12/2004?
}
// void get_regs(void)

/*** init_spi *****************************************************************
 *	init card
 *****************************************************************************/
void init_spi(void) {
	uint8_t tries, retval;

	MMC_PORT |= _BV(MMC_CS);	// cs high (deselect)
	spi_byte(0xFF);				// spec says to clock at least 74 high bits into the
	spi_byte(0xFF);				//	MMC; we'll do 80 here, it's easier ;)
	spi_byte(0xFF);
	spi_byte(0xFF);
	spi_byte(0xFF);
	spi_byte(0xFF);
	spi_byte(0xFF);
	spi_byte(0xFF);
	spi_byte(0xFF);
	spi_byte(0xFF);
	MMC_PORT &= ~_BV(MMC_CS);			// cs low (select, spi mode)
	
	// send CMD0 (reset/idle)
	
	// send CMD0 (reset) continuously until out of idle
	// loop until return == 00 0001 (idle); ?after timeout there's an error
	tries = 0xFF;
	while(( (retval=spi_command())==0) && tries!=0)	// (less space than checking !=1) all vals in array already 0, no need to mod spi_command
		tries--;								// all vals in array already 0, no need to mod
	printf_P(PSTR("\n\rResponse to CMD0: 1 / %d"), retval);	// get rid of retval assignment in while when done debugging
	if(tries == 0)
		return;
	
	// send CMD1 (init) continuously until ready
	// loop until return == 00 0000 (ready); ?after timeout there's an error
	spi_cmd[0] = 1;
	tries = 0xFF;
	while(( (retval=spi_command())!=0) && tries!=0)			// 
		tries--;
	printf_P(PSTR("\n\rResponse to CMD1: 0 / %d"), retval);
	if(tries == 0)
		return;
	
	spi_cmd[0] = 13;
	retval = spi_command();
	printf_P(PSTR("\n\rResponse to CMD13: 0,0 / %d,%d"), retval,spi_byte(0xff));
	    // show 1st and 2nd bytes of R1b response

	get_regs();
}
// void init_spi(void)


/*** init_io ******************************************************************
 *	init io ports
 *****************************************************************************/
void init_io(void) {
	// init ports
	
	/* PORT C pins connected to MMC
		0 / CS	- out, high (inactive)
		1 / DO	- out from MMC (MISO, only read on clock)
		2 / DI	- in to MMC (MOSI)
		4 / GND	- 2nd ground; make output and ground pin
		6 / CLK	- in to MMC
	*/

	//				 C G MMC
	//				 K   IOS
	MMC_DDR		= 0b01010011;	// 1011 1111: all outputs except Din (PA6); PA7 unused
	MMC_PORT	= 0b10101101;	// 1100 1011: gnd, clk, MOSI low, others high (pull-up on Din)
	
	LED_DDR		= 0xFF;			// all leds are outputs
	LED_PORT	= 0xFF;			// all leds off
	
	SW_DDR = 0x00;				// all switches are inputs
	SW_PORT = 0xFF;				// pull-ups active
	
	// init USART
//	UCSRC &= ~_BV(UMSEL);		// make sure we're in async mode
//	UCSRA |= _BV(U2X);			// set double speed, change 16 to 8 below
	
	// set baud rate: UBRR = (F_CPU/(16*BAUDRATE)) - 1
	UBRRH = BAUD_H;
	UBRRL = BAUD_L;
	UCSRB = _BV(RXEN) | _BV(TXEN) | _BV(RXCIE);	// enable tx, rx //and rx int
	
	sei();
}
// void init_io(void)

/*** init_vars ****************************************************************
 *	init vars
 *****************************************************************************/
void init_vars(void) {
	spi_cmd[0] = 0;		// command
	spi_cmd[4] = 0;		// 31-24	(MSB)
	spi_cmd[3] = 0;		// 23-16
	spi_cmd[2] = 0;		// 15-08
	spi_cmd[1] = 0;		// 07-00	(LSB)
}
// void init_vars(void)

int main(void) {
//	uint8_t retval;
	
	init_vars();
	init_io();
	
	FILE * stdio_dev;
	stdio_dev = fdevopen(uart_tx, uart_rx);
	
	printf_P(PSTR("\n\r\n\r\n\rLocked Card Fixer (build 080305003), by Muhammad J. A. Galadima.\n\r\n\r"));
	
	init_spi();
	
	printf_P(PSTR("\n\r\n\r\n\rPlease remove your card now, or press a key to wipe."));
	show_resp(128);
	printf_P(PSTR("\n\rPerforming card WIPE..."));
	
	
	// now do something (ie. erase card!)
	
	// set block length to 1
	spi_cmd[0] = 16;	// set block length / R1
	spi_cmd[4] = 0;		// set size to 1
	spi_cmd[3] = 0;		// 		"
	spi_cmd[2] = 0;		// 		"
	spi_cmd[1] = 1;		// 		"
	spi_command();
	
	// send the send forced erase command
	spi_cmd[0] = 42;	// LOCK_UNLOCK / R1b
	spi_cmd[4] = 0;		// 
	spi_cmd[3] = 0;		// 		"
	spi_cmd[2] = 0;		// 		"
	spi_cmd[1] = 0;		// 		"
	spi_command();
	spi_busywait();		// wait for busy
	
	// send the data byte (1 byte, as defined in block length)
	spi_byte(0b11111110);	// data start token
	spi_byte(0b00001000);	// forced erase
	spi_byte(0xFF);			// 
	spi_byte(0xFF);			// CRC
	
	printf_P(PSTR("\n\r\n\rData Response: 0x%x"), 0b00011111 & spi_byte(0xFF));	// read data response
	spi_busywait();
	printf_P(PSTR("\n\rCard Erased!"));
	
	/*
	uint32_t i;
	uint8_t block[512];
	for(i=0; i<512; i++)
		block[i] = 0;
	*/
	
	// loop forever
    for (;;);
}
