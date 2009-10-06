/*
 * main.c
 *
 *  Created on: 2009-09-01
 *      Author: liku
 *
 *      Main program routine - here everything starts.
 *
 *      Changelog:
 *      2009-09-27 - corrected bug when TL finishes timer counts one more time
 */

#include <stdint.h>
#include <stdbool.h>

#include <avr/eeprom.h>

#include <util/delay.h>

#include "hw.h"
#include "lcd.h"
#include "pt.h"
#include "dlg.h"
#include "rtc.h"
#include "eeprom.h"
#include "enc.h"


/*
 * Small check if EEPROM data is correct
 */
bool checkMagic(void)
{
	uint32_t magic = eeprom_read_dword(&eemagic);
	if(magic != 1885893746l) // 'phtr'
		return false;
	else
		return true;
}

/*
 * Small function to set new time
 */
void setNewTime(void)
{
	dlgInfo_P(PSTR("Set new time"));
	tm newtime = { .tm_hour = 12, .tm_min = 0, .tm_sec = 0 };
	if(dlgSetTime(&newtime, true, false))
		rtcSetSysTime(&newtime); // even if user doesn't want to set time we'll set it to 12:00:00
}

/*
 * Main routine
 */
int main(void)
{
	// if we're here than this is the first time after power was completely off (no batteries, no nothing)
	hwSetupAll();

	// after setup enter power save and wait for power on
	hwEnterPwrSave();

	// eeprom's fine? (meh, we already read something from it in previous function)
	// this is to warn user if he/she forgot to program EEPROM
	if(!checkMagic())
		dlgInfo_P(PSTR("EEPROM data may\nbe inv. or corr."));

	// try to set the time
	setNewTime();

	// start phototrigger
	startPT();

	return 0;
}
