/*****************************************************
This program was produced by the
CodeWizardAVR V1.24.4 Standard
Automatic Program Generator
© Copyright 1998-2004 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com
e-mail:office@hpinfotech.com

Project : 
Version : 
Date    : 10/12/2006
Author  : Salman Shariati                 
Company : Home                            
Comments: 


Chip type           : ATmega8
Program type        : Application
Clock frequency     : 4.000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 256
*****************************************************/

#include <mega8.h>
#include <delay.h> 
#include <stdio.h>
#include <stdlib.h>

// Alphanumeric LCD Module functions
#asm
   .equ __lcd_port=0x18 ;PORTB
#endasm
#include <lcd.h>

// Declare your global variables here

void main(void)
{
// Declare your local variables here

float deg = 2.7;
char str1[10],str2[28];

// Input/Output Ports initialization
// Port B initialization
// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTB=0x00;
DDRB=0xFF;

// Port C initialization
// Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTC=0x00;
DDRC=0x00;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=T State6=T State5=T State4=T State3=0 State2=0 State1=0 State0=0 
PORTD=0x00;
DDRD=0x0F;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
TCCR0=0x00;
TCNT0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer 1 Stopped
// Mode: Normal top=FFFFh
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
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
MCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x00;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// LCD module initialization
lcd_init(16);

while (1)
      {
        lcd_gotoxy(0,0);
        lcd_putsf("Position : ");
                
        if (deg <= 360)
          {
            PORTD = 0b00000110;
            delay_ms(10);
            deg = deg + 1.8;
            ftoa(deg,1,str1);
            sprintf(str2,"%5s Degree  ",str1); 
            lcd_gotoxy(0,1);
            lcd_puts(str2); 
            PORTD = 0b00000101;
            delay_ms(10);
            deg = deg + 1.8;
            ftoa(deg,1,str1);
            sprintf(str2,"%5s Degree  ",str1); 
            lcd_gotoxy(0,1); 
            lcd_puts(str2);
            PORTD = 0b00001001;
            delay_ms(10);  
            deg = deg + 1.8;
            if (deg <= 360)
              {
              ftoa(deg,1,str1);
              sprintf(str2,"%5s Degree  ",str1); 
              lcd_gotoxy(0,1);
              lcd_puts(str2);
              PORTD = 0b00001010;
              delay_ms(10);
              deg = deg + 1.8;
              }
            else 
              {
              deg = 0.9;
              ftoa(deg,1,str1);
              sprintf(str2,"%5s Degree  ",str1); 
              lcd_gotoxy(0,1);
              lcd_puts(str2);
              PORTD = 0b00001010;
              delay_ms(10);
              deg = deg + 1.8;
              }  
          ftoa(deg,1,str1);
          sprintf(str2,"%5s Degree  ",str1); 
          lcd_gotoxy(0,1);
          lcd_puts(str2);
          }
      }
}
