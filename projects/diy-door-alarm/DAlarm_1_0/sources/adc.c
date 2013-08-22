/*
 * adc.c
 *
 * Created:
 * Author: PjV
 * Version: 1.0
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "defines.h"
#include <util/delay.h>

#include "main.h"
#include "adc.h"

static volatile unsigned char ADC_newSample;// = 0;


/*********************************************************************************
 *
 *********************************************************************************/
void ADC_Init(void){

	ADC_newSample = 0;

	ADMUX |= (1<<ADLAR);	// Left Adjust Result => 8bit precision => read only ADCHibyte 
	
	//ADMUX |= (1<<REFS0);  //External voltage reference at AREF pin, internal voltage reference turned off	
	//ADMUX |= (1<<REFS1);  //Internal 2.56V voltage reference (VCC > 3.0V),
							//without external bypass capacitor, disconnected from AREF. 
							//AD-signal to PORTA:0, ADC channel 0
	//ADCSRB |= (1<<REFS2);	//Bit4 – REFS2: tämä bitti admuxin lisäksi valitsee AREFin.

	ACSRA = 0b10000000;		//Disable Analog Comparator. Reduce power consumption.	
	DIDR0 |= (1<<ADC0D)|(1<<AREFD);	//disable digital inputs of AREF(internal)-pin and
									//adc0-channel-pin(power safe). Reduce power consumption.
	// sys clk 1Mhz, ADC-clk usually min. 50k to 200k...
	//ADCSRA |= (1<<ADPS0);
	//ADCSRA |= (1<<ADPS1);  	//Use prescale factor 8 -> ADC clock is 1MHz/8
	ADCSRA |= (1<<ADPS2);		//Use prescale factor 16 -> ADC clock is 1MHz/16
	ADCSRA |= (1<<ADIE); 		//Enable ADC conversion complete interrupt
	ADCSRA |= (1<<ADEN); 		//Enable the ADC
	
	//Start Conversion
	ADCSRA |= (1<<ADSC); 	//This first conversion performs initialization of the ADC.
}

/*********************************************************************************
 * Determine ADC-level-status
 *********************************************************************************/
ADC_STATUS_t ADC_ProcessData(void){

	ADC_STATUS_t return_value = UNKNOWN;

	if( ADC_newSample > 5 && ADC_newSample < 75 ){ // 100mV -- 1.5V OK
		return_value = DOOR_CLOSED_OK;
	}else if( ADC_newSample > 200){	// >4V SWITCH_BYPASS
		return_value = DOOR_BYPASS_PRESSED;
	}else if( ADC_newSample < 6){		// <100mV DOOR_OPEN
		return_value = DOOR_OPEN_ALARM;
	}

	ADCnewSampleReady_g = FALSE;
	_delay_ms(200);		//button 
	ADCSRA |= (1<<ADSC);//Start new conversion
	
	return return_value;
} 

/*********************************************************************************
 * ADC Complete Interrupt Service Routine
 *********************************************************************************/
ISR(ADC_vect)
{
	ADC_newSample = ADCH;		//8-bit resolution, left shift, result in hi-register
	ADCnewSampleReady_g = TRUE;
}
