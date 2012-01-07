//Here is the same code written for AVR-GCC (winAVR) compiler:

//******* LED BLINKING PROGRAM *********
//Controller: ATmega32 (1Mhz or 16Mhz)
//Compiler: AVR-GCC (winAVR)
//Author: CC Dharmani, Chennai (INDIA)
//*************************************

//#define F_CPU 1000000UL    //use this if internal oscillator is used
#define F_CPU 16000000UL  //use this when ext 16MHz crystal is used

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void port_init(void)
{
 PORTD = 0x00;
 DDRD  = 0xFF;
}

//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 cli(); //disable all interrupts
 port_init();

 MCUCR = 0x00;
 GICR  = 0x00;
 TIMSK = 0x00; //timer interrupt sources
// sei(); //re-enable interrupts
 //all peripherals are now initialized
}


int main()
{
 init_devices();
 
 while(1)
 {
    PORTD = 0xFF;
    _delay_ms(500);
	
    PORTD = 0;
	_delay_ms(500);
 }
 
 return(0);
}

//---------- END -------------
