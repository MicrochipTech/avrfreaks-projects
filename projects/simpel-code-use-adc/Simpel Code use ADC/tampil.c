#include<mega32.h> 
#include<delay.h>
#define mode_ADC 0x20;
unsigned int baca_adc(unsigned char pin_adc);
unsigned int Level,Suhu;   
void io_init(); 
void ADC_init();
void main(void)
{        
	 io_init();
	 ADC_init();
	 while(1)
	 { 
       	 Level=baca_adc(0);
         Suhu=baca_adc(1);
         PORTC=Level;	//PORTC to output ADC 0
         PORTD=Suhu;
         delay_ms(200); // just to give mikro not to work hard u can use 300 ,500, until 1000
         }
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>fungsi-fungsi<<<<<<<<<<<<<<<<<<<<<<<<
unsigned int baca_adc(unsigned char pin_adc)
{
	ADMUX=pin_adc|mode_ADC;
	ADCSRA|=0x40;
	while((ADCSRA & 0x10)==0);
	ADCSRA|=0x10;
	return ADCH;//>>1;//ADCW>>1;
} 	 
void io_init()
{  

//--------------- output	
	DDRD=0xff;
	DDRC=0xff;
//--------------- seting port A 0 dan 1 to input ADC
	DDRA.0=0;
	DDRA.1=0;
}  
void ADC_init()
{
	ADMUX=mode_ADC;
	ADCSRA=0x87; //prescaler  87  
}