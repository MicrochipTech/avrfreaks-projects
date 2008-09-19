/*****************************************************
This program was produced by the
CodeWizardAVR V1.24.4 Standard
Automatic Program Generator
© Copyright 1998-2004 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com
e-mail:office@hpinfotech.com

Project : 
Version : 
Date    : 9/16/2008
Author  : Osama                           
Company : OSAMA                           
Comments: 


Chip type           : ATmega8L
Program type        : Application
Clock frequency     : 8.000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 256
*****************************************************/

#include <mega8.h>
#include <delay.h>
#include <stdio.h>

int r;

// Timer 1 input capture interrupt service routine
interrupt [TIM1_CAPT] void timer1_capt_isr(void)
{                
          TCCR1B &= 0xF8; //stop Timer/Counter1 
          TIMSK &= 0xDF;  //disable Input Capture Interrupt
          DDRB |= 0x01;   //set PORTB.0 as output
          PORTB &= 0xFE;  //clear PORTB.0
          delay_ms(500);  //discharge the capacitor
          DDRB &= 0xFE;   //set PORTB.0 as input
          PORTB &= 0xFE;  //Tri-state
          TIMSK |= 0x20;  //enable Input Capture Interrupt       
          r = TCNT1/1000;
          printf("\b\b\b\b\b\b\b\b\bLevel: %d",r);
          TCNT1H = 0x00;  //clear Timer1 MSBs       
          TCNT1L = 0x00;  //clear Timer1 LSBs                 
          TCCR1B |= 0x03; //set prescaler to clk/64 (start counter)
}

// Declare your global variables here

void main(void)
{
// Declare your local variables here

// Input/Output Ports initialization
// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTB=0x00;
DDRB=0x00;

// Port C initialization
// Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
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
TCCR0=0x00;
TCNT0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 125.000 kHz
// Mode: CTC top=OCR1A
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Rising Edge
TCCR1A=0x00;
TCCR1B=0x4B;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0xFF;
OCR1AL=0xFF;
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
TIMSK=0x20;
    
// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: Off
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud rate: 9600
UCSRA=0x00;
UCSRB=0x08;
UCSRC=0x86;
UBRRH=0x00;
UBRRL=0x33;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// Global enable interrupts
#asm("sei")          

          TCCR1B &= 0xF8; //stop Timer/Counter1 
          TIMSK &= 0xDF;  //disable Input Capture Interrupt
          DDRB |= 0x01;   //set PORTB.0 as output
          PORTB &= 0xFE;  //clear PORTB.0
          delay_ms(500); //discharge the capacitor
          DDRB &= 0xFE;   //set PORTB.0 as input 
          PORTB &= 0xFE;  //Tri-state          
          TIMSK |= 0x20;  //enable Input Capture Interrupt 
          TCCR1B |= 0x03; //set prescaler to clk/64 (start counter)  
}
