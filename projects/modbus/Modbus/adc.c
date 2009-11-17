#include <iom8535v.h>
#include <macros.h> 
#include "ADC.h"

#pragma interrupt_handler ADC_interrupt: iv_ADC;
static int ADC_val;

void ADC_interrupt( void )
{
 	 ADC_val = ADCL;
	 ADC_val += (ADCH << 8); 	 
}

/*****************************************************************************
*
*   Function name : ADC_init
*
*   Returns :       None
*
*   Parameters :    char input
*
*   Purpose :       Initialize the ADC with the selected ADC-channel
*
*****************************************************************************/
void ADC_init(char input)
{
  
    ADMUX = input;    // external AREF and ADCx
    
    ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1);    // set ADC prescaler to , 8MHz / 64 = 125kHz    

    input = ADC_read(0);        // dummy 
}


/*****************************************************************************
*
*   Function name : ADC_read
*
*   Returns :       int ADC
*
*   Parameters :    n - chanel #
*
*   Purpose :       Do a Analog to Digital Conversion
*
*****************************************************************************/

int ADC_read(char n)
{
 int ADC_value;      /* NOTE: the Bug was ADC_value should be an int & not unsigned char ! */ 

// read ch n of internal 10 bit a/d
  ADMUX &= 0xE0;
  ADMUX |=n;       //select channel n
  	
  ADCSRA |= (1<<ADEN); 
  // sbi(ADCSRA, ADEN);     // Enable the ADC

  ADCSRA |= (1<<ADSC);        // do single conversion
  while(!(ADCSRA & 0x10));    // wait for conversion done, ADIF flag active
    
  ADC_value = ADCL; // read lower bytes first
  ADC_value += ADCH << 8; // sample at 10-bit resolution

 return ADC_value; 
}
