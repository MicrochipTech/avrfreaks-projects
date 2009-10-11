#include <avr\interrupt.h>
#include "Init.h"

#define CH1Bit 4    // bit 2
#define CH2Bit 8	//bit 3
#define CH3Bit	16	//bit 4
#define CHECKBIT(x,y) (x & (y)) /* Check bit y in byte x*/

ISR(ADC_vect)
 {
 
extern uint16_t CH0,CH1,CH2,CH3,CH4,CH5;
extern uint8_t channel;


switch (channel)
		{

	case 0:

   CH0=(ADCL+(ADCH*256));
   channel=1;
	
	break;


	case 1:
	CH1=(ADCL+(ADCH*256));
	channel=2;
	break;

	case 2:
	CH2=(ADCL+(ADCH*256));
	channel=3;
	break;
	
	case 3:
	CH3=(ADCL+(ADCH*256));
	channel=4;
	break;
	
	case 4:
	CH4=(ADCL+(ADCH*256));
	channel=5;
	break;

	default:
	CH5=(ADCL+(ADCH*256));
	channel=0;


	
	}
	

	ADMUX=channel;
	ADMUX|=(1<<REFS0);
	ADCSRA|=(1<<ADSC);
	



}
 
