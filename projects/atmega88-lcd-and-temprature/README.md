# Atmega88--> LCD and temprature

Uploaded by riddhesh on 2011-03-03 14:32:00 (rating 0 out of 5)

## Summary

Dear Freaks,  

I am interfacing At mega 88 with LCD and temperature sensor(LM35).  

My interface is ok.  

But i have some issues with values of LM 35.  

I have directly connected LM35 with At mega88(at 10bit ADC input).  

My values on LCD are varying very rapidly.  

and when i disconnect LM35, then there are garbage values displayed on my LCD.  

what can be done in order to remove this garbage when my LM35 is disconnected.  

and how can I minimize the variations in LM 35 readings.


For Better Reference I am Pasting code below.  

  

FREAKS PLEASE HELP!! 


 
//Program to use internal ADC of AVR microcontroller using interrupts
#include
#include
#include 
///////////////////////////////////////////////////
#ifndef F\_CPU
#define F\_CPU 20000000UL
#endif
#define lcd PORTD
#define rs PB0
#define rw PB1
#define en PB2
/////////////////////////////////////////////////////
void adc\_init(void);
unsigned int adc\_read(void);
void adc\_conv();
void lcd\_init(void);
void lcdcmd(unsigned char);
void lcddata(unsigned char);
unsigned int adc\_out;
//////////////////////////////////////////////////////
void adc\_init() // ADC configuration
{
 ADMUX =(1<<REFS0); // reference at +5V VCC
 ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // Prescaller Set to 128
 ADCSRA |=(1<<ADEN)|(1<<ADIF)|(1<<ADIE); // ADc interrupt enable, Interrupt flag enable
 ADMUX &=~ ((1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0)); //ADC channel 0
 ADMUX =(0<<ADLAR);// ADC for 8-bit precision
}
void adc\_conv()
{
 unsigned int i,a,b;
 char position=0xC3;
 ADCSRA|=(1<<ADSC); //start ADC conversion
 while(!(ADCSRA & (1<<ADIF))); //Pooling for ADC conversion to finish
 {
 ADCSRA &=~ (1 << ADSC);// Switching off the ADC
 adc\_out = ADCW; // transfering ADC result into variable
 ADCSRA |= (1 << ADIF); |


 b=adc\_out/2;


 for(i=0;i<3;i++)  

 {  

 a=b%10;  

 b=b/10;  

 lcdcmd(position);  

 lcddata(48+a);  

 position--;  

 }  

 \_delay\_ms(100);  

 }  

}


void lcd\_init() // fuction for LCD initialization  

{  

 lcdcmd(0x38);  

 lcdcmd(0x0C);  

 lcdcmd(0x01);  

 lcdcmd(0x06);  

 lcdcmd(0x80);  

}


void lcdcmd(unsigned char cmdout)  

{  

 lcd=cmdout;  

 PORTB=(0<<rs)|(0<<rw)|(1<<en);  

 \_delay\_ms(10);  

 PORTB=(0<<rs)|(0<<rw)|(0<<en);  

}


void lcddata(unsigned char dataout)  

{  

 lcd=dataout;  

 PORTB=(1<<rs)|(0<<rw)|(1<<en);  

 \_delay\_ms(10);  

 PORTB=(1<<rs)|(0<<rw)|(0<<en);  

}  

unsigned char data[12]= "TEMPRATURE:";  

int main(void)  

{  

 int i=0;  

 DDRB=0x07;  

 DDRD=0xFF;  

 DDRC=0x00;  

 lcd\_init();  

 while(1)  

 {  

 while(data[i]!='\0')  

 {  

 lcddata(data[i]);  

 \_delay\_ms(1);  

 i++;  

 }  

 adc\_init();  

 adc\_conv();  

 }  

}

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
