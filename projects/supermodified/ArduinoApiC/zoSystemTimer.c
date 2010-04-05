#include <avr/io.h>
#include <avr/interrupt.h>
#include "zoSystemTimer.h"
#include "zoMcu.h"


static volatile u16 Miliseconds = 0;
static volatile u16 WatchDogMiliseconds = 0;
static volatile u16 WatchDogTimeOut = 0xFFFF;
static volatile bool WatchDogResetEnabled = FALSE;

void zoSystemTimerInit(void)
{
	//MCUSR &= ~_BV(WDRF);		//allow for WDE clear
	//WDTCSR |= _BV(WDCE);		//enable watchdog change
	WDTCSR = 0xD8;				//0b11011000:WDIF WDIE WDP3 WDCE WDE WDP2 WDP1 WDP0 
								//configure interrupt and system reset mode  
	zoSystemTimerWatchDogDisable();
}

//watchdog functionality 
inline void zoSystemTimerWatchDogInit(const u16 timeOutMiliSecond)
{
	enterCritical();
	WatchDogTimeOut = timeOutMiliSecond;
	exitCritical();
}

inline void zoSystemTimerWatchDogEnable(void)
{
	enterCritical();
	WatchDogResetEnabled = TRUE;
	exitCritical();
}

inline void zoSystemTimerWatchDogDisable(void)
{
	enterCritical();
	WatchDogResetEnabled = TRUE;
	exitCritical();
}

inline void zoSystemTimerWatchDogReset(void)
{
	enterCritical();
	WatchDogMiliseconds = 0;
	exitCritical();
}

//timeout functionality 
inline void zoSystemTimerTimeOutInit(u16 *counter)
{
	zoSystemTimerMeasureStart(counter);
}

inline bool zoSystemTimerTimeOutExpired(u16 *counter, const u16 timeOutMiliSecond)
{
	return( ( zoSystemTimerMeasureGet(counter) >= timeOutMiliSecond )?TRUE:FALSE );
}

//time measuring functionality
//system measured in ms
inline void zoSystemTimerMeasureStart(u16 *counter)
{
	enterCritical();
	*counter = Miliseconds;
	exitCritical();
}

u16 zoSystemTimerMeasureGet(u16 *counter)
{
	u16 curr;
	
	enterCritical();
	curr= Miliseconds;	
	exitCritical();

	if(*counter <= curr)
		return (curr - *counter);
	else
		return (0xFFFF + curr -*counter); 
}

ISR(WDT_vect)
{
	Miliseconds+=16;
	WatchDogMiliseconds+=16;

	if( (WatchDogMiliseconds <= WatchDogTimeOut) || (!WatchDogResetEnabled) )
		WDTCSR |= _BV(WDCE)|_BV(WDIE);	//avoid reseting MCU			
}