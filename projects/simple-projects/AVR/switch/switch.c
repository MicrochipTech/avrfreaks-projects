/*****************************************************
This program was produced by the
CodeWizardAVR V1.24.8d Professional
Automatic Program Generator
� Copyright 1998-2006 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 02/01/2008
Author  : F4CG                            
Company : F4CG                            
Comments: 


Chip type           : AT90S2313
Clock frequency     : 8.000000 MHz
Memory model        : Tiny
External SRAM size  : 0
Data Stack size     : 32
*****************************************************/

#include <90s2313.h>
#include <delay.h>

// Declare your global variables here
bit HolldSwitchStatus;
void main(void)
{
// Declare your local variables here 
char NumbersCode[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x27,0x7f,0x6f};
char Counter=0;


// Input/Output Ports initialization
// Port B initialization
// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTB=0x00;
DDRB=0xFF;

// Port D initialization
// Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=Out 
// State6=T State5=T State4=T State3=T State2=T State1=T State0=1 
PORTD=0x01;
DDRD=0x01;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
TCCR0=0x00;
TCNT0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer 1 Stopped
// Mode: Normal top=FFFFh
// OC1 output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x00;
TCNT1H=0x00;
TCNT1L=0x00;
OCR1H=0x00;
OCR1L=0x00;

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
PORTB=~NumbersCode[Counter];
while (1)
      {
      // Place your code here
        HolldSwitchStatus = PIND.0;
        delay_ms(50);
        if(HolldSwitchStatus != PIND.0)
                {
                HolldSwitchStatus = PIND.0;
                delay_ms(100);
                if(PIND.0 == 0)
                        {
                        Counter++;
                        if(Counter>9)
                                Counter=0;
                        PORTB=~NumbersCode[Counter];
                        }
                }
      };
}
