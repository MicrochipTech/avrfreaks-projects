/* 				
/			PS2_LOW.C
/
/		Low level PS2 protocol driver
/	
/		Author: Visakhan C
/
*/

#include "ps2_low.h"


static volatile uint8_t bitcount,bufferOut,toDevice;
static uint8_t parityOut;

static uint8_t ParityOdd(uint8_t);
extern void Decode(uint8_t);

void PS2_Init(void)
{
	MCUCSR = (0<<ISC2);   // INT2 interrupt on falling edge
	bitcount = 11;
	toDevice = FALSE;
	//edge = 0;
	GICR |= (1<<INT2); //enable INT2 interrupt on
	
}	

/*
void PS2_SendToDevice(uint8_t byteOut)
{	
	//disable INT2
	GICR &= ~(1<<INT2);
	
	toDevice = TRUE;
	bufferOut = byteOut;
	parityOut = ParityOdd(byteOut);
	bitcount = 0;
	
	DDR_CLOCK |= (1<<CLOCK_PIN);
	PORT_CLOCK &= ~(1<<CLOCK_PIN); //pull CLK low
	_delay_us(1);
	DDR_DATA |= (1<<DATA_PIN);
	PORT_DATA &= ~(1<<DATA_PIN);
	DDR_CLOCK &= ~(1<<CLOCK_PIN);  //release clock
	
	//enable INT2
	GICR |= (1<<INT2);
	while(toDevice == TRUE);
}

*/

/* returns Odd parity bit of 'byte' */
static uint8_t ParityOdd(uint8_t byte)
{
	uint8_t count=7,par=1;
	
	do {
	
		if( byte&(1<<count) )
			par ^= 1;
		
	} while(count--);
	
	return par;
}

ISR(INT2_vect)
{
	static uint8_t byteIn;
	
	if(toDevice == FALSE) {
	
		if(bitcount>2 && bitcount<11) {
			
			byteIn = (byteIn>>1);
			if(PIN_DATA & (1<<DATA_PIN) )
				byteIn |= 0x80;
		}
		if(--bitcount == 0) {
		
			Decode(byteIn);
			bitcount = 11;
		}
	}
	else {
	
		if(bitcount<8) {
			
			if(bufferOut & (1<<bitcount))
				PORT_DATA |= (1<<DATA_PIN);
			else
				PORT_DATA &= ~(1<<DATA_PIN);
			bitcount++;

		}
		else if(bitcount == 8) {  //send parity
		
			if(parityOut)
				PORT_DATA |= (1<<DATA_PIN);
			else
				PORT_DATA &= ~(1<<DATA_PIN);
			bitcount++;
		}
		else {
			
			DDR_DATA &= ~(1<<DATA_PIN);  
			PORT_DATA &= ~(1<<DATA_PIN); //release DATA
			
			toDevice = FALSE;
			bitcount = 11;
		}
	}
}