#include <mega128.h>
#include <stdlib.h>
#include <ok128.h>
#include <delay.h>
  
#define FALSE	0
#define TRUE	1

#define ON	1
#define OFF	0	

#define SONAR_IN	PINB.5 
#define SONAR_OUT	PORTB.4
#define CRYSTAL	16000000


void setupSonar(void)
{
	DDRB.4 = 1;		//trigger as output
	DDRB.5 = 0;		//input pin
	PORTB.5 = 1;		//pull up?
}
           
unsigned long int readSonar(void)
{	
	unsigned long int data = 0;
	unsigned long int count = 0;  	
	double time = 0;
      
 	//DDRB &= 0xFF;
  
	SONAR_OUT = ON;
	delay_us(10);	
	SONAR_OUT = OFF;

	//PORTB.6 = ON;     	
	
	while(SONAR_IN == OFF)
	{	
		data++;
		
		if (data >= 4294967295)
		{
			count++;
			data = 0;   
			
			if (count > 1 || data > 576000) return 0; //sensor is not attached or obstacles out of range
		} 				  
	}			
	
	data = 0; 
	count = 0 ;
	
    	while(SONAR_IN == ON)
    	{
		data++; 		
			
	    	if (data >= 4294967295)
		{			
			count++;

			if (count > 1 || data > 576000) return 0; //sensor is not attached or obstacles out of range
			
			data = 0;      
		} 
    	}   	
	    	
    	time = ((count * 4294967295) + data)/(2) ;
    	//time = ((count * 4294967295) + data)/2;
	//PORTB.6 = OFF;
	    	
    	return time;  	    	
}


float distance(void)
{
	return readSonar()/10;                  
}


void main(void)
{
	double raw_time = 0;
	char	 buffer[32] = {NULL};
	int angka = 0;
	
	MCU_initialize();
	delay_ms(50);
	
	LCD_initialize();
	LCD_clear();	

	setupSonar(); 

	while(1)
	{
		//raw_time = readSonar();
		//delay_ms(50);   
		
		//ftoa(raw_time, 2, buffer);
		//delay_ms(50);
		
		//LCD_puts(buffer);					

		ftoa(distance(), 2, buffer);
		delay_ms(50);		
		LCD16_gotxy_ok128(0, 1);
		LCD_puts(buffer);		
		LCD_putsf(" cm");
		
		delay_ms(100);		
		LCD_clear();
	}	
		
}


//Buat debug
/*
	while(1)
	{
		if (SONAR_IN == ON)
		{
			angka++;
			itoa(angka,buffer);
			LCD_clear();
			LCD_puts(buffer);
			delay_ms(500);
			
		}
	} 
*/