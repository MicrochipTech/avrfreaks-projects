
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "lcd.h"
#include "ADC.h"


uint16_t EEMEM resTable[165] =  

{
  0,0,0,21000,13500,9800,8000,6300,4600,3800,3200,
  0,19800,16000,10500,6700,4803, 3900, 3100, 2300, 1850, 1550,
  12000,9800, 7200, 5100, 3300, 2500, 2000, 1500, 1100, 900, 750,
  5200, 4700, 3200, 2350, 1800, 1300, 980, 750, 575, 430, 350,
  2800, 2000, 1400, 1050, 840, 630, 470, 385, 282, 210, 170,
  720, 510, 386, 287, 216, 166, 131, 104, 80, 66, 51,
  384, 271, 211, 159,123, 95, 77, 63, 52, 45, 38,
  200, 149, 118, 91, 70, 55, 44, 38, 32, 30, 24,
  108, 82, 64, 51, 40, 31, 25, 21, 17, 14, 12,
  64, 48, 38, 31, 25, 20, 17, 13, 11, 9, 8,
  38, 29, 24, 19, 16, 13, 11, 9, 8, 7, 6,							//75%RH
  23, 18, 15, 12, 10, 9, 7, 6, 6, 5, 4, 							//80%RH
  16, 12, 10, 8, 7, 6, 5, 4, 4, 3, 3, 								//85%RH
  10, 8, 7, 6, 5, 4, 4, 3, 3, 2, 2,									//90%RH
  7, 5, 5, 4, 3, 3, 3, 2, 2, 2, 2,									//95%RH
};

#define bit_clear(a,b) ((a) &= (b)) //Macro for bit clear

void delay();

void updateWeather(uint16_t);

void updateHumi(uint16_t);

uint16_t AD_temp = 0x0200;

uint16_t AD_result; //AD conversion result

uint16_t bitCounter = 0x0200; //Bit counter for AD-conversion

uint8_t loop = 150;

uint8_t curTemp = 250; //Current temperature

uint8_t tempDelay = 0; 





int main(void)
		{
    
	lcd_init();  //initialize LCD-display	
	lcd_printf("Temp");
	lcd_goto(0,1);
	lcd_printf("Humidity");

setupADC();
delay();
setupHumidity();
delay();
setupADC();
sleep_enable();
 sleep_cpu();
for(;;)
 {
 }
}


/*Interrupt vector for timer1 overflow.
Loop 128 times before check analog comparator
to let the RC filter adapt to right voltage*/
ISR(TIMER1_OVF_vect)
{
    
		loop = loop -1;

    if(loop == 1)
	{;
		loop = 150; //Next bit, RC-filter adapt
		if(bit_is_clear(ACSR,5)) //Check analog comparator
		   {
		   
		   AD_temp  &= ~(bitCounter); //AD voltage to high, clear AD last bit
          
		   }
	   
	    bitCounter >>= 1; //Shift bit counter right

		 if(bit_is_set(SREG, 0)) //Carry bit set?
			{  	 
			 loop = 150; //Set loop for next conversion
			 AD_result = AD_temp; //copy AD result
			 AD_temp = 0x0200; //clear temp
			 bitCounter = 0x0200; //Set bitcounter for new conversion
			 
             updateWeather(AD_result);
			 OCR1A = 0x0200;  //PWM length

			return;
			}	
			
		  else
		     { 
					AD_temp |= bitCounter; //Set next pwm bit to one
					OCR1A = AD_temp; //PWM output length
			 }		 
	}
}

/*Update wheater function*/
void updateWeather(uint16_t AD)
{ 
	/* Every bit represents 0,00488758 voltage
	 25C is 2,982V
	 */
	 uint16_t temp = 616;  //software calibration of temp
	 
	 uint16_t temperatur = 250;

if(AD < temp)
        { 
			while(AD < temp)
			    {
				  AD++;
				  temperatur -= 5; 
		      	}
		}
		
if(AD > temp)
        { 
			while(AD > temp)
	           {
				  AD--;
				  temperatur = temperatur + 5; 
		      	}
		}
	if(temperatur > max_temp)
	  max_temp = temperatur;
    if(min_temp > temperatur)
	  min_temp = temperatur;
	
	curTemp = temperatur;

	lcd_update_weather(temperatur);
	delay();
}



/*
*Interrupt vector for humidity sensor
*/
ISR(TIMER1_COMPA_vect)
{
loop = loop -1;

    if(loop == 1)
	{

		if(bit_is_clear(ACSR,5)) //Check analog comparator
		   {
		   
		   AD_temp  &= ~(bitCounter); //AD voltage to high, clear AD last bit
          
		   }
	    loop = 128;
	    bitCounter >>= 1; //Shift bit counter right

		 if(bit_is_set(SREG, 0)) //Carry bit set?
			{  	 
			 loop = 128; //Set loop for next conversion
			 AD_result = AD_temp; //copy AD result
			 AD_temp = 0x0200; //clear temp
			 bitCounter = 0x0200; //Set bitcounter for new conversion

             updateHumi(AD_result);
			 OCR1A = 0x0200;  //PWM length
		
			 PORTD = 0x00; //disable humidity sensor,
			
			
		    TIMSK = 0x00;
			return;
			}	
			
		  else
		     { 
					AD_temp |= bitCounter; //Set next pwm bit to one
					OCR1A = AD_temp; //PWM output length
			 }	
}
}


void updateHumi(uint16_t AD)
 { 
 	char res[8]; //3 digit result
    uint16_t laskuriX = 0; //x-axis of resistance table
	uint16_t laskuriY = 0;
	uint32_t resVal; //resistance value
    
    uint16_t temp = 0;

	uint16_t eeprom_high; //eeprom value
	uint16_t eeprom_low; //eeprom read value
	uint8_t RH = 0; //Humidity value


	while((temp + 25) < curTemp)
	{
	  temp += 50;
	  laskuriX++;
	}

    resVal = 110000/(((AD)/1.023)*5); //Resistance value in kOhm	
	 resVal -= 22;
   
   laskuriX *= 2; //read words from eeprom;
   eeprom_low = eeprom_read_word(laskuriX);
   uint8_t loo=0;
    /**
	*Read resistance values from eeprom*
	*/
	for(loo; loo < 15; loo + 22)
     {
	  RH += 5;
	  if(eeprom_low < resVal)
	   { 
		eeprom_high = eeprom_read_word(laskuriX+laskuriY-22);
		break;
	   }
	  laskuriY += 24;
	  eeprom_low = eeprom_read_word(laskuriX+laskuriY);
	}
	

    if((eeprom_high - resVal) < (resVal - eeprom_low))
	  RH -= 5;


	

	ltoa(RH, res, 10); //Convert int to string
    res[7] = '\0';
    lcd_goto(9,1); //move to line 2 
	lcd_printf(res); //print result
	char c = 0x25; //print %
	lcd_print(c);
}


void delay()
{
	int i=0;
	for(i; i<1000; i++)
	{
		int j = 1;
		for(j; j < 1000; j++)
		{
		 ;
		 }
	}
}


