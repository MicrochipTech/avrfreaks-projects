//*****************************************************************
//       ************    ADC ROUTINES **********
//*****************************************************************
//Controller:	ATmega32 (16 MHz)
//Compiler:		ICCAVR Ver
//Author:		CC Dharmani, Chennai, India
//Date:			August 2008
//*****************************************************************

#include <iom32v.h>
#include <macros.h>
#include "ADC_routines.h"
#include "UART_routines.h"

unsigned char valueDisplay[]=":  .   volt";

float Vref = 5.00; // Reference voltage Vref of ADC

//*****************************************************************
//Purpose :       Initialize the ADC
//*****************************************************************
//ADC initialize
// Conversion time: 52uS
void ADC_init(void)
{
 ADCSRA = 0x00; //disable adc
 ADMUX = 0x40; //select adc input 0
 ADCSRA = 0x86;
}


/********************************************************************
*   Purpose :       Do a Analog to Digital Conversion
*	Paramtr	:		none
*   return	: 		intger temperature value
********************************************************************/
int ADC_read(void)
{
    char i;
    int ADC_temp, ADCH_temp;
    int ADC_var = 0;
    
    ADC_ENABLE;
    
    ADC_START_CONVERSION;		//do a dummy readout first
	
    while(!(ADCSRA & 0x10));    // wait for conversion done, ADIF flag active
	
	ADCSRA|=(1<<ADIF);
        
    for(i=0;i<8;i++)            // do the ADC conversion 8 times for better accuracy 
    {
	 	ADC_START_CONVERSION;
        while(!(ADCSRA & 0x10));    // wait for conversion done, ADIF flag active
        ADCSRA|=(1<<ADIF);
		
        ADC_temp = ADCL;            // read out ADCL register
        ADCH_temp = ADCH;    // read out ADCH register        
		ADC_temp +=(ADCH_temp << 8);
        ADC_var += ADC_temp;      // accumulate result (8 samples) for later averaging
    }

    ADC_var = ADC_var >> 3;     // average the 8 samples
        
	ADC_DISABLE;
	
    return ADC_var;
}


/*****************************************************************************
* Purpose	:	To calculate temp in degre celsius
* Paramtr	:	Integer value of temp, received from ADC read
* Return	:	float temp value(in deg celsius)
*****************************************************************************/
float ADC_calculateValue(int inputValue)
{
    float actualValue;
    
	actualValue=(inputValue * Vref/1024.0);    
									 //calculates the voltage present
	return actualValue;
  }
  
/*****************************************************************************
* Purpose	:	To update the tempDisplay string based on the latest temp read
* Paramtr	:	Float value of temp
* Return	:	String pointer pointing to the updated tempDisplay
*****************************************************************************/
  
 unsigned char* updateDisplay(float actualValue)
 { 	
    
	int temp;
	unsigned char c;
	temp=(int)(actualValue*100.0);   //to include decimal point for display
	
	if((actualValue*100.0 - temp) >= 0.5) temp=temp+1;
	
	valueDisplay[5] = ((unsigned char)(temp%10)) | 0x30;
	temp=temp/10;
   	valueDisplay[4] = ((unsigned char)(temp%10)) | 0x30;
	temp=temp/10;
	valueDisplay[2] = ((unsigned char)(temp%10)) | 0x30;
	temp=temp/10;
	
	return valueDisplay;
       
}

//************************************************

void ADC_transmitValue(void)
{
  int value;
  float value1;
  unsigned char i;
  
  TX_NEWLINE;
  
  for(i=0; i<8; i++)
  {
    ADMUX &= 0xe0;
	ADMUX |= i;   //select channel
	value = ADC_read();
    value1 = ADC_calculateValue(value);
    TX_NEWLINE;
    transmitString_F("   Channel ");
	transmitByte(i | 0x30);
    transmitString(updateDisplay(value1));
  }
    TX_NEWLINE;
	TX_NEWLINE;
}
  