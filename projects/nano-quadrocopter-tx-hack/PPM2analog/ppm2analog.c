// -------------------------------------------- PPM to pseudo-analog PPM converter, 6 channels to begin with
// -------------------------------------------- Converted from ppm2mcx

#include "common.h"
#include "in_ppm.h"

#define F_CPU 8000000UL
#include <util/delay.h>


#define max(a,b) (a>b?a:b)
#define min(a,b) (a>b?b:a)

// kanal 0-5 ausm ppm - hier kann man ggf. das Channel Mapping einstellen. Vorläufig 1:1
#define CH0 0
#define CH1 1
#define CH2 2
#define CH3 3
#define CH4 4								
#define CH5 5								

// Definitionen des PPM-Protokolls
#define NEUTRALIMPULS (1500)				// Neutralimpulslänge anpassbar
#define MINLEN (6)							// minimaler pwm duty cycle (8 bit)
#define MAXLEN (250)						// maximaler pwm duty cycle (8 bit)



int main()
{
 	int ch0,ch1,ch2,ch3;  
    int pwm=0;
   

	inDecoderInit();
	// enable interrupts...
	
	sei();

	DDRD=0xFF;   // configure as outut
	
	

	while(1)
	{

		
	
		
		ch0=(channelData[CH0]-NEUTRALIMPULS+512)>>2;   // shift raw millisecond result by neutral impulse length and move to 10 bit centre, 
		ch0=max(MINLEN,ch0);							// then scale down to 8 bit by shifting right
		ch0=min(MAXLEN,ch0);
		if(ch0>pwm) cbi(PORTD,CH0); else sbi(PORTD,CH0);
		ch1=(channelData[CH1]-NEUTRALIMPULS+512)>>2;
		ch1=max(MINLEN,ch1);
		ch1=min(MAXLEN,ch1);
		if(ch1>pwm) cbi(PORTD,CH1); else sbi(PORTD,CH1);		
		ch2=(channelData[CH2]-NEUTRALIMPULS+512)>>2;
		ch2=max(MINLEN,ch2);
		ch2=min(MAXLEN,ch2);
		if(ch2>pwm) cbi(PORTD,CH2); else sbi(PORTD,CH2);
		ch3=(channelData[CH3]-NEUTRALIMPULS+512)>>2;
		ch3=max(MINLEN,ch3);
		ch3=min(MAXLEN,ch3);
		if(ch3>pwm) cbi(PORTD,CH3); else sbi(PORTD,CH3);
		
		if(pwm<254) pwm++; else pwm=0;
		

		
	


		



	
	}
	  	

	return 0;
}
