#include <ioavr.h>
#include <inavr.h>

char led = 0;

#pragma vector = INT0_vect
__interrupt void ext_int0_isr(void)
{
// Place your code here

if (led == 0)
    led = 1;
else
    led = 0;

}

// Timer 0 overflow interrupt service routine
#pragma vector = TIM0_OVF_vect
__interrupt void timer0_ovf_isr(void)
{
// Place your code here
if (led == 0)
    led = 1;
else
    led = 0;

}



int main( void )
{
  
  
  // Main Clock source: Calibrated Internal 8 MHz Osc.
// Critical section that can't be optimized.
CCP=0xD8;
CLKMSR=0x00;
// Clock Prescaler division factor: 16
CCP=0xD8;
CLKPSR=0x04;



// Voltage Level Monitor
// Trigger Level: Voltage Level Monitor Disabled
// Interrupt: Off
VLMCSR=0x00;

// Input/Output Ports initialization
// Port B initialization
// Pull-up initialization
PUEB=0x00;
// Func3=In Func2=In Func1=Out Func0=Out 
// State3=T State2=T State1=0 State0=0 
PORTB=0x00;
DDRB=0x03;

// Break Before Make Mode PORTB: Off
PORTCR=0x00;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 125.000 kHz
// Mode: Normal top=FFFFh
// Input Capture on Falling Edge
// Input Capture Noise Canceler: Off
// OC0A output: Disconnected
// OC0B output: Disconnected
TCCR0A=0x00;
TCCR0B=0x03;
TCNT0=0x0000;
ICR0=0x0000;
OCR0A=0x0000;
OCR0B=0x0000; 

// External Interrupt(s) initialization
// INT0: On
// INT0 Mode: Falling Edge
// Interrupt on any change on pins PCINT0-3: Off
EICRA=0x02;
EIMSK=0x01;
EIFR=0x01;
PCICR=0x00;

// Timer/Counter 0 Interrupt(s) initialization
TIMSK0=0x01;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 0: Off
ACSR=0x80;

// Global enable interrupts

__enable_interrupt();

while(1)
{
  
  if (led == 0)
    PORTB = 0xFE;
  
  else
    PORTB = 0xFD;
}
  

}
