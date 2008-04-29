/******************************************************************************
Test extrnal interrupts
*
*Jidan Al-Eryani
*******************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>


//variables that are accesed in ISR's and other code areas of a program, have to be declared with voltatile
volatile uint8_t state;


// This function is Interrupt service routine (ISR)
ISR(INT4_vect)     
{
    PORTC = ~PORTC;
	//EIFR &= ~(1<<INTF4);
}


int main(void)
{    
    DDRC  = 0xff;                // set PortC as output, ouput=1
    PORTC = 0xff;                

    DDRE  = 0x00;                // use all pins on port E for input
    PORTE = 0xff;                // activate internal pull-up
    
    EIMSK |= (1<<INT4);           // enable external int4
    EICRB |= (1<<ISC40);          // trigger on any level change
    
    sei();                       // enable interrupts 
    
    for (;;) {}                  // loop forever   
}


