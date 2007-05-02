/*
Author: Juha Viitanen
e-mail: viidakkovekara@gmail.com

You can use this code where ever you want as much as you want :)
If you have any good ideas for this, send e-mail.

*/

#include <avr/io.h>
#include <avr/signal.h>
#include "lcd.h"
#define uchar unsigned char
#define uint unsigned int

uint16_t count_pulses=0, rpm=0;
uint8_t kerroin=1;


SIGNAL(SIG_INTERRUPT0)
	  {
           count_pulses++;	// pulses counted in a second
	  }

/*When overflow occurs, */	  
SIGNAL(SIG_OVERFLOW1)
{
    rpm = count_pulses;   // pulse amount is written to rpm
    count_pulses=0; 		// reset value
}


/* MAIN FUNKTIO STARTS*/
int main(void)
{
    TCCR1A = 0b10000000; // Timer1 in normal mode
    TCCR1B = 0b00000011; // Prescaler settings
    TIMSK = 0x45;		// timer interrupt owerflow
    //DDRA = 0xFF;    
    DDRC = 0xFF;    // pull-up
    InitLCD(3);		// LCD intialized for PORTC
    GICR = 0b11100000; // External int0 activated
    MCUCR = 0b00000111; // Interupt activated on rising edge of the pulse
    SREG = 0b10000000;  // interrupts allowed
    Write(1,1,"rpm:"); 


    while (1) //while
      {       
		WriteInt( 2, 5, 4, 0, rpm*28); //RPM*fix_multiplier
		WriteInt( 2, 10, 4, 0, rpm*30); // RPM*fix_multiplier
		WriteInt( 1, 11, 4, 0, count_pulses); // writes the counted pulse amount to the lcd       
      } //while

} //main

