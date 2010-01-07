# ping parallax

Uploaded by putraade on 2010-01-07 11:20:00 (rating 1 out of 5)

## Summary

/*****************************************************  

This program was produced by the  

CodeWizardAVR V1.25.3 Standard  

Automatic Program Generator  

Â© Copyright 1998-2007 Pavel Haiduc, HP InfoTech s.r.l.  

<http://www.hpinfotech.com>


Project :  

Version :  

Date : 1/1/2010  

Author : babythecrow  

Company : babythecrow  

Comments:


Chip type : ATmega16  

Program type : Application  

Clock frequency : 12.000000 MHz  

Memory model : Small  

External SRAM size : 0  

Data Stack size : 256  

*****************************************************/


#include   

#include   

#include 


// Alphanumeric LCD Module functions  

#asm  

.equ \_\_lcd\_port=0x15 ;PORTC  

#endasm  

#include   

#define PulseOut PORTD.0  

#define PulseIn PIND.0  

#define DirPulse DDRD.0  

#define pulseki PORTA.0  

unsigned int counter;  

float jarak;


unsigned char Text1[16];  

unsigned char Text2[16];


// Declare your global variables here


void main(void)  

{  

// Declare your local variables here


// Input/Output Ports initialization  

// Port A initialization  

// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In  

// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T  

PORTA=0x00;  

DDRA=0x00;


// Port B initialization  

// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In  

// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T  

PORTB=0x00;  

DDRB=0x00;


// Port C initialization  

// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In  

// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T  

PORTC=0x00;  

DDRC=0x00;


// Port D initialization  

// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In  

// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T  

PORTD=0x00;  

DDRD=0x00;


// Timer/Counter 0 initialization  

// Clock source: System Clock  

// Clock value: Timer 0 Stopped  

// Mode: Normal top=FFh  

// OC0 output: Disconnected  

TCCR0=0x00;  

TCNT0=0x00;  

OCR0=0x00;


// Timer/Counter 1 initialization  

// Clock source: System Clock  

// Clock value: Timer 1 Stopped  

// Mode: Normal top=FFFFh  

// OC1A output: Discon.  

// OC1B output: Discon.  

// Noise Canceler: Off  

// Input Capture on Falling Edge  

// Timer 1 Overflow Interrupt: Off  

// Input Capture Interrupt: Off  

// Compare A Match Interrupt: Off  

// Compare B Match Interrupt: Off  

TCCR1A=0x00;  

TCCR1B=0x00;  

TCNT1H=0x00;  

TCNT1L=0x00;  

ICR1H=0x00;  

ICR1L=0x00;  

OCR1AH=0x00;  

OCR1AL=0x00;  

OCR1BH=0x00;  

OCR1BL=0x00;


// Timer/Counter 2 initialization  

// Clock source: System Clock  

// Clock value: Timer 2 Stopped  

// Mode: Normal top=FFh  

// OC2 output: Disconnected  

ASSR=0x00;  

TCCR2=0x00;  

TCNT2=0x00;  

OCR2=0x00;


// External Interrupt(s) initialization  

// INT0: Off  

// INT1: Off  

// INT2: Off  

MCUCR=0x00;  

MCUCSR=0x00;


// Timer(s)/Counter(s) Interrupt(s) initialization  

TIMSK=0x00;


// Analog Comparator initialization  

// Analog Comparator: Off  

// Analog Comparator Input Capture by Timer/Counter 1: Off  

ACSR=0x80;  

SFIOR=0x00;


// LCD module initialization  

lcd\_init(16);


while (1)  

{  

// Place your code here  

counter=0; //inisialisasi conter  

DirPulse=1; //set as output  

PulseOut=1; //kirim sinyal  

delay\_us(5); //lama data  

PulseOut=0;  

DirPulse=0;  

PulseOut=1;  

while (PulseIn==0)  

{}  

while (PulseIn==1)


{counter++;}


jarak=(counter*0.034442)/2;


if (counter>1000)  

{pulseki=1;}  

else if (counter<1000)  

{pulseki=0;}


lcd\_gotoxy(0,0);  

sprintf(Text1,"Counter=%5d uS",counter);  

lcd\_puts(Text1);  

lcd\_gotoxy(0,1);  

sprintf(Text2,"S=%3d.2f cm",jarak);  

lcd\_puts(Text2);  

delay\_ms(200);  

lcd\_clear();  

};  

}

## Compilers

- CodeVisionAVR-Standard

## Tags

- General Lib.functions
- CodeVisionAVR-Standard
