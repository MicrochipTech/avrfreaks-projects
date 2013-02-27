# wall follower robot

Uploaded by electronicsbaazar on 2013-02-27 04:25:00 (rating 0 out of 5)

## Summary

electronics materials - dc motor 2no, power supply, l293d motordriver, ir proximate sensor, wheel 4no, chassis,atmega16 


#include   

#include   

#include


void adc\_init()  

{  

ADCSRA=0XE5;  

}  

unsigned char read(unsigned char y)  

{  

unsigned char c;  

ADMUX=0X60|y;  

\_delay\_ms(10);  

c=ADCH;  

return c;  

}


void main()  

{


 DDRA=0x00;  

 DDRB=0XFF;


 adc\_init();  

 \_delay\_ms(25);  

 unsigned char a,b,c;  

 while(1)  

 {  

 a=read(0);  

 b=read(1);  

 c=read(2);


 if(a<150 && b>150 && c>150 )  

 {  

 PORTB=0b00001010;  

 }  

 else if(a<150 && b<150 && c>150 )  

 {  

 PORTB=0b000001001;  

 }  

 else if(a>150 && b>150 && c<150 )  

 {  

 PORTB=0b00001010;  

 }  

 else if(a>150 && b<150 && c<150 )  

 {  

 PORTB=0b000000110;  

 }


 else  

 {  

 PORTB=255;  

 }


}  

}


<http://electronicsbaazar.com/>

## Compilers

- AVR Studio 5

## Tags

- Complete code
- AVR Studio 5
