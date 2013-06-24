# 3 canales pwm atmega8

Uploaded by jmsaak on 2013-06-24 01:59:00 (rating 0 out of 5)

## Summary

/*  

 * GENERAR\_PWM\_M8.cpp  

 *  

 * Created: 19/06/2013 04:47:45 p.m.  

 * Author: JesusManuel  

 */ 


#include   

#define F\_CPU 8000000l


#include 


#define color PORTC  

#define control\_red 0  

#define control\_green 1  

#define control\_blue 2


uint8\_t red;  

uint8\_t green;  

uint8\_t blue;


void pwm\_config ()  

{  

 TCCR2 |= (1<<WGM20)|(1<<COM21)|(0<<COM20)|(1<<WGM21)|(1<<CS22)|(0<<CS21)|(1<<CS20);  

 TCCR1A|= (1<<COM1A1)|(0<<COM1A0)|(1<<COM1B1)|(0<<COM1B0)|(0<<WGM13)|(0<<WGM12)|(0<<WGM11)|(1<<WGM10);  

 TCCR1B|= (0<<ICNC1)|(0<<ICES1)|(0<<CS12)|(1<<CS11)|(1<<CS10);


}


void actualiza\_pwm ()  

{  

 OCR1A= red;  

 OCR1B = green;  

 OCR2 = blue;  

}


int main(void)  

{  

 DDRB = 0b11111111;  

 DDRC = 0b11110000;  

 red = 0;  

 green = 0;  

 blue = 0;


 pwm\_config();


 while(1)  

 {  

 if (bit\_is\_set(PINC,control\_red))  

 {  

 red++;  

 actualiza\_pwm();  

 \_delay\_ms(20);  

 }


 if (bit\_is\_set(PINC,control\_green))  

 {  

 green++;  

 actualiza\_pwm();  

 \_delay\_ms(20);  

 }


 if (bit\_is\_set(PINC,control\_blue))  

 {  

 blue++;  

 actualiza\_pwm();  

 \_delay\_ms(20);  

 }


 actualiza\_pwm(); 


 }  

}

## Compilers

- AVR Studio 5

## Tags

- Complete code
- AVR Studio 5
