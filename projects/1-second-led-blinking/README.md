# 1 second LED blinking

Uploaded by admin on 2011-06-09 06:20:00 (rating 0 out of 5)

## Summary

#include "avr/io.h"  

#include "stdio.h"  

#define F\_CPU 11059200UL  

#include   

#include 


ISR (TIMER1\_OVF\_vect)  

{


PORTB^= 0x01<<PB4;


TCNT1 = 53205;


}


int main()  

{


DDRB = 0xFF;  

TCCR1B = 0x05;  

TCNT1 = 53205;


TIMSK = (0x01<<TOIE1);  

sei();  

 while(1);  

}

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
