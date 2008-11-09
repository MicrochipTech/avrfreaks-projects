/*****************************************************
This program was produced by the
CodeWizardAVR V1.24.6 Standard
Automatic Program Generator
© Copyright 1998-2005 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com
e-mail:office@hpinfotech.com

Project : 
Version : 
Date    : 2008.10.26.
Author  : STEFFY                          
Company : HOME                            
Comments: 


Chip type           : ATtiny13
Clock frequency     : 4,800000 MHz
Memory model        : Tiny
External SRAM size  : 0
Data Stack size     : 16
*****************************************************/

#include <tiny13.h>  
#include <delay.h>
#include <sleep.h>

// Declare your global variables here
int fel_le,a;
void main(void)
{
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization
// Port B initialization
// Func5=In Func4=In Func3=In Func2=In Func1=Out Func0=Out 
// State5=T State4=T State3=T State2=T State1=1 State0=0 
PORTB=0x02;
DDRB=0x03;
 
// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 600,000 kHz
// Mode: Fast PWM top=FFh
// OC0A output: Inverted PWM
// OC0B output: Disconnected
TCCR0A=0xC3;
TCCR0B=0x02;
TCNT0=0x00;
OCR0A=0x00;
OCR0B=0x00;

// External Interrupt(s) initialization
// INT0: Off
// Interrupt on any change on pins PCINT0-5: Off
GIMSK=0x00;
MCUCR=0x00;

// Timer/Counter 0 Interrupt(s) initialization
TIMSK0=0x00;

// Analog Comparator initialization
// Analog Comparator: Off
ACSR=0x80;
ADCSRB=0x00;

fel_le = 1; //tehát fel
a=930;
sleep_enable();
while (1)
      {
      // Place your code here  
       if(fel_le) { 
          if(OCR0A<255) OCR0A++; //addig növelem amíg nem max 
            else fel_le = !fel_le; //ha max, akkor lefelé 
                } 
       if(!fel_le) { 
          if(OCR0A) OCR0A--; //addig csökkentem amíg >0 
            else fel_le = !fel_le; //ha min, akkor felfelé 
            }                                              
      delay_ms(1);
      if (a==0)
          {TCCR0A=0x00;
           TCCR0B=0x00;
           PORTB.0=1;
           idle();
          }
      a--;      
      };
}
