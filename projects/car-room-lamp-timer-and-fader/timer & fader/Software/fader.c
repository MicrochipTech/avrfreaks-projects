          /****************************************************
          ** Processor      : ATtiny13                        **
         *** Frequncy       : 9.600000 MHz                    ***
        **** AUTHOR         : Behnam Esckandariun             ****
       ***** Project        : Car room lamp fader V.1.00.5    *****
        **** Date           : 2007/10/10   07:49 PM           ****
         *** Compiler       : CodeVisionAVR  V 1.25.5         ***
          ** comment        : Just for educational use        **
           ****************************************************/

#include <tiny13.h>
#include <delay.h>
#include <sleep.h>

#define sw PINB.1
#define pwm_out OCR0A                   
#define ADC_VREF_TYPE 0x00              //VCC used as analog reference
                          
void sys_init(void);
unsigned int read_adc(unsigned char adc_input);

///////////////////////// External Interrupt 0 service routine \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
interrupt [EXT_INT0] void ext_int0_isr(void)
{
unsigned char i=0;
#asm("cli")                             //Disable Global interrupts
for (i=0;i<255;i++)                     //Starting fade IN
        {                               
        pwm_out = i;                    
        delay_ms(8);                   
        }; 
pwm_out=0xff;                           //Turn on room lamp completly
while(!sw);                             //Wait for door closed
delay_ms((read_adc(1)/60)*1000);        //Delay ms after door closing adjustable with R2 from 0-17 second
for (i=255;i>=1;i--)                    //Starting fade OUT
        {                               
        pwm_out = i;                    
        delay_ms(16);                   
        };                              
pwm_out=0x00;                           //Turn off room lamp completly
#asm("sei")                             //Enable Global interrupts
}

/////////////////////////// Read the AD conversion result \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
unsigned int read_adc(unsigned char adc_input)
{
ADMUX=adc_input | (ADC_VREF_TYPE & 0xff);
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the AD conversion
ADCSRA|=0x40;
// Wait for the AD conversion to complete
while ((ADCSRA & 0x10)==0);
ADCSRA|=0x10;
return ADCW;
}

//////////////////////////////////// Main Program \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
void main(void)
{
sys_init();
#asm("sei")
while (1)
      {
      powerdown();              //Sleep MCU and wait for interrupt occur
      };
}

//////////////////////////////////// System init \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
void sys_init(void)
{
// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization
// Port B initialization
// Func5=In Func4=In Func3=In Func2=In Func1=In Func0=Out 
// State5=P State4=T State3=T State2=T State1=P State0=0 
PORTB=0x22;
DDRB=0x01;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 9600.000 kHz
// Mode: Fast PWM top=FFh
// OC0A output: Non-Inverted PWM
// OC0B output: Disconnected
TCCR0A=0x83;
TCCR0B=0x01;
TCNT0=0x00;
OCR0A=0x00;
OCR0B=0x00;

// External Interrupt(s) initialization
// INT0: On
// INT0 Mode: Falling Edge
// Interrupt on any change on pins PCINT0-5: Off
GIMSK=0x40;
MCUCR=0x02;
GIFR=0x40;

// Timer/Counter 0 Interrupt(s) initialization
TIMSK0=0x00;

// Analog Comparator initialization
// Analog Comparator: Off
ACSR=0x80;
ADCSRB=0x00;

// ADC initialization
// ADC Clock frequency: 75.000 kHz
// ADC Bandgap Voltage Reference: Off
// ADC Auto Trigger Source: None
// Digital input buffers on ADC0: On, ADC1: On, ADC2: On, ADC3: On
DIDR0&=0x03;
DIDR0|=0x00;
ADMUX=ADC_VREF_TYPE & 0xff;
ADCSRA=0x87;
/*
// Watchdog Timer initialization
// Watchdog Timer Prescaler: OSC/1024k
// Watchdog Timer interrupt: Off
#pragma optsize-
WDTCR=0x39;
WDTCR=0x29;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif
*/
sleep_enable();
}