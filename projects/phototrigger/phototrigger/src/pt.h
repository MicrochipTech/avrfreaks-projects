/*
 * pt.h
 *
 *  Created on: 2009-09-01
 *      Author: li
 *
 *      Header file for actual implementation of phototrigger.
 */

#ifndef PT_H_
#define PT_H_

#include <stdbool.h>
#include <stdint.h>

#include "rtc.h"

void startPT(void);
void handlePowerOff(void* arg, char* name);
void handleAbout(void* arg, char* name);
void handleBulb(void* arg, char* name);
void handleTL(void* arg, char* name);
void handleSetTime(void* arg, char* name);
void handleLCDBacklight(void *arg, char* name);
void handleLCDContr(void* arg, char* name);
void handleBulb(void* arg, char* name);
void handleTL(void* arg, char* name);
void handleClock(void* arg, char* name);
void handleShutRelTime(void* arg, char* name);
void handleBattVoltage(void* arg, char* name);
void handleShutRelTime(void* arg, char* name);
bool waitForStart(tm* starttime);
bool doBulb(tm* bulb);
bool waitForNextPhoto(tm* period, uint16_t count, uint16_t total);

#endif /* PT_H_ */
