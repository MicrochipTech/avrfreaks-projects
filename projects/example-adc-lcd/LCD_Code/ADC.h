#include <avr/io.h>
#include <inttypes.h>
#include <avr/pgmspace.h>


#ifndef _ADC_H
#define _ADC_H

#define XTAL 1000000              /**< clock frequency in Hz, used to calculate delay timer */



void init_adc(void);
unsigned int Read_ADC(unsigned char channel);



#endif
