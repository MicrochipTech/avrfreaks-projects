/*
 * dlg.h
 *
 *  Created on: 2009-09-02
 *      Author: liku
 */

#ifndef DLG_H_
#define DLG_H_

#include <stdbool.h>
#include <stdint.h>

#include <avr/pgmspace.h>

#include "rtc.h"

void dlgInfo_P(PGM_P info);
bool dlgYesNo(char* label);
bool dlgSetTime(tm* t, bool limit24h, bool atleast1s);
bool dlgSetVar(int32_t* val, int32_t min, int32_t max, void (*callback)(int32_t val), char postfix);
int32_t dlgIncVar(int32_t val, int32_t incval, int32_t min, int32_t max, bool roundrobin);
void dlgPrintSel(char *mask, uint8_t idx, char mark);
void dlgPrintTimeLeft(tm* t);
void dlgPrintBulb(tm* t);
void dlgPrintCountAndTime(uint16_t count, uint16_t total, tm* t);

#endif /* DLG_H_ */
