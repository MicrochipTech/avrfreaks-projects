#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "zoMotor.h"
#include "zoSms.h"
#include "zoMcu.h"

#define TIMER_0_OVERFLOWS_FOR_CONTROL_LOOP  (F_CPU/510/ZO_SMS_CONTROL_LOOP_HZ_APPROX)

static volatile u16 NoOfTimer0OverFlowIsrEntries = 0;
static volatile bool MotorState = FALSE;

static void (*timedEventHandler)(void)=FALSE;

void zoMotorSetTimedEventHandler(void (*timedEventFunction)(void))
{
	timedEventHandler = timedEventFunction;
}

void zoMotorInit(void)
{
	Sms.Control.LoopHz = (double)F_CPU/(double)510/(double)TIMER_0_OVERFLOWS_FOR_CONTROL_LOOP;
		
	//set timer prescalers to zero -> PWM frequency 39215 Hz for phase correct mode
	TCCR0B = 0x01;	//set prescaler to 1
	TCCR2B = 0x01;	//set prescaler to 1

	//configure phase correct mode
	TCCR0B &= ~0x08;	//clear WGM02
	TCCR0A &= ~0x02;	//clear WGM01
	TCCR0A |= 0x01;		//set WGM00
	TCCR0A |= 0x10;		//set COM0B0
	TCCR0A |= 0x20;		//set COM0B1
	TCCR0A &= ~0xC0;	//clear COM0A0 and COM0A1
	
	TCCR2B &= ~0x08;	//clear WGM22
	TCCR2A &= ~0x02;	//clear WGM21
	TCCR2A |= 0x01;		//set WGM20	
	TCCR2A |= 0x10;		//set COM2B0
	TCCR2A |= 0x20;		//set COM2B1
	TCCR2A &= ~0xC0;	//clear COM2A0 and COM2A1

	OCR0B = 0x00;		//set PWM duty to zero
	OCR2B = 0x00;		//set PWM duty to zero

	TIMSK0 |= _BV(0);	//enable timer 0 overflow interrupt
	sei();				//enable global interrupts

	PRR &=~ (_BV(PRTIM0)|_BV(PRTIM2));		//enable timer0 and timer2	
}

void zoMotorSet(s32 pwmDuty)
{
	u08 ocrReg;
	u32 pwmDutyAbsolute;

	pwmDutyAbsolute = labs(pwmDuty);	//compute absolute value of motor command

	pwmDutyAbsolute >>= 8;	//discard lower 8 bits : this is done to allow finer tuning of gains			

	if(pwmDutyAbsolute > 0xFF)
		pwmDutyAbsolute = 0xFF;
	
	ocrReg = (u08)pwmDutyAbsolute;		

	if(pwmDuty >= 0)
	{
		OCR0B = ocrReg;
		OCR2B = 0x00;		//constantly high
	}
	else
	{
		OCR0B = 0x00;		//constantly high
		OCR2B = ocrReg;
	}
}

void zoMotorSetState(bool onOff)
{
	if(onOff)
		PORTD |= _BV(PORT4);
	else
		PORTD &= ~_BV(PORT4);

	zoMotorSet(0);
	enterCritical();
	MotorState = onOff;
	exitCritical();
}

bool zoMotorGetState(void)
{
	bool state;

	enterCritical();
	state = MotorState;
	exitCritical();

	return state;
}

ISR(TIMER0_OVF_vect)
{
	if( NoOfTimer0OverFlowIsrEntries++ >= TIMER_0_OVERFLOWS_FOR_CONTROL_LOOP )
	{
		timedEventHandler();
		NoOfTimer0OverFlowIsrEntries = 0;
	}
}