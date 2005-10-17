#include "main.h"
#include "rc_matsushita.h"


#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <string.h>

#define TOLERANCE 40		//tolerance for sync ,appr 10%
#define ACCEPTANCE_MIN 3000		//3.25ms@7.3728MHz
#define ACCEPTANCE_MAX 3560		//3.75ms

volatile byte rc_new_input;
byte sync;
word prev_icr, icr_ticks,max_time,min_time;

struct {
			word device;
			word command;
		} matsushita;


SIGNAL (SIG_INPUT_CAPTURE1)
{	static byte bit_position;
	static word timeslice;
	static unsigned long l_aux;
	unsigned long ibit;
	word current_icr;

	current_icr=ICR1;
	icr_ticks=current_icr-prev_icr;
	prev_icr=current_icr;
	switch (sync)
	{ 	case 0:	sync++;
					TCCR1B|=0x40;		//trigger on rising egde
					break;
		case 1:	//first sync step: long low pulse of 8T
					TCCR1B&=0xBF;		//trigger on falling egde
					if ((icr_ticks>ACCEPTANCE_MIN)&&(icr_ticks<ACCEPTANCE_MAX))
					{	sync++;
						timeslice=(icr_ticks/8);
						max_time=timeslice+TOLERANCE;
						min_time=timeslice-TOLERANCE;
						bit_position=0;
					}
					else
						sync=0;
					break;
		case 2:	//second step long high pulse of 8T or 4T for repeated command
					if ((icr_ticks>(8*min_time))&&(icr_ticks<(8*max_time)))
					{	sync++;
						l_aux=0;
					}
					else
						sync=0;
					break;
		case 3:	ibit=2;
					if ((icr_ticks>(8*min_time))&&(icr_ticks<(8*max_time)))
						ibit=1;
					else if((icr_ticks>(4*min_time))&&(icr_ticks<(4*max_time)))
						ibit=0;
					if (ibit==2)
					{	sync=0;
					}
						else
					{	l_aux|=(ibit<<(bit_position++));
						if(bit_position>23)
						{	rc_new_input=1;
							matsushita.device=(l_aux&0x0000003F);
							matsushita.device|=(l_aux>>4)&0x00003F00;
							matsushita.command=(l_aux>>6)&0x0000003F;
							matsushita.command|=(l_aux>>10)&0x00003F00;
							sync=0;
						}
					}
					
					
					break;

		default:	break;
	}				
}



void rc_init(void)
{	
	rc_new_input=0;
	sync=0;
//	TCCR1B|=0x82;		//enable noise canceller, prescaler 8
	TCCR1B|=0x02;		//disable noise canceller, prescaler 8

//	TCCR1B|=0x40;		//trigger on rising egde
	TCCR1B&=0xBF;		//trigger on falling egde
	TIMSK|=0x20;		//enable ICP interrupt
}

