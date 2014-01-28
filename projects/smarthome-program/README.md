# SmartHome Program

Uploaded by akp500 on 2014-01-28 08:37:00 (rating 0 out of 5)

## Summary

For open the door u have to input right code with push button


#include   

#include 


int x =11;  

int y =11;  

int z =11;  

int l =0;  

int p =11;  

void main(void)  

{


PORTA=0x00;  

DDRA=0x00;


PORTB=0xFF;  

DDRB=0x00;


PORTD=0x00;  

DDRD=0xFF;


TCCR0=0xFF;  

TCNT0=0xFF;  

OCR0=0xFF;


TCCR1A=0xA1;  

TCCR1B=0x0D;  

TCNT1H=0x00;  

TCNT1L=0x00;  

ICR1H=0x00;  

ICR1L=0x00;  

OCR1AH=0x00;  

OCR1AL=0x00;  

OCR1BH=0x00;  

OCR1BL=0x00;


ASSR=0x00;  

TCCR2=0xFF;  

TCNT2=0xFF;  

OCR2=0x00;


MCUCR=0x00;  

MCUCSR=0x00;


TIMSK=0x00;


ACSR=0x80;  

SFIOR=0x00;  

OCR1A=0xFF;  

OCR1B=0xFF;


while (1)  

 {  

 if(l==0)  

 {  

 if(PINB.0==0)//PB1  

 {  

 x=0;  

 l=1;  

 delay\_ms(100);  

 }  

 if(PINB.1==0)//PB2  

 {  

 x=1;  

 l=1;  

 delay\_ms(100);  

 }  

 if(PINB.2==0)//PB3  

 {  

 x=2;  

 l=1;  

 delay\_ms(100);  

 }  

 if(PINB.3==0)//PB4 and etc (i use paralel PB)  

 {  

 x=3;  

 l=1;  

 delay\_ms(100);  

 }  

 }


 if(l==1)  

 {  

 if(PINB.0==0)  

 {  

 y=0;  

 l=2;  

 delay\_ms(100);  

 }  

 if(PINB.1==0)  

 {  

 y=1;  

 l=2;  

 delay\_ms(100);  

 }  

 if(PINB.2==0)  

 {  

 y=2;  

 l=2;  

 delay\_ms(100);  

 }  

 if(PINB.3==0)  

 {  

 y=3;  

 l=2;  

 delay\_ms(100);  

 }  

 }


 if(l==2)  

 {  

 if(PINB.0==0)  

 {  

 z=0;  

 l=3;  

 delay\_ms(100);  

 }  

 if(PINB.1==0)  

 {  

 z=1;  

 l=3;  

 delay\_ms(100);  

 }  

 if(PINB.2==0)  

 {  

 z=2;  

 l=3;  

 delay\_ms(100);  

 }  

 if(PINB.3==0)  

 {  

 z=3;  

 l=3;  

 delay\_ms(100);  

 }  

 }  

 if(l==3)  

 {  

 if(x==0 & y==1 & z==2) // Motor ON, and the door has opened  

 {  

 PORTD=0x01;  

 OCR1B=100;  

 if(PINB.4==0)  

 {  

 l=4;  

 delay\_ms(50);  

 }


 }  

 else // If u input wrong password, the buzzer on  

 {  

 PORTA=0x01;  

 delay\_ms(200);  

 PORTA=0x00;  

 delay\_ms(100);  

 x=11;  

 y=11;  

 z=11;  

 p=11;  

 l=0;  

 }  

 }


 if(l==4) //limit switch for stop the motor DC  

 {  

 PORTD=0x00;  

 OCR1B=0;  

 if(PINB.5==0)  

 {  

 l=5;  

 }  

 }  

 if(l==5) //sensor to close the door  

 {  

 PORTD=0x02;  

 OCR1B=100;  

 PORTA=0x02;  

 if(PINB.6==0)  

 {  

 l=6;  

 }  

 }  

 if(l==6)// limit switch to the motor  

 {  

 PORTD=0x00;  

 OCR1B=0;  

 if(PINB.7==0) // PB for open the door from inside of the home  

 {  

 p=1;  

 delay\_ms(100);  

 l=7;


 }  

 if(l==7)  

 {  

 PORTD=0x01;  

 OCR1B=100;  

 delay\_ms(200);  

 PORTD=0x00;  

 OCR1B=0;  

 delay\_ms(200);  

 PORTD=0x02;  

 OCR1B=100;  

 delay\_ms(200);  

 PORTD=0x00;  

 OCR1B=0;  

 delay\_ms(200);  

 PORTA=0x00;  

 delay\_ms(200);


 x=11;  

 y=11;  

 z=11;  

 p=11;  

 l=0;  

 }


 }  

 }


 };

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code
- CodeVisionAVR-Standard
