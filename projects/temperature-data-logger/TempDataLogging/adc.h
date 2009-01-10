//**********************************************************************//
//FILE: adc.h 
//AUTHOR: Adam Kadolph
//DATE:	01-7-09
//DESCRIPTION: Header file to handel A/D Converter definitions and functions
//**********************************************************************//

#ifndef _ADC_H_
#define _ADC_H_

//ADC variable definitions
unsigned char temperature[11];

//ADC function declarations
void ADCinit(void);
void ADCstart(void);
void TempConvert(unsigned char ADC_DATA, unsigned char i);

#endif
