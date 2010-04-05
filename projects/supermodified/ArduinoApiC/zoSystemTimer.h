#ifndef ZO_SYSTEM_TIMER_H
#define ZO_SYSTEM_TIMER_H

#include "zoTypes.h"

//watchdog timer is used to give an ISR every ~16ms
//this is the system timer resolution
//this isr is used for system timing functionality
//watchdog reset functionality is maintained
void zoSystemTimerInit(void);

//watchdog functionality 
void zoSystemTimerWatchDogInit(const u16 timeOutMiliSecond);
void zoSystemTimerWatchDogEnable(void);
void zoSystemTimerWatchDogDisable(void);
void zoSystemTimerWatchDogReset(void);

//timeout functionality 
void zoSystemTimerTimeOutInit(u16 *counter);
bool zoSystemTimerTimeOutExpired(u16 *counter, const u16 timeOutMiliSecond);

//time measuring functionality
//system measured in ms
void zoSystemTimerMeasureStart(u16 *counter);
u16 zoSystemTimerMeasureGet(u16 *counter);

#endif //ZO_SYSTEM_TIMER_H