#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>


#define SCREEN_SIZE 16

#include "adc.h"
#include "lcd.h"


#define keypad_port PORTB
#define keypad_ddr DDRB
#define keypad_pin PINB

#define lcd_port PORTC
#define lcd_ddr DDRC
#define lcd_pin PINC





int main( void )
{

float temp,light;
uint8_t i,loop;

	MCUCSR = 0x80;			//disable jtab to make portC to work
	MCUCSR = 0x80;

lcd_ddr=0xFF;
lcd_port=0xFF;

//Initialize LCD module
InitLCD(LS_BLINK|LS_ULINE);

LCDClear();


InitADC();


LCDWriteStringXY(0,0,"***PANOSCURE***");


while(1)
{




      temp=ReadADC(2);  //Read ADC Channel 0
	  temp=(5*temp*100)/1024; //this algorithm gives the actual temperature
	  
      light=ReadADC(3);  //Read ADC Channel 1


      LCDWriteStringXY(0,1,"***Temp*Light***");
      LCDWriteStringXY(2,2,"Temp = ");
	  //LCDWriteStringXY(2,3,"Light = ");

      LCDWriteDecimals(9,2,temp,2);	//this function returns the value with 2 decimals
	  //LCDWriteDecimals(9,3,light,3);
	  		
	LCDWriteStringXY(0,3,"                ");

	if(light>950)
	{loop=0;}
	else if(light<950 && light>900)
	{loop=1;}
	else if(light<850 && light>800)
	{loop=2;}
	else if(light<800 && light>750)
	{loop=3;}
	else if(light<750 && light>700)
	{loop=4;}
	else if(light<700 && light>650)
	{loop=5;}
	else if(light<650 && light>600)
	{loop=6;}
	else if(light<600 && light>550)
	{loop=7;}
	else if(light<550 && light>500)
	{loop=8;}
	else if(light<500 && light>450)
	{loop=9;}
	else if(light<450 && light>400)
	{loop=10;}
	else if(light<400 && light>350)
	{loop=11;}
	else if(light<350 && light>300)
	{loop=12;}
	else if(light<300 && light>200)
	{loop=13;}
	else if(light<200 && light>100)
	{loop=14;}
	else if(light<100)
	{loop=15;}


    for(i=0;i<loop;i++)
	{
	LCDGotoXY(i,3);
	LCDByte(255,1);
	}


	

      _delay_ms(1000);



}


}
