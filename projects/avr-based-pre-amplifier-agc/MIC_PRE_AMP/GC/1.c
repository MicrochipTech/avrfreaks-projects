/*****************************************************
This program was produced by the
CodeWizardAVR V2.02.6 Professional
Automatic Program Generator
© Copyright 1998-2008 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 2005/09/02
Author  : F4CG                            
Company : F4CG                            
Comments: 


Chip type           : ATtiny26L
Clock frequency     : 8.000000 MHz
Memory model        : Tiny
External RAM size   : 0
Data Stack size     : 32
*****************************************************/

#include <tiny26.h>  
#include <delay.h>



unsigned char   min=0,max=0;

unsigned char   vpp=0;   
unsigned char   gain=7;





#define ADC_VREF_TYPE 0x20

// Read the 8 most significant bits
// of the AD conversion result
unsigned char read_adc(unsigned char adc_input)
{
ADMUX=adc_input | (ADC_VREF_TYPE & 0xff);
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the AD conversion
ADCSR|=0x40;
// Wait for the AD conversion to complete
while ((ADCSR & 0x10)==0);
ADCSR|=0x10;
return ADCH;
}

// Timer 1 overflow interrupt service routine           // 32 ms
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
        unsigned int x;
        unsigned char temp;
        min = 128;
        max = 128;
        for(x=0;x<500;x++)
        {      
                temp = read_adc(9);
                if( temp > max)
                        max = temp;
                if( temp < min)
                        min = temp;
        }
        vpp = max - min ;     
        if(vpp>230)
        {               
                if(gain > 0)
                        gain --;
        }       
        if( vpp < 100)
        {
                if(gain < 7)
                        gain ++;
        }        
        
        PORTA = gain;
}

void main(void)
{
// Declare your local variables here

// Input/Output Ports initialization
// Port A initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=Out Func1=Out Func0=Out 
// State7=P State6=P State5=P State4=P State3=P State2=0 State1=0 State0=0 
PORTA=0xF8;
DDRA=0x07;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=P State6=T State5=P State4=P State3=P State2=P State1=P State0=P 
PORTB=0xBF;
DDRB=0x00;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
TCCR0=0x00;
TCNT0=0x00;
// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 7.813 kHz
// Mode: Normal top=FFh
// OC1A output: Disconnected
// OC1B output: Disconnected
// Timer 1 Overflow Interrupt: On
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
PLLCSR=0x00;
TCCR1A=0x00;
TCCR1B=0x0B;
TCNT1=0x00;
OCR1A=0x00;
OCR1B=0x00;
OCR1C=0x00;

// External Interrupt(s) initialization
// INT0: Off
// Interrupt on any change on pins PA3, PA6, PA7 and PB4-7: Off
// Interrupt on any change on pins PB0-3: Off
GIMSK=0x00;
MCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x04;

// Universal Serial Interface initialization
// Mode: Disabled
// Clock source: Register & Counter=no clk.
// USI Counter Overflow Interrupt: Off
USICR=0x00;

// Analog Comparator initialization
// Analog Comparator: Off
ACSR=0x80;

// ADC initialization
// ADC Clock frequency: 1000.000 kHz
// ADC Voltage Reference: AVCC pin
// Only the 8 most significant bits of
// the AD conversion result are used
ADMUX=ADC_VREF_TYPE & 0xff;
ADCSR=0x83;

// Global enable interrupts
#asm("sei")

while (1)
      {


      };
}
