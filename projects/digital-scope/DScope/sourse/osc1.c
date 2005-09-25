/*********************************************
Chip type           : ATmega8
Program type        : Application
Clock frequency     : 16.000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 256
*********************************************/

#include <mega8.h>
unsigned int Dly;
#include "stdio.h"
#include "OscFunc8.c"

#define MaxSample 255  
#define ShowSample 128  


#define ADC_VREF_TYPE 0x60
// Read the 8 most significant bits
// of the AD conversion result
unsigned char read_adc(unsigned char adc_input)
{
ADMUX=adc_input|ADC_VREF_TYPE;
// Start the AD conversion
ADCSRA|=0x40;
// Wait for the AD conversion to complete
while ((ADCSRA & 0x10)==0);
ADCSRA|=0x10;
return ADCH;
} 

                                            
unsigned char DC,Samples[256];

///////////////////////////////////////////
void init_IC(void);
void Sampling(void);

void main(void)
{              
  unsigned char i,temp;

  init_IC();
  initLCD();
  Demo();
  grid_mode=0x01; 
  Cmd(0x9F);
  while (1)
  {     
    Sampling();  
    lineto(0,63);
    Grid();
    
    for(i=0;i<ShowSample;i++)
    {                
      temp=127-(Samples[i]>>1);
      putpixel(i,temp);
      //lineto(i,temp);
    }         
    
    readkey();
    delay_ms(1);
  
  }; 
   
}              
//////////////////////////////////
void Sampling()
{                
  unsigned char i,index;
  unsigned int sum;
// sampling

  for(i=0;i<MaxSample;i++)
  {
    Samples[i]=read_adc(0);
    delay();
  }        

  
// Sub DC
if(DC_AC==1)
{
// cal DC
  sum=0;
  for(i=0;i<MaxSample;i++)
    sum+=Samples[i];
  DC=sum>>8;  


  for(i=0;i<MaxSample;i++)
    Samples[i]=128+Samples[i]-DC;
}    
 
        
// Trigger
  i=0;
  while((Samples[i]>=TrigLevel) && i<ShowSample) i++;
  while((Samples[i]<TrigLevel) && i< ShowSample) i++;
  index=i; 
  for(i=0;i<ShowSample;i++)
    Samples[i]=Samples[i+index];

}
///////////////////////////////////////////////
void init_IC(void)
{
// Input/Output Ports initialization
// Port B initialization
// Func0=In Func1=In Func2=In Func3=In Func4=In Func5=In Func6=In Func7=In 
// State0=T State1=T State2=T State3=T State4=T State5=T State6=T State7=T 
PORTB=0x00;
DDRB=0xFF;

// Port C initialization
// Func0=In Func1=In Func2=In Func3=In Func4=In Func5=In Func6=In 
// State0=T State1=T State2=T State3=T State4=T State5=T State6=T 
PORTC=0x38;
DDRC=0x00;

// Port D initialization
// Func0=In Func1=In Func2=In Func3=In Func4=In Func5=In Func6=In Func7=In 
// State0=T State1=T State2=T State3=T State4=T State5=T State6=T State7=T 
PORTD=0x00;
DDRD=0xFF;

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
GICR|=0x00;
MCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x00;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
// Analog Comparator Output: Off
ACSR=0x80;
SFIOR=0x00;

// ADC initialization
// ADC Clock frequency: 125.000 kHz
// ADC Voltage Reference: AVCC pin
// ADC High Speed Mode: On
// ADC Auto Trigger Source: None
// Only the 8 most significant bits of
// the AD conversion result are used
ADMUX=ADC_VREF_TYPE;
ADCSRA=0x87;
SFIOR&=0xEF;
SFIOR|=0x10;

// Global enable interrupts
#asm("cli")
}
