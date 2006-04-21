#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#include "routines.h"

//#if !defined(__AVR_ATtiny26__)
//#error Only support ATTINY26 !
//#endif



int main(void)
{
	AUX_DDR = 0xfc;
	init_receiver();

    while (1) 
	{
		disp_status(recv_serial());
		
		//recv_serial();

//		if ( PINC&0x04 ) AUX_PORT |= 0x10; else AUX_PORT&=~0x10;
	//	if ( PINC&0x08 ) AUX_PORT |= 0x20; else AUX_PORT&=~0x20;
	//	if ( PINC&0x10 ) AUX_PORT |= 0x40; else AUX_PORT&=~0x40;
	//	if ( PINC&0x20 ) AUX_PORT |= 0x80; else AUX_PORT&=~0x80;
		if ( (PINC&0x18)==0x18 )  BEEP();
		//BEEP();
		
#if defined(__AVR_ATtiny26__)
		__sleep();
#endif

	}

    return 0;
}

