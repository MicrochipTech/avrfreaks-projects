/*****************************************************
This program was produced by the
CodeWizardAVR V2.04.3a Standard
Automatic Program Generator
© Copyright 1998-2009 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : Tiny5 Level Meter
Version : 0.001
Date    : 8/31/2009
Author  : Brian Hammill
Company : Atmel Corporation
Comments: 
Simple project for the ADC on the Tiny5


Chip type               : ATtiny5
AVR Core Clock frequency: 12.000000 MHz
Memory model            : Tiny
External RAM size       : 0
Data Stack size         : 8
*****************************************************/

#include <tiny5.h>

#include <delay.h>

#define THRESHOLD_LOW   102     // (Low is 0 to 2 volts
#define THRESHOLD_HIGH  153     // (High is 3 to 5 volts
// Between 2 and 3 is MID.  Assume 5 volt Vcc for reference voltage.

// Define 3 Active Low LED
#define LED_LOW     PORTB1
#define LED_MID     PORTB2
#define LED_HIGH    PORTB3

#define TRUE    1
#define FALSE   0



void update_leds(void);     // Function Prototype

unsigned char new_data;     // Flag to tell us if the ADC has updated.


// Timer 0 output compare A interrupt service routine
interrupt [TIM0_COMPA] void timer0_compa_isr(void)
{
// Place your code here
update_leds();
new_data = FALSE;

}

#define FIRST_ADC_INPUT 0
//#define LAST_ADC_INPUT 0
unsigned char adc_data;


// ADC interrupt service routine
// with auto input scanning
interrupt [ADC_INT] void adc_isr(void)
{
// static unsigned char input_index=0;
// Read the A/D conversion result
adc_data=ADCL;
new_data = TRUE;    // Access control to be sure ADC data is written.

// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the A/D conversion
ADCSRA|=0x40;
}

void update_leds()
{
    // We have 3 LEDs on PB1, PB2, and PB3.
    // ADC0 is on PB0.
    // There are no more pins...
    // Based on the ADC reading, we want to light up 1 of 3 LEDs 
    
    if (adc_data <= THRESHOLD_LOW )
    {   // Want to light up LED on PB1 (LOW) 
        PORTB = ~(1<<LED_LOW);
        }
        
    else if ( adc_data >= THRESHOLD_HIGH )
    {
        // Want to light up LED on PB3 (HIGH)
        PORTB = ~(1<<LED_HIGH);
        }
        
    else // Voltage on ADC is between the two thresholds.
    {
        // Want to light up LED on PB2 (MID)  
        PORTB = ~(1<<LED_MID);
        }
        
        
}
        
// Declare your global variables here

void main(void)
{
// Declare your local variables here

// Clock Prescaler division factor: 1
#pragma optsize-
CCP=0xD8;
CLKPSR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Voltage Level Monitor
// Trigger Level: 2.5V -> Power-On Reset
// Interrupt: Off
VLMCSR=0x02;

// Input/Output Ports initialization
// Port B initialization
PUEB=0x00;
// Break Before Make Mode: Off
PORTCR=0x00;
// Func3=Out Func2=Out Func1=Out Func0=In 
// State3=1 State2=1 State1=1 State0=T 
PORTB=0x0E;
DDRB=0x0E;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 1500.000 kHz
// Mode: Normal top=FFFFh
// Input Capture on Falling Edge
// Input Capture Noise Canceler: Off
// OC0A output: Disconnected
// OC0B output: Disconnected
TCCR0A=0x00;
TCCR0B=0x02;
TCNT0=0x0000;
ICR0=0x0000;
OCR0A=0x0400;   // Was 0x1500.
OCR0B=0x0000;

// External Interrupt(s) initialization
// INT0: Off
// Interrupt on any change on pins PCINT0-3: Off
EICRA=0x00;
EIMSK=0x00;
PCICR=0x00;

// Timer/Counter 0 Interrupt(s) initialization
TIMSK0=0x02;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 0: Off
ACSR=0x80;

// ADC initialization
// ADC Clock frequency: 93.750 kHz
// ADC Auto Trigger Source: Free Running
// Digital input buffers on ADC0: Off, ADC1: On, ADC2: On, ADC3: On
DIDR0=0x01;
ADMUX=FIRST_ADC_INPUT;
ADCSRA=0xEF;
ADCSRB=0x00;

// Global enable interrupts
#asm("sei")

while (1)
      {
      // Place your code here

      };
}
