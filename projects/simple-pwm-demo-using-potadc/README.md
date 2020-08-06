# Simple PWM demo using a Pot(ADC)

Uploaded by ki0bk on 2020-08-05 21:01:00 (rating 0 out of 5)

## Summary

ICCAVR v8 project for the Atmega8 to read a voltage from a Pot using the ADC and use that value to set the PWM duty cycle, as a bonus, send the ADC value to the USART.


Should be easy to port to other C compilers or classic AVR's (M48-M328,M2560, etc)


 


/*  

 * Compiler ImageCraft C for Mega8(16)  

 * Read poteniometer connected to ADC0 and set PWM duty cycle and  

 * send ADC value to serial USART  

 */  

#define F\_CPU 1000000UL  

#include <iccioavr.h> //<avr\io.h>  gcc io header  

#include <stdint.h>  

#include <stdio.h>  

//#include <delay.h>  //not used


 


enjoy

## Compilers

- ImageCraft ICCAVR V6

## Tags

- Complete code
- ImageCraft ICCAVR V6
