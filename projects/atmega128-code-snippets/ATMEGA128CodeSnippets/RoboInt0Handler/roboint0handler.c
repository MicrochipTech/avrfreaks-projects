#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr\iom128.h>
 

//#define outp(a, b) b = a
//#define inp(a) a

//uint8_t flag;
unsigned char temp;


//typedef unsigned char  u08;

ISR(INT0_vect) { /* signal handler for external interrupt int0 */


	temp = ~temp;
	PORTB = temp;

}

int main(void) {

 
	DDRB = 0xFF;
	PORTB = 0x00;
	DDRD = 0x00;
	PORTD = 0xFF;

	cli();
	EICRA = 0x0F;
	EICRB = 0x00;

    EIMSK = 0x03; // enable external int0

    sei();       /* enable interrupts */

    while(1); 

}
