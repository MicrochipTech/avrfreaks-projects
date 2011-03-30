# ADC

Uploaded by ghost800 on 2011-03-30 16:45:00 (rating 0 out of 5)

## Summary

// ***********************************************************  

// Project: Analog Comparator example  

// Author: winavr.scienceprog.com  

// Module description: Analog comparator example with positive comparator  

// input connected to Vref 1.23V. When compared voltage exceed 1.23V LED lighst on.  

// When voltage drops bellow - LED turns OFF. Comparator inputis connected to ADC3 pin.  

// ***********************************************************  

#include   

#include   

#define AINpin PA3  

#define LED PD0  

void Init(){  

 DDRA&=~(1<<AINpin);//as input  

 PORTA&=~(1<<AINpin);//no Pull-up  

 DDRD|=(1<<LED); //Led pin as output  

 PORTD|=(1<<LED);//Initally LED is OFF  

 SFIOR|=(1<<ACME);//enable multiplexer  

 ADCSRA&=~(1<<ADEN);//make sure ADC is OFF  

 ADMUX|=(0<<MUX2)|(1<<MUX1)|(1<<MUX0); //select ADC3 as negative AIN  

 ACSR|=  

 (0<<ACD)| //Comparator ON  

 (1<<ACBG)| //Connect 1.23V reference to AIN0  

 (1<<ACIE)| //Comparator Interrupt enable  

 (0<<ACIC)| //input capture disabled  

 (0<<ACIS1)| //set interrupt on output toggle  

 (0<<ACIS0);  

 sei();//enable global interrupts  

}  

// Interrupt handler for ANA\_COMP\_vect  

//  

ISR(ANA\_COMP\_vect) {  

if bit\_is\_clear(ACSR, ACO)  

 PORTD&=~(1<<LED);//LED is ON  

 else PORTD|=(1<<LED);//LED is OFF  

}  

// ***********************************************************  

// Main program  

//  

int main(void) {  

 Init();  

 while(1) { // Infinite loop; interrupts do the rest  

 }  

}

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
