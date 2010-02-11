#include <avr/io.h>
#include <util/delay.h>
#include "debounce.h"

volatile uint8_t lastState;

void ButtonClicker(uint8_t index,uint8_t state);


/*

Pin-change interrupt 0

	Detects pin changes on PCINT0-7 (all masked on), i.e. PB0-7 on Atmega2560

	It compares these values to the last known state and signals a change
	on any pins that have changed state.

*/
ISR(PCINT0_vect)
{
	uint8_t temp = lastState^PINB;
	lastState = PINB;

	if ((temp & _BV(0)))
		signalChangedState(0,2);
	if ((temp & _BV(1)))
		signalChangedState(1,3);
	if ((temp & _BV(2)))
		signalChangedState(2,4);
	if ((temp & _BV(3)))
		signalChangedState(3,5);
	if ((temp & _BV(4)))
		signalChangedState(4,20);
	if ((temp & _BV(5)))
		signalChangedState(5,50);
	if ((temp & _BV(6)))
		signalChangedState(6,200);
	if ((temp & _BV(7)))
		signalChangedState(7,200);
} 



int main(void)
{
	//Initialize PORTB as all inputs, no internal pull-ups
	DDRB  = 0x00;
	PORTB = 0x00;

	//Initialize PORTD as all outputs, all HIGH (LEDs off)
	DDRD  = 0xFF;
	PORTD = 0xFF;

	//Initial timer setup (does not start timer)
	initializeDebouncerTimer();

	lastState = PINB;

	//Fills in details regarding 
	registerDebouncer(&PINB,PB0,0,1,&ButtonClicker);
	registerDebouncer(&PINB,PB1,1,1,&ButtonClicker);
	registerDebouncer(&PINB,PB2,2,1,&ButtonClicker);
	registerDebouncer(&PINB,PB3,3,1,&ButtonClicker);
	registerDebouncer(&PINB,PB4,4,0,&ButtonClicker);
	registerDebouncer(&PINB,PB5,5,0,&ButtonClicker);
	registerDebouncer(&PINB,PB6,6,0,&ButtonClicker);
	registerDebouncer(&PINB,PB7,7,0,&ButtonClicker);

	//Enable pin-change interrupt & mask
	PCICR  = _BV(PCIE0);
	PCMSK0 = 0xFF;

	//Enable interrupts
	sei();

	while(1)
	{
		//This will loop through any signaled debouncer items and
		//call their handlers (doesn't apply to asynchronous)
		callSignaledHandlers();

		_delay_ms(5);
	}

	return 0;
}

void ButtonClicker(uint8_t index,uint8_t state)
{
	if (state == 0)
	{

		PORTD ^= _BV(index);
	}
}

