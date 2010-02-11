#ifndef __DEBOUNCE_H__
#define __DEBOUNCE_H__

#include <avr/interrupt.h>

//Optimized for 20 millisecond period on a 1Mhz clock
#define DBNC_TIMR0_PRESCALER 	_BV(CS02)
#define DBNC_TIMR0_BASEVAL	 	178

//Defines the size of the debouncer handler arrays (i.e. 4 for a maximum of four)
#define DBNC_NUM_DEBOUNCERS		8

//Defines the number of timer ticks before a value is considered legitimate
//This will define a maximum debounce time of approx 100 milliseconds @ 5
//The minimum debounce time will be approx 80 milliseconds
#define DBNC_COUNTER_TOP 3


#define _BITSHIFTBY(bit,num) ((bit)<<(num))


typedef void(*debounceHandler)(uint8_t,uint8_t);

typedef struct
{
	//A pointer to a volatile port (I/O or otherwise)
	volatile uint8_t *port;

	//A pointer to a debounceHandler function
	debounceHandler handler;

	//This is the decremental counter which determines
	//if the button has been debounced
	uint8_t counter;

	/*

		Bits 0-3: bit index to check against (0-7)
		Bits 4-5: unused
		Bit    5: Last known sate
		Bit    6: Signaled
		Bit    7: Asynchronous

	*/
	uint8_t bitmap;
} DBNC_ITEM;

typedef struct
{

	//An array of debouncer units
	DBNC_ITEM dbUnits[DBNC_NUM_DEBOUNCERS];

	//This is set to 1 when any signal in the dbncSignaled array has been set
	uint8_t signalReady;

} DBNC_GLOBAL;

/*
	Forward Declarations
*/


//ISR for timer0 overflow interrupt
ISR(TIMER0_OVF_vect);
void callSignaledHandlers(void);
void registerDebouncer(volatile uint8_t *port,uint8_t bit,uint8_t index,uint8_t Asynchronous,debounceHandler handler);
void signalChangedState(uint8_t index,uint8_t counterTop);
void initializeDebouncerTimer();

#endif
