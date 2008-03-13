
#include <avr/interrupt.h>
#include <avr/io.h>

#define FCPU 8000000
#define T_INTERRUPT 4000
/* adc MUX input ch
00000 ADC0
00001 ADC1
00010 ADC2*/

#define CH_X 0
#define CH_Y 1
#define CH_Z 2

unsigned stepwaitadc[3]={0,0,0};

//Steppermotor output ports
#define stepperX PORTB
#define stepperY PORTD
#define stepperZ PORTC

char steppattern[]={0x01,0x03,0x02,0x06,0x04,0x0c,0x08,0x09};

int Index[]={0,0,0};

// I/O
void init(void){

(*(&stepperX-1))=0xFF;//DDRx out
stepperX=0x00; //PORTx 0

(*(&stepperY-1))=0xFF;
stepperY=0x00;

(*(&stepperZ-1))=0xFF;
stepperZ=0x00;
}//end init

unsigned char test=0;
//************ init Timer 1 *************

void init_timer1(void){
	TCCR1A=0;
	TCCR1B=(1<<CS10); //ingen prescaling
	TCNT1=0;//teller fra null første gang
	OCR1A=T_INTERRUPT; //->avbrudd hvert 1 ms ved 8MHz
	TIMSK=(1<<OCIE1A);//interrupt enable
	sei();//global interrupt enable	
}


// ADLAR ADC Left Adjust Result
// ADATE ADC Auto Trigger Enable -> Trigger source ADTS in SIFOR
// ADSC ADC Start Convertion
// ADEN ADC enable
// ADIF ADC Interrupt Flag (bit 4 , 0x10 in ADCSRA)
// ADIE ADC Interrupt Enable
// ADPS2:0: ADC Prescaler Select Bits.(only ADPS0 -> division factor 2)



unsigned int read_adc(unsigned char adc_input_ch){
  ADMUX=adc_input_ch |(1<<REFS0) | (1<<ADLAR); //Set channel for input, REFS0=AVCC with external capacitor at AREF pin
  ADCSRA=( (1<<ADEN) | (1<<ADSC)  | (1<<ADPS1) | (1<<ADPS2) );//start conversion
  while((ADCSRA & (1<<ADSC))); // ADSC will read as one as long as a conversion is in progress
  return ADCH;
}


/******************** Interrupt routine ***********************/

SIGNAL(SIG_OUTPUT_COMPARE1A){
	OCR1A+=T_INTERRUPT;
	
		
 	//motor x
	if(stepwaitadc[CH_X]!=0)stepwaitadc[CH_X]--;
		if(stepwaitadc[CH_X]==0){
			stepwaitadc[CH_X]=read_adc(CH_X);
			if(stepwaitadc[CH_X]>=153){
		    Index[CH_X]++;
			Index[CH_X]&=0x07;
			stepwaitadc[CH_X]=(256-stepwaitadc[CH_X]);
			stepperX=steppattern[Index[CH_X]];
			}
			else if(stepwaitadc[CH_X]<=103){
			Index[CH_X]--;
			Index[CH_X]&=0x07;
			stepperX=steppattern[Index[CH_X]];
			}
			else stepperX=0;
				
		}
	//motor y
	if(stepwaitadc[CH_Y]!=0)stepwaitadc[CH_Y]--;
		if(stepwaitadc[CH_Y]==0){
			stepwaitadc[CH_Y]=read_adc(CH_Y);
			if(stepwaitadc[CH_Y]>=153){
		    Index[CH_Y]++;
			Index[CH_Y]&=0x07;
			stepwaitadc[CH_Y]=(256-stepwaitadc[CH_Y]);
			stepperY=steppattern[Index[CH_Y]];
			}
			else if(stepwaitadc[CH_Y]<=103){
			Index[CH_Y]--;
			Index[CH_Y]&=0x07;
			stepperY=steppattern[Index[CH_Y]];
			}
			else stepperY=0;	
		}

	//motor z 
	if(stepwaitadc[CH_Z]!=0)stepwaitadc[CH_Z]--;
		if(stepwaitadc[CH_Z]==0){	
		stepwaitadc[CH_Z]=read_adc(CH_Z);
		if(stepwaitadc[CH_Z]>=153){
		Index[CH_Z]++;
		Index[CH_Z]&=0x07;
		stepwaitadc[CH_Z]=(256-stepwaitadc[CH_Z]);
		stepperZ=(steppattern[Index[CH_Z]]<<1);// left shifted because I soldered the transistors wrong
		}
		else if(stepwaitadc[CH_Z]<=103){
		Index[CH_Z]--;
		Index[CH_Z]&=0x07;
		stepperZ=(steppattern[Index[CH_Z]]<<1);	
		}
		else stepperZ=0;		
	  }
	

}//end signal
/*************** WAIT timer 0***************/
void waitN10us(unsigned char count)
{
    // grab the timer
    TCCR0 = (1<<CS00);
    if (count > 2) count -= 2;
    while(count) 
    {
        //TCNT0= 256 - ((FCPU/800000)-1);
        TCNT0=256-50;
		TIFR = (1<<TOV0);
        while(!(TIFR & (1<<TOV0)));
        count--;
    }
    // free timer
    TCCR0 = 0;
}

void waitNms(unsigned int count)
{
  while(count) 
  {
    waitN10us(100);
    count--;
  }
}



//******************************


int main(void){

	init();
	init_timer1();
	
	while(1){
	
		asm("nop");

	}//endwhile
return 0;
}//end main

/* Note to self:

Starting a Conversion 
A single conversion is started by writing a logical one to the ADC Start Conversion bit,
ADSC. This bit stays high as long as the conversion is in progress and will be cleared by
hardware when the conversion is completed. If a different data channel is selected while
a conversion is in progress, the ADC will finish the current conversion before performing
the channel change.
Alternatively, a conversion can be triggered automatically by various sources. Auto Triggering
is enabled by setting the ADC Auto Trigger Enable bit, ADATE in ADCSRA. The
trigger source is selected by setting the ADC Trigger Select bits, ADTS in SFIOR (see
description of the ADTS bits for a list of the trigger sources). When a positive edge
occurs on the selected trigger signal, the ADC prescaler is reset and a conversion is
started. This provides a method of starting conversions at fixed intervals. If the trigger
signal still is set when the conversion completes, a new conversion will not be started. If
another positive edge occurs on the trigger signal during conversion, the edge will be
ignored. Note that an Interrupt Flag will be set even if the specific interrupt is disabled or
the global interrupt enable bit in SREG is cleared. A conversion can thus be triggered
without causing an interrupt. However, the Interrupt Flag must be cleared in order to trigger
a new conversion at the next interrupt event.



*/

