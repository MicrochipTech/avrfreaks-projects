#include <avr\interrupt.h>
#include "Init.h"

#define CH0Bit 1    //bit 0
#define CH1Bit 2	//bit 1
#define CH2Bit 4	//bit 2
#define CH3Bit 8    //bit 3
#define CH4Bit 16	//bit 4
#define CH5Bit 32	//bit 5
#define CHECKBIT(x,y) (x & (y)) /* Check bit y in byte x*/




/////////////////////////////////////////////////////////////////
////////////////TimerCounter2 Overflow Interrupt /////////////////
//////////////////////////////////////////////////////////////////

ISR(TIMER2_OVF_vect)//the timer just over flows after 1 ms and counts 0 to 255 in the next 1 ms
 {/*					//as servo signal is going to be between 1 and 2 ms this scheme works fine for RC signal decoding
 extern uint8_t Function;
extern uint8_t CounterValue;
CounterValue++;

if (CounterValue >= 1)
{
 Function=1;
	CounterValue=0;
	}*/
 }
 
 
 ////////////////////////////////////////////////////////////////////
 ////////////Pin Change Interrupt 2 /////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////
ISR(PCINT1_vect)
{
extern uint8_t CH0;	//PORTC0,PCINT8 value
extern uint8_t CH1;	//PORTC1,PCINT9 value
extern uint8_t CH2;// PORTC2,PCINT10 value
extern uint8_t CH3;	//PORTC3,PCINT11 value
extern uint8_t CH4;// PORTC4,PCINT12 value
extern uint8_t CH5;	//PORTC5,PCINT13 value
extern uint8_t STATUS_D; //port D value
uint8_t DValue=PINC;
uint8_t temp2=PINC;
static uint8_t temp;
STATUS_D = STATUS_D^DValue;//set bit for pin which changed signal.
temp=STATUS_D;
STATUS_D=temp;
DValue=temp2;
if ((CHECKBIT(STATUS_D, CH0Bit)))// if channel 0 signal changed
{
if ((CHECKBIT(DValue,CH0Bit)))//if ch0 bit was set
TCNT2=0;//initialize timer counter
else CH0=TCNT2; //store timer counter in channel 0
}
STATUS_D=temp;
DValue=temp2;
if ((CHECKBIT(STATUS_D, CH1Bit)))// if channel 1 signal changed
{
if ((CHECKBIT(DValue,CH1Bit)))//if ch1 bit was set
TCNT2=0;//initialize timer counter
else CH1=TCNT2; //store timer counter in channel 1
}
STATUS_D=temp;
DValue=temp2;
if ((CHECKBIT(STATUS_D, CH2Bit)))// if channel 2 signal changed
{
if ((CHECKBIT(DValue,CH2Bit)))//if ch2 bit was set
TCNT2=0;//initialize timer counter
else CH2=TCNT2; //store timer counter in channel 2
}
STATUS_D=temp;
DValue=temp2;
 if ((CHECKBIT(STATUS_D, CH3Bit)))// if channel 3 signal changed
{
if ((CHECKBIT(DValue,CH3Bit)))//if ch3 bit was set
TCNT2=0;//initialize timer counter
else CH3=TCNT2; //store timer counter in channel 3
}
STATUS_D=temp;
 DValue=temp2;
 if ((CHECKBIT(STATUS_D, CH4Bit)))// if channel 4 signal changed
{
if ((CHECKBIT(DValue,CH4Bit)))//if ch4 bit was set
TCNT2=0;//initialize timer counter
else CH4=TCNT2; //store timer counter in channel 4
}
STATUS_D=temp;
 DValue=temp2;
if ((CHECKBIT(STATUS_D, CH5Bit)))// if channel 5 signal changed
{
if ((CHECKBIT(DValue,CH5Bit)))//if ch5 bit was set
TCNT2=0;//initialize timer counter
else CH5=TCNT2; //store timer counter in channel 5
}
STATUS_D=DValue;//store port D data in STATUS_D
}
 


