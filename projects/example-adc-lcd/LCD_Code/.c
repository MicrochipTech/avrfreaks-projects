#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <compat/deprecated.h>

#include "lcd.h"
#include "ADC.h"
/************* Gobal Value Declaration****/

unsigned int adc_val=0;

/*************Function Prototype*********/





int main(void)
{
	DDRB |= 0x80;
	DDRC |= 0x00; // AN0, PC0 pin as input for analog  
	
	sbi(PORTB, 7); // set pin for PORTB, 7th to power led for working

	//Initialize LCD module
	InitLCD(LS_BLINK|LS_ULINE);
	//Clear the screen
	LCDClear();
	LCDWriteString("LCD TEST PRINT");
	LCDGotoXY(0,1);
	LCDWriteString("By. HEIKHAMA");
	_delay_ms(100);
	LCDGotoXY(0,1);
	LCDWriteString("ADC COUNT=  ");
	//Initialize ADC
	init_adc();


	while(1)
	{
	
		adc_val = Read_ADC(0); // Reading Channel = 0
		
		LCDWriteIntXY(11,1,adc_val, 5);
		
		_delay_ms(100);

	}


return(0);
}
