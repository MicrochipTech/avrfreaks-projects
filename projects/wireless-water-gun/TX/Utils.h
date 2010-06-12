#ifndef UTILS_H
#define UTILS_H


#include <avr/io.h>         //include I/O definitions
#include <avr/interrupt.h>	// include interrupt support
#include <util/delay.h>     //include delay support

#define ADC_enable          ADCSRA |= (1 << ADEN)
#define start_conversion    ADCSRA |= (1 << ADSC)
#define ADC_ADMUX_mask      0x60  //01100000 

#define OFF 10   //off data
#define LOW 20   //low power data byte
#define MED 30   //med power data byte
#define HIGH 40  //  high power data byte

#define enable_interrupts   SREG |= (1 << 7)
#define disable_interrupts  SREG &= ~(1 << 7) 


//define port functions
#define PORT_ON( port_letter, number )			port_letter |= (1<<number)
#define PORT_OFF( port_letter, number )			port_letter &= ~(1<<number)
#define PORT_ALL_ON( port_letter, number )		port_letter |= (number)
#define PORT_ALL_OFF( port_letter, number )		port_letter &= ~(number)
#define FLIP_PORT( port_letter, number )		port_letter ^= (1<<number)
#define PORT_IS_ON( port_letter, number )		( port_letter & (1<<number) )
#define PORT_IS_OFF( port_letter, number )		!( port_letter & (1<<number) )


//function definitions
void initialize_PORTS();
void initialize_PWM();
char get_position();
void startup();


#endif
