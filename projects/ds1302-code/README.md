# DS1302 Code

Uploaded by shahrear on 2007-04-05 13:25:00 (rating 0 out of 5)

## Summary

This code will read a Time from ds1302 rtc  

and show it on a HD44780 LCD 24x2 display.


#include   

#include   

#include   

// Alphanumeric LCD Module functions  

#asm  

 .equ \_\_lcd\_port=0x1B ;PORTA  

#endasm  

#include   

/* the DS1302 is connected to PORTB */  

/* the IO signal is bit 3 */  

/* the SCLK signal is bit 4 */  

/* the RST signal is bit 5 */  

#asm  

 .equ \_\_ds1302\_port=0x18  

 .equ \_\_ds1302\_io=3  

 .equ \_\_ds1302\_sclk=4  

 .equ \_\_ds1302\_rst=5  

#endasm


/* now you can include the DS1302 Functions */  

#include 


// Declare your global variables here  

unsigned char h,m,s;  

unsigned char buff[17];  

void main(void)  

{  

int i,key;  

DDRD=0x00;  

// LCD module initialization  

lcd\_init(24);  

rtc\_init(1,1,3);  

rtc\_set\_date(12,12,07);  

rtc\_set\_time(13,12,00);  

ds1302\_write(0x80,0x00);  

lcd\_gotoxy(0,0);  

while (1)  

 {  

 lcd\_gotoxy(0,0);  

 key=PIND;  

 delay\_ms(20);  

 if(key==0)  

 {  

 lcd\_clear();  

 lcd\_putsf("SET TIME");  

 while(1){};  

 }  

 rtc\_get\_time(&h,&m,&s);  

 delay\_ms(20);  

 sprintf(buff, "%02u/%02u/%02u",h,m,s);  

 for (i=0;i<8;i++)  

 {  

 lcd\_putchar(buff[i]);}  

};  

}

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code
- CodeVisionAVR-Standard
