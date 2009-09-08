
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd_4-8.h"


#define ADC_start ADCSRA |= (1<<ADSC)

uint8_t sw_data = 0;

char ADC_buf[4];

/*********************************************************************************
* Initialize ADC
*********************************************************************************/
void adc_init(void)
{
	// REFS0 AVCC with external cap on AREF pin
	// ADLAR Left adjust for 8 bit resolution... and select channel "7"
	ADMUX = 1<<REFS0 | 1<<ADLAR | 1<<MUX2 | 1<<MUX1 | 1<<MUX0;
	
	// The ADC enable (ADEN = 1)
	// Prescaler = 128
	ADCSRA = 1<<ADEN | 1<<ADPS2 | 1<<ADPS1 |1<<ADPS0;
	
}	// end adc_init

/*********************************************************************************
* Initialize External Interrupt 0
**********************************************************************************/

void EXT_INT0_init(void)
{
	MCUCR = (1<<ISC01);	//interrupt 0 falling edge detection
	GICR = (1<<INT0);	//enable external interrup 0 

}	// end int0 init

/*********************************************************************************
* External interrupt_0 handler--- Do Not Block RTC interrupt ---
*********************************************************************************/

ISR(INT0_vect, ISR_NOBLOCK)	 // -TODO-  check with RTC int
{
	ADC_start;	//Start a conversion to see if a switch is pressed

	while(!(ADCSRA & 0x10));	//Loop until ADIF set

	ADCSRA |=(1<<ADIF);	// Clear the ADIF flag

	sw_data = ADCH >> 4;	// ADCLAR = 1 so we use only 8 bits

	itoa(sw_data,ADC_buf,16);	// convert to string

	lcd_clrscr();	//TEST

	lcd_puts(ADC_buf);	// TEST
}
/*********************************************************************************
* 
*********************************************************************************/

int main(void)
{

	cli();			//Stop interrupts
	adc_init();
	lcd_init();	
	lcd_clrscr();
	EXT_INT0_init();	

	sei(); 	//  Enable global interrupts 

	while(1);
	{


	}
	
return 0;  
}
