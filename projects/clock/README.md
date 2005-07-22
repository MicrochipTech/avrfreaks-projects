# Clock

Uploaded by admin on 2005-07-21 20:18:00 (rating 0 out of 5)

## Summary

/*********************************************  

This program was produced by the  

CodeWizardAVR V1.0.2.1b Evaluation  

Automatic Program Generator  

© Copyright 1998-2001  

Pavel Haiduc, HP InfoTech S.R.L.  

<http://infotech.ir.ro>  

e-mail:[dhptechn@ir.ro](mailto:dhptechn@ir.ro) , [hpinfotech@xnet.ro](mailto:hpinfotech@xnet.ro)


Project : Clock  

Version :  

Date : 7/20/2005  

Author : Freeware  

Company :  

Comments: 


Chip type : AT90S8515  

Clock frequency : 8.000000 MHz  

Memory model : Small  

Internal SRAM size : 512  

External SRAM size : 0  

Data Stack size : 128  

*********************************************/


#include <90s8515.h>  

#include   

// Declare your global variables here  

 /* void delay() {  

 unsigned char ch1,ch2;  

 for(ch1=0;ch1=255;ch1++)  

 for(ch2=0;ch2=255;ch2++);  

 } */  

void main(void)  

{  

// Declare your local variables here  

 unsigned char sec=0,min=0,hour=12,d;  

// Input/Output Ports initialization  

// Port A  

PORTA=0x00;  

DDRA=0xFF;


// Port B  

PORTB=0x00;  

DDRB=0xFF;


// Port C  

PORTC=0x00;  

DDRC=0xFF;


// Port D  

PORTD=0x00;  

DDRD=0xFF;


// Timer/Counter 0 initialization  

// Clock source: System Clock  

// Clock value: Timer 0 Stopped  

// Mode: Output Compare  

// OC0 output: Disconnected  

TCCR0=0x05;  

TCNT0=0xFF;


// Timer/Counter 1 initialization  

// Clock source: System Clock  

// Clock value: Timer 1 Stopped  

// Mode: Output Compare  

// OC1A output: Discon.  

// OC1B output: Discon.  

// Noise Canceler: Off  

// Input Capture on Falling Edge  

TCCR1A=0x00;  

TCCR1B=0x00;  

TCNT1H=0x1E;  

TCNT1L=0x84;  

OCR1AH=0x00;  

OCR1AL=0x00;  

OCR1BH=0x00;  

OCR1BL=0x00;


// External Interrupt(s) initialization  

// INT0: Off  

// INT1: Off  

GIMSK=0x00;  

MCUCR=0x00;


// Timer(s)/Counter(s) Interrupt(s) initialization  

TIMSK=0x00;


// Analog Comparator initialization  

// Analog Comparator: Off  

// Analog Comparator Input Capture by Timer/Counter 1: Off  

ACSR=0x80;


while (1)  

 {  

 // Place your code here


 again:  

 delay\_us(1000);


 if(sec>=59)  

 { min++; sec=0; }  

 else  

 sec++;  

 if(min>=59)  

 { hour++; min=0;  

 }  

 if(hour>=23) hour=0;  

 PORTA=sec;  

 PORTB=min;  

 PORTC=hour;  

 goto again;  

 };  

}

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code
- CodeVisionAVR-Standard
