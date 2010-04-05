#include <avr/io.h>
#include "zoEncoder.h"
#include "zoMcu.h"
#include <stdlib.h>
#include "zoSms.h"
#include <avr/sfr_defs.h>

#define ENCODER_PWM_FREQUENCY_HZ			975.6
#define ENCODER_TICKS_PER_MICROSEC			1
#define VELOCITY_READ_FREQUENCY_APPROX		10
#define VELOCITY_READ_CONTROL_LOOP_ENTRIES	(ZO_SMS_CONTROL_LOOP_HZ_APPROX/ \
											VELOCITY_READ_FREQUENCY_APPROX)
#define MICROSECONDS_PER_SECOND				1000000UL
#define CLOCK_TICKS_PER_MICROSEC			(F_CPU/MICROSECONDS_PER_SECOND)

static volatile s16 DeltaPos=0;
static volatile u16 PositionAbsolute=0;
static volatile u16 PulseOnTimerTicks;
static volatile u16 PulseOffTimerTicks;

static volatile u08 PinStatePhaseA;
static volatile u08 PinStatePhaseB;
static volatile u08 LastPinStatePhaseA;
static volatile s32 EncoderPos = 0;
static volatile s32 LastEncoderPos = 0;

static volatile u08 NoOfEntriesControlLoopIsr = 1;

static volatile u08 HighLow = 0 ;

void zoEncoderInit(void)
{
	//initialize timer1 used for input capture
	TCCR1A = 0x00;	//all waveform generation disabled
	TCCR1B = 0xC1;	//enable input noise canceller,prescaler set to 1, 
					//capture on rising edge
	enterCritical();
	TCNT1 = 0;
	ICR1 = 0;
	exitCritical();

	zoEncoderIcpIsrEnable();

	PCMSK0 = 0x06;		//enable pin change interrupts on PCINT1 & PCINT2
	PCICR = 0x01;		//enable pin change interrupt 0 for PCINT0-PCINT7
	PCIFR |= _BV(1);	//SW clear interrupt flag
	PinStatePhaseA = ((PINB & 0x02) == 0)?0:1;
	PinStatePhaseB = ((PINB & 0x04) == 0)?0:1;

	PRR &= ~_BV(PRTIM1);

	sei();			//enable global interrupts
}

s32 zoEncoderGetPos(void)
{
	s32 pos;

	enterCritical();
	pos = EncoderPos;
	exitCritical();

	return pos;
}

u16 zoEncoderGetPosAbsolute(void)
{
	u16 posAbs;
	enterCritical();
	posAbs = PositionAbsolute/CLOCK_TICKS_PER_MICROSEC*ENCODER_TICKS_PER_MICROSEC ;
	exitCritical();

	return posAbs;
}

s16 zoEncoderGetVel(void)
{
	s16 vel;

	enterCritical();
	vel = DeltaPos;
	exitCritical();

	vel = (s16)( (double)(vel) *
				 Sms.Control.LoopHz / 
				 (double)VELOCITY_READ_CONTROL_LOOP_ENTRIES );

	return vel;
}

void zoEncoderReset(void)
{
	enterCritical();
	EncoderPos = 0;
	exitCritical();
}

void zoEncoderIcpIsrEnable(void)
{
	enterCritical();
	TCNT1 = 0x0000;
	ICR1 = 0x0000;
	exitCritical();

	TIFR1 |= _BV(ICF1);	//clear ICF1 interrupt flag manually
	TIMSK1 |= _BV(ICIE1);	//enable input capture interrupt
}

void zoEncoderIcpIsrDisable(void)
{
	TIMSK1 &= ~_BV(5);	//disable input capture interrupt
	TIFR1 |= _BV(5);	//clear ICF1 interrupt flag manually

	enterCritical();
	TCNT1 = 0x0000;
	ICR1 = 0x0000;
	exitCritical();
}

ISR(TIMER1_CAPT_vect)
{
	if(  bit_is_set(PINB,0)  )
	{
		TCCR1B &= ~_BV(6);				//configure input capture at falling edge
		PulseOffTimerTicks = ICR1;		//get input capture value
		HighLow++;
	}
	else
	{
		HighLow--;
		TCCR1B |= _BV(6);			//configure input capture at rising edge
		PulseOnTimerTicks = ICR1;	//get input capture value
		TCNT1 = 0x0000;				//initialize timer counter
		
		if(HighLow == 0)		//be sure the pulse had a high and a low state
			PositionAbsolute = PulseOnTimerTicks - PulseOffTimerTicks;
		else
			HighLow = 0;
	}
}

ISR(PCINT0_vect)
{
	LastPinStatePhaseA = PinStatePhaseA;
	PinStatePhaseA = ((PINB & 0x02) == 0)?0:1;
	PinStatePhaseB = ((PINB & 0x04) == 0)?0:1;
	
	if( (PinStatePhaseA ^ PinStatePhaseB) == 0 )
	{
		if( (PinStatePhaseA ^ LastPinStatePhaseA) == 0 )
			EncoderPos++;
		else
			EncoderPos--;
	}
	else
	{
		if( (PinStatePhaseA ^ LastPinStatePhaseA) == 0 )
			EncoderPos--;
		else
			EncoderPos++;
	}
}

//this function needs to run inside the control loop, because accurate timing is needed
//in order to calculate velocity.

void zoEncoderSetVel(void)
{
	if(NoOfEntriesControlLoopIsr++ >= VELOCITY_READ_CONTROL_LOOP_ENTRIES)
	{
		NoOfEntriesControlLoopIsr = 1;
		DeltaPos = (s16)( (EncoderPos - LastEncoderPos) );
		LastEncoderPos = EncoderPos;
	}	
}

inline bool zoEncoderPollOverFlowError(void)
{
	bool ovfl = FALSE;
	
	enterCritical();
	if(	EncoderPos >= ((s32)0x7FFFFC00) )
		ovfl = TRUE;
	exitCritical();

	return ovfl;

}

inline bool zoEncoderPollUnderFlowError(void)
{
	bool unfl = FALSE;
	
	enterCritical();
	if(	EncoderPos <= ((s32)0x80000400) )
		unfl = TRUE;
	exitCritical();

	return unfl;
}
