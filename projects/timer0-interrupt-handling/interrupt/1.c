/**********************************************************************************************
	This small example of interrupt handling changes PORTB status each 0,4997sec.
	It was important to me learning how to calculate interruption from TCCNT0 counter.

	TCCNT0 starts countiong from 12 up to 256 (8-bits Counter) if we use prescaler 
	CLK/1024 -> TCCR0 = 5, time to counter overlfow will be 
	[(256-12) * (CK/1024)] = 0,031232sec. to achieve 500ms we need to 16 times achieve 
	counter overflow. 0,031232sec * 16=0,4997sek

	We could also use prescaler: CLK/8 (TCCR0=2), than for TCNT0=156 we have 0,0001sec 
	for each overflow. To achieve 500ms we have to count 50000 overflows.
	The "temp" variable in this case shall be defined as int and this makes code bigger
	
	=================
	uC:			AT90S2313
	CLK:		8MHz
	Compiler: 	WinAVR
	=================

	e-mail: jdebs@wp.pl

/**********************************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#define tau0	0x0C;			// 12dec -> initial value for TCNT0
unsigned char temp;				// temporary variable


// Interrupt handling

ISR (SIG_OVERFLOW0)
{
	
	TCNT0 = tau0;				// inintialize TCNT0 
	temp ++;					// increment counter
	if (temp == 16)				// change PortB status an clear counter
	{
		PORTB^=0xFF;
		temp 	= 0;
	}
	
}


int main ()

{
	DDRB 	=	0xff;        	//PORTB - output
 	DDRD 	=	0x00;        	//PORTD - input
 	PORTD	=	0xFF;          	//Pullup - default "1"
	PORTB	=	0xFF;          	//LEDS OFF
 		
	TIMSK 	= 	1 <<TOIE0;
	TCNT0	= 	tau0;
	TCCR0	=	5;
	
	sei ();


	while (1)
	{	
	
	}

}
