/*----------------------------------------------------------------
-----------------HEADER FILES-------------------------------------
-----------------------------------------------------------------*/
#define F_CPU 1600000UL
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <compat/deprecated.h>		//HEADER FILE FOR FUNCTIONS LIKE SBI AND CBI
#include<util/delay.h>				//HEADER FILE FOR DELAY
#include "lcd.h"


unsigned int read_adc(unsigned char);

unsigned int read_adc(unsigned char ch)
{

		ADCSRA = _BV(ADEN) | _BV(ADPS2);
		ADMUX = ch;						// Select pin ADC0 using MUX		
		ADCSRA |= _BV(ADSC);			// Start conversion 		
		while (ADCSRA & _BV(ADSC)) 	// wait until converstion completed
		{
		
		}
		return ADCW;
}

int main(void)
{
    //unsigned char buffer[7];
    unsigned int  data_solar = 0;
	//unsigned int  data_grid = 0;
    
	
	ADCSRA = _BV(ADEN) | _BV(ADPS2);    // Activate ADC with Prescaler 16 --> 1Mhz/16 = 62.5kHz
	DDRB=0x10;						//SET DATA DIRECTION REGISTER
//	DDRC = 0xFC;					//SET 1 for OUTPUT PORT
									//SET 0 FOR INPUT PORT
									//ONLY PORTB.0 IS OUTPUT
									//CONFIGURE AS PER REQUIREMENT
									//DEFAULT ALL OTHERS ARE INPUT
									//CHANGE ACCORDING TO USE
	
	DDRD=0xC0;						//SET DATA DIRECTION REGISTER
									//SET 1 for OUTPUT PORT
									//SET 0 FOR INPUT PORT
									//ONLY PORTD.7 AND PORTD.6 ARE OUTPUT
									//CONFIGURE AS PER REQUIREMENT
									//DEFAULT ALL OTHERS ARE INPUT
									//CHANGE ACCORDING TO USE
	
	sbi(PORTB,4);					//LED1 ON (INDICATION FOR READY TO USE)
	
	//sbi(PORTD,2);					//ENABLE PULL UP FOR SW1
	//sbi(PORTD,3);					//ENABLE PULL UP FOR SW2
    
   //Initialize LCD module
//	InitLCD(LS_BLINK|LS_ULINE);

	//Clear the screen
//	LCDClear();

	//Simple string printing
//	LCDWriteString("Charger ON");
	
	//A string on line 2
	//LCDWriteStringXY(0,1,"TEMP ");
	while(1)
	{
		data_solar = read_adc(0);
		data_solar = (data_solar/1023.0)*5*100;

		if(data_solar > 216)
		{
//			LCDClear();
//			LCDWriteString("SOLAR ON");
//			LCDWriteStringXY(0,1,"CHARGE ON SOLAR ");
			do
			{
				sbi(PORTD, 6);
				
				data_solar = read_adc(0);
				data_solar = (data_solar/1023.0)*5*100;
				
			}while(data_solar > 200);
//			LCDClear();
//			LCDWriteString("Charger ON");
			
			cbi(PORTD, 6);
		}
		else 
		{
//			LCDClear();
//			LCDWriteString("SOLAR OFF");
//			LCDWriteStringXY(0,1,"CHARGE ON GRID ");
			do
			{
				
				cbi(PORTD, 6);
				data_solar = read_adc(0);
				data_solar = (data_solar/1023.0)*5*100;
				
				
			}while(data_solar < 216);
			
			
		}
	}

	return 0;
}
