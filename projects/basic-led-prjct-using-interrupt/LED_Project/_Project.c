/* LEDproject.c
*  Author: Sean Mongeon
*  Date: 24-Nov-2007
*  Description: Turns on (illuminates) an LED for each time a user presses a
*               switch.  1 press turns on 1 LED.  2 presses turns on 2 LEDs.
*               Works up to 3 LEDs.
*/

#include <avr/io.h>
#include <avr/interrupt.h>

ISR(INT0_vect)
{
	static int LEDoutput = 0; /* counter variable */

	LEDoutput += 1; /* increment each time interrupt is triggered */

	if(LEDoutput > 3)
		LEDoutput = 0; /* reset counter */
	
	/* handle LED outputs */	
	switch(LEDoutput)
	{
		case 0:
			PORTB = 0;
			break;

		case 1:
			PORTB = (1<<PB1);
			break;

		case 2:
			PORTB = (1<<PB1)|(1<<PB3);
			break;

		case 3:
			PORTB = (1<<PB1)|(1<<PB3)|(1<<PB4);
			break;
	}
}

int main(void)
{
	/* set PORTB pins 1, 3, 4, 5 to output */
	DDRB = (1<<DDB1)|(1<<DDB3)|(1<<DDB4);

	/* setup external interrupt INT0 */
	SREG = (1<<SREG_I); /* enable global interrupts */
	MCUCR = (1<<PUD)|(1<<ISC01)|(0<<ISC00); /* set interrupt to trigger on low level of INT0 */
	GIMSK = (1<<INT0); /* enable External interrupt 0 */	

	/* wait for interrupt */
	while(1)
	{}
}





