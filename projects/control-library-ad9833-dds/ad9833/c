// ******************************************************************************************************************************** //
// *******************  SPI Communication driver for AD9833 DDS with Frame Sync,,,  ***********************************************	//
// ********************************************************************************************************************************	//
// 	Last modification:  April 2008																									//
//	Original source:	AVRwiz http://greschenz.dyndns.org/?title=AvrWiz															//
//	Reference 1: 		http://winavr.scienceprog.com/avr-gcc-tutorial/serial-peripheral-interface-spi-of-avr-microcontrollers.html	//
// 	Reference 2: 		http://www.mikroe.com/forum/viewtopic.php?t=9976															//
//	Modified by: 		Michael Grant (krazatchu at hotmail dot com)																//		
//	Compiler: 			GCC / AVR Studio 4																							//
//	Target device:		ATmega8 @ 10 MHz																							//
// 	Application: 		http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=62560										//
// ********************************************************************************************************************************	//

#include <avr/io.h>
#include "AD9833.h"


/*************************************************************************
Function: SPI_init()
Purpose:  initialize the SPI bus 
Input:    none
Returns:  none
**************************************************************************/
void SPI_init (void)
{
	DDRB 	= 0xFF;
	PORTB 	= 0xFF;
	
	DDRB  = _BV(PB0) | _BV(PB3) | _BV(PB5);	// 	set SCK,MOSI,PB0 as Fsync 
	PORTB = _BV(PB5) | _BV(PB0);			// 	SCK and PB0 high 
	SPCR  = _BV(SPE)| _BV(MSTR)| _BV(CPOL); // 	Enable SPI // Set Master mode //	Set clk to inv.
	// SPCR |= _BV(SPR0)					//	Clk speed = fck/4, SPR0,SPR1 = 0,0 // uncomment for fck/16
}


/*************************************************************************
Function: SPI_write16()
Purpose:  send a 16bit word to the AD9833 
Input:    unsigned short data = 16bits
Returns:  none
Comment:  uses 8bit filter and two consecutive writes while fsync stays low
**************************************************************************/
void SPI_write16 (unsigned short data)    	// 	send a 16bit word and use fsync
{

	unsigned char MSdata = ((data>>8) & 0x00FF);  	//filter out MS
	unsigned char LSdata = (data & 0x00FF);			//filter out LS

	PORTB &= ~_BV(PB0);						// 	Fsync Low --> begin frame
	
	SPDR = MSdata;							// 	send First 8 MS of data
	while (!(SPSR & (1<<SPIF)));			//	while busy

	SPDR = LSdata;							// 	send Last 8 LS of data
	while (!(SPSR & (1<<SPIF)));			//	while busy

	PORTB |= _BV(PB0);						// 	Fsync High --> End of frame
}


/*************************************************************************
Function: Freq_change()
Purpose:  change the frequency and select AD9833 onboard register
Input:    unsigned short freq_out = frequency, unsigned int select = register 0 or 1
Returns:  none
Comment:  uses 14 bit filter and adds control words, 
**************************************************************************/
void Freq_change ( unsigned short freq_out, unsigned int select )  // take base10 frequency and do frequency hop
{

unsigned long freq_reg = freq_out * 13.4217728; 	// make freq register from frequency // set for 20 MHz Mclk
unsigned short MS_reg = ((freq_reg>>14) & 0x3FFF);  // filter out MS -- make 2 x 14 bit frequency words
unsigned short LS_reg = (freq_reg & 0x3FFF);		// filter out LS -- make 2 x 14 bit frequency words

MS_reg += 0x4000; 									// add control bits hex = 0x4000
LS_reg += 0x4000; 									// add control bits hex = 0x4000

if (select == 0 ) { SPI_write16(0x2000);}			// prep ad9833 to recieve full 28bit word for freq 0
if (select == 1 ) { SPI_write16(0x2800);}			// prep ad9833 to recieve full 28bit word for freq 1

SPI_write16(LS_reg);								// send the LS word first, to the ad9833
SPI_write16(MS_reg);								// send the MS word last,  to the ad9833
}


/*************************************************************************
Function: AD9833_init()
Purpose:  Init the AD9833
Input:    none
Returns:  none
Comment:  this function isn't nessecary, can be done manually
**************************************************************************/
void AD9833_init (void)
{

SPI_write16(0x2100);		// control word, set output to mid value voltage 

SPI_write16(0x7288);		// Freq0 registerdata MSB  = approx. 29 khz
SPI_write16(0x4017);		// Freq0 registerdata LSB  = approx. 29 khz

SPI_write16(0xACEA);		// Freq1 registerdata MSB  = approx. 24 khz
SPI_write16(0x8013); 		// Freq1 registerdata LSB  = approx. 24 khz

SPI_write16(0xC000);		// Phase offset of Freq0 = 0
SPI_write16(0xE000);		// Phase offset of Freq1 = 0

SPI_write16(0x2000);		// control word, set output = sine

}


