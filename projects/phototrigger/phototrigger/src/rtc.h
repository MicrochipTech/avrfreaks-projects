/*
 * rtc.h
 *
 *  Created on: 2009-08-30
 *      Author: liku
 *
 *      Header file for RTC. Contains some constants related with timeouts.
 */

#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>
#include <stdbool.h>

#define RTCSTATTIMERCOUNT 2  // how many timers to handle (currently only 1 is used)
#define RTCINACTIVITYTIME 10 // how many seconds before dimming the backlight
#define RTCPWRONTIME      2  // how much time, in seconds, to power on (holding switch key)

// semi implementation of tm structure
struct _tm
{
	uint8_t tm_sec;
	uint8_t tm_min;
	uint8_t tm_hour;
};
typedef struct _tm tm;


void rtcGetSysTime(tm* currtime);
void rtcSetSysTime(tm* nt);
uint32_t rtcGetStatTimer(uint8_t i);
uint32_t rtcTm2Int(tm* t);
void rtcInt2Tm(uint32_t ti, tm* t);
void rtcSetStatTimer(uint8_t i, uint32_t val);
void rtcSyncToSec(void);
bool rtcGetPwrOnFlag(void);
void rtcUpdInactTimer(void);
void rtcWaitMs(uint16_t t);
void rtcDec1sFromTm(tm* t);

#endif /* RTC_H_ */
