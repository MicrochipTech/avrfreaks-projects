#include <avr\io.h>
#include <avr\delay.h>
#include <lcdlib.h>
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
int j;

void adc_init(void)
{
      ADMUX = 0b01000000; //0x40
      ADCSRA |= 0x80;
}
int readadchan(char n)
{
//read ch n of internal 10 bit a/d

      ADMUX = n;
      ADMUX |= 0x40;
      ADCSRA |= 0x44;

      while((ADCSRA & 0x40) !=0){}; //wait for conv complete
      //while((ADCSRA & ADIF) ==0); 

      return ADC;
}

int main(void)
{
      DDRA = 0x00;
      DDRD = 0xFF;
      PORTD = 0x00;
      LCD_init();
      
      unsigned char *k1;
      unsigned char *k2;
      unsigned char *m1;
      unsigned char *m2;
      k1="";
      k2="";
      
      LCD_Send_String("Read1 : ");
      LCD_Send(0xC0,0);
      LCD_Send_String("Read0 : ");
      
      int a0;
      int a1;
      int s;
      adc_init();
      
      while(1)
      {                 
            a1 = readadchan(0x01);
            a0 = readadchan(0x00);
 
            m1 = itoa(a1,k1,10);
           
            LCD_Send(0x88,0);
            LCD_Send_String(k1);      
            LCD_Send(0xC8,0);
            m1 = itoa(a0,k1,10);
            LCD_Send_String(k1);      
            _delay_ms(100000);
      }
}
