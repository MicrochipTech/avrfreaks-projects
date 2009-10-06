/*
 * pt.c
 *
 *  Created on: 2009-09-01
 *      Author: liku
 *
 *      This file contains actual implementation of phototrigger: menus,
 *      handling different actions, etc.
 */

#include <stdbool.h>

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>
#include <avr/power.h>

#include <util/atomic.h>
#include <util/delay.h>

#include "pt.h"
#include "hw.h"
#include "enc.h"
#include "lcd.h"
#include "dlg.h"
#include "tinymenu.h"
#include "eeprom.h"

/*
 * **************************** MENU (scroll down for code) *************************
 */

#define OPTION_STARTNOW   0 /* symbol for immediate start */
#define OPTION_STARTLATER 1 /* symbol to start later */

menu_t TL_menu =
{
	.top_entry = 0,
	.current_entry = 0,
	.entry =
	{
		{
			.flags = 0,
			.select = handleTL,
			.name = "Start now",
			.value = OPTION_STARTNOW,
		},
		{
			.flags = 0,
			.select = handleTL,
			.name = "Start at",
			.value = (void*) OPTION_STARTLATER,
		},
		{
			.flags = MENU_FLAG_EXITMENU | MENU_FLAG_FORGETPOS,
			.select = 0,
			.name = "Back",
			.value = 0,
		}

   },
   .num_entries = 3,
   .previous = 0,
};

menu_t bulb_menu =
{
	.top_entry = 0,
	.current_entry = 0,
	.entry =
	{
		{
			.flags = 0,
			.select = handleBulb,
			.name = "Start now",
			.value = OPTION_STARTNOW,
		},
		{
			.flags = 0,
			.select = handleBulb,
			.name = "Start at",
			.value = (void*) OPTION_STARTLATER,
		},
		{
			.flags = MENU_FLAG_EXITMENU | MENU_FLAG_FORGETPOS,
			.select = 0,
			.name = "Back",
			.value = 0,
		}

   },
   .num_entries = 3,
   .previous = 0,
};

menu_t settings_menu =
{
	.top_entry = 0,
	.current_entry = 0,
	.entry =
	{
		{
			.flags = 0,
			.select = handleSetTime,
			.name = "Set clock",
			.value = 0,
		},
		{
			.flags = 0,
			.select = handleLCDBacklight,
			.name = "LCD brightness",
			.value = 0,
		},
		{
			.flags = 0,
			.select = handleLCDContr,
			.name = "LCD contrast",
			.value = 0,
		},
		{
			.flags = 0,
			.select = handleShutRelTime, // FIXME
			.name = "Shut. rel. time",
			.value = 0,
		},
		{
			.flags = 0,
			.select = handleBattVoltage,
			.name = "Batt. voltage",
			.value = 0,
		},
		{
			.flags = 0,
			.select = handleClock,
			.name = "Clock",
			.value = 0,
		},
		{
			.flags = 0,
			.select = handleAbout,
			.name = "About",
			.value = 0,
		},
		{
			.flags = MENU_FLAG_EXITMENU | MENU_FLAG_FORGETPOS,
			.select = 0,
			.name = "Back",
			.value = 0,
		}

   },
   .num_entries = 8,
   .previous = 0,
};

menu_t main_menu =
{
	.top_entry = 0,
	.current_entry = 0,
	.entry =
	{
		{
			.flags = MENU_FLAG_SUBMENU,
			.select = 0,
			.name = "Timelapse mode",
			.value = &TL_menu,
		},
		{
			.flags = MENU_FLAG_SUBMENU,
			.select = 0,
			.name = "Bulb mode",
			.value = &bulb_menu,
		},
		{
			.flags = MENU_FLAG_SUBMENU,
			.select = 0,
			.name = "Settings & info",
			.value = &settings_menu,
		},
		{
			.flags = MENU_FLAG_FORGETPOS,
			.select = handlePowerOff,
			.name = "Power off",
			.value = 0,
		}
	},
	.num_entries = 4,
	.previous = 0,
};

menu_context_t menu_context = {
   .x_loc = 0,
   .y_loc = 0,
   .height = 2,
   .width = 16,
   .menu = 0,
};

/*
 * ************************************* CODE **********************************
 */

/*
 * Handles power off
 */
void handlePowerOff(void *arg, char *name)
{
	hwEnterPwrSave();
}

/*
 * Show about screen (author and version)
 */
void handleAbout(void *arg, char *name)
{
	dlgInfo_P(PSTR("phototrigger by:\nLukasz Goralczyk"));
	lcd_setxy(0, 0);
	printf_P(PSTR("v%-10s   OK\n"), HWVERSIONSTRING);
	printf_P(PSTR("              %c%c"), LCDCHARARRVERT, LCDCHARARRVERT);
	while(encGetKey(KEYNOWAIT) != KEYSWITCH);
}

/*
 * Handles timelapse
 */
void handleTL(void *arg, char *name)
{
// simple state machine
#define TL_SETSTARTTIME 0
#define TL_SETCOUNT     1
#define TL_SETPERIOD    2
#define TL_BULB         3
#define TL_INFO         4

	uint8_t mode;
	if(arg == (void*) OPTION_STARTLATER)
		mode = TL_SETSTARTTIME;
	else
		mode = TL_SETCOUNT;

	tm starttime, period, bulb;
	int32_t total = 0;
	bool starttimeset = false; // flag used to only once retrieve system time

	while(1)
	{
		switch(mode)
		{
		case TL_SETSTARTTIME:
			dlgInfo_P(PSTR("Set start time"));
			if(!starttimeset)
			{
				rtcGetSysTime(&starttime);
				starttimeset = true; // get system time only once
			}
			if(dlgSetTime(&starttime, true, false))
				mode = TL_SETCOUNT;
			else
				return; // exit if back is pressed
			break;

		case TL_SETCOUNT:
			dlgInfo_P(PSTR("Set no of photos\n0 - infinite"));
			total = eeprom_read_word(&eetltotal); // read stored value from eeprom
			if(dlgSetVar(&total, 0, 20000, 0, ' '))
			{
				eeprom_write_word(&eetltotal, total); // write new value to eeprom
				mode = TL_SETPERIOD;
			}
			else if(arg == (void*) OPTION_STARTLATER)
				mode = TL_SETSTARTTIME;
			else
				return;
			break;

		case TL_SETPERIOD:
			dlgInfo_P(PSTR("Set time dist.\nbetween photos"));
			eeprom_read_block(&period, &eetlperiod, sizeof(tm));  // read stored value from eeprom
			if(dlgSetTime(&period, false, true))
			{
				eeprom_write_block(&period, &eetlperiod, sizeof(tm)); // write new value to eeprom
				mode = TL_BULB;
			}
			else
				mode = TL_SETCOUNT;
			break;

		case TL_BULB:
			dlgInfo_P(PSTR("Bulb time\n0 - no bulb"));
			eeprom_read_block(&bulb, &eetlbulb, sizeof(tm));  // read stored value from eeprom
			if(dlgSetTime(&bulb, false, false))
			{
				eeprom_write_block(&bulb, &eetlbulb, sizeof(tm)); // write new value to eeprom
				mode = TL_INFO;
			}
			else
				mode = TL_SETPERIOD;
			break;

		case TL_INFO:
			/**************** TIMELAPSE HANDLER *****************/
			if(arg == (void*) OPTION_STARTLATER) // should we wait some time for start?
				if(!waitForStart(&starttime))
					return;

			bool     inf;                // infinite number of photos?
			bool     dobulbflag = false; // Do bulb?
			int32_t  count      = 0;     // up counter of photos
			uint16_t shutreltm  = eeprom_read_word(&eetlshutreltime); // shutter release time

			rtcDec1sFromTm(&period); // we'll lose this second when syncing

			inf = (total ? false : true); // infinite number of photos?

			if(rtcTm2Int(&bulb)) // since rtcTm2Int may be CPU intensive - compute once
			{
				rtcDec1sFromTm(&bulb); // we'll lose this second when syncing
				dobulbflag = true;
			}

			dlgInfo_P(PSTR("Starting..."));

			/********************* MAIN LOOP ********************/
			while(1)
			{
				// release the shutter! (check the bulb option)
				if(dobulbflag)
				{
					if(!doBulb(&bulb))
						return;
				}
				else // no bulb
				{
					hwShutOn();
					rtcWaitMs(shutreltm);
					hwShutOff();
				}

				count++;

				// last photo was made?
				if((count >= total) && (inf == false))
				{
					// refresh photo count display
					dlgPrintCountAndTime(count, total, &period);
					// show that we're finished and wait for user interaction
					// note: cursor should be in second line already (first line contains info about photo count)
					printf_P(PSTR("\rDone,press a key"));
					encGetKey(KEYWAITFORKEY);
					return;
				}

				/******************* WAIT FOR NEXT PHOTO ***************/
				if(!waitForNextPhoto(&period, count, total))
					return;
			} // <--- the big loop
			break;
		}
	}
}

/*
 * Bulb shutter release (shutter open for bulb time)
 * returns true - all ok, false - user canceled operation
 */
bool doBulb(tm* bulb)
{
	rtcSyncToSec(); // sync to a full second
	rtcSetStatTimer(0, rtcTm2Int(bulb));
	hwShutOn();
	/********************** BULB LOOP ************/
	uint32_t timer;
	tm tl;
	while(1)
	{
		timer = rtcGetStatTimer(0);
		rtcInt2Tm(timer, &tl);

		dlgPrintBulb(&tl);

		if(!timer) //  bulb finished!
			break;

		if(encGetKey(KEYNOWAIT) == KEYSWITCH)
			if(dlgYesNo("Cancel?"))
			{
				hwShutOff();
				return false;
			}
	}
	hwShutOff();

	return true;
}

/*
 * Waits for next photo
 * returns true - all OK, false - user cancelled operation
 */
bool waitForNextPhoto(tm* period, uint16_t count, uint16_t total)
{
	rtcSyncToSec(); // sync to a full second
	rtcSetStatTimer(0, rtcTm2Int(period));

	uint32_t timer;
	tm tl;

	while(1)
	{
		timer = rtcGetStatTimer(0);

		rtcInt2Tm(timer, &tl);
		dlgPrintCountAndTime(count, total, &tl);

		if(!timer)
			break;

		// cancel?
		if(encGetKey(KEYNOWAIT) == KEYSWITCH)
			if(dlgYesNo("Cancel?"))
				return false;
	}

	return true;
}

/*
 * Waits until start time == current time (with cancel)
 * returns true - all OK, false - user cancelled operation
 */
bool waitForStart(tm* starttime)
{
	// we need to wait for a start...
	while(1)
	{
		dlgPrintTimeLeft(starttime);
		if(encGetKey(KEYNOWAIT) == KEYSWITCH)
			if(dlgYesNo("Cancel?"))
				return false;

		tm currtm;
		rtcGetSysTime(&currtm);
		if((currtm.tm_hour == starttime->tm_hour) &&
			(currtm.tm_min == starttime->tm_min) &&
			(currtm.tm_sec == starttime->tm_sec)) // note: probably in rare cases we may miss the match
		{
			break;
		}
		_delay_ms(50); // delay to avoid flickering
	}

	return true;
}

/*
 * Handle bulb menu
 */
void handleBulb(void *arg, char *name)
{
// local state machine
#define BL_SETSTARTTIME 0
#define BL_BULB         1
#define BL_INFO         2

	uint8_t mode;
	if(arg == (void*) OPTION_STARTLATER)
		mode = BL_SETSTARTTIME;
	else
		mode = BL_BULB;

	tm starttime, bulb;
	bool starttimeset = false; // flag to set start time only once

	while(1)
	{
		if(mode == BL_SETSTARTTIME)
		{
			dlgInfo_P(PSTR("Set start time"));
			if(!starttimeset) // get time from system only once
			{
				rtcGetSysTime(&starttime);
				starttimeset = true;
			}
			if(dlgSetTime(&starttime, true, false))
				mode = BL_BULB;
			else
				return; // exit if back is pressed
		}
		else if(mode == BL_BULB)
		{
			dlgInfo_P(PSTR("Set bulb time"));
			eeprom_read_block(&bulb, &eebbulb, sizeof(tm));  // read remembered value from EEPROM
			if(dlgSetTime(&bulb, false, true))
			{
				eeprom_write_block(&bulb, &eebbulb, sizeof(tm)); // write new value to EEPROM
				mode = BL_INFO;
			}
			else // go back, but where?
			{
				if(arg == (void*) OPTION_STARTLATER)
					mode = BL_SETSTARTTIME;
				else
					return;
			}

		}
		else if(mode == BL_INFO)
		{
			if(arg == (void*) OPTION_STARTLATER)
				if(!waitForStart(&starttime))
					return;

			dlgInfo_P(PSTR("Starting..."));

			doBulb(&bulb);

			// inform user that we're done
			lcd_clr();
			printf_P(PSTR("Bulb done\nPress any key"));
			encGetKey(KEYWAITFORKEY);

			return;
		}
	}
}

/*
 * Handles setting a time
 */
void handleSetTime(void *arg, char *name)
{
	tm t;
	rtcGetSysTime(&t);
	if(dlgSetTime(&t, true, false))
		rtcSetSysTime(&t);
}


/*
 * Glue callback function for setVarDlg()
 */
void glueSetBL(int32_t v)
{
	lcd_setbacklight((v * 1023l) / 100);
}

/*
 * Handles setting LCD backlight
 */

void handleLCDBacklight(void *arg, char *name)
{
	int32_t t = (100l * OCR1A) / 1023l;
	if(dlgSetVar(&t, 0l, 100l, glueSetBL, '%'))
		eeprom_write_word(&eebacklight, (t * 1023l) / 100);
	else
		lcd_setbacklight(eeprom_read_word(&eebacklight));
}

/*
 * Glue callback function for setVarDlg()
 */
void glueSetContr(int32_t v)
{
	lcd_setcontrast((v * 255l) / 100l);
}

/*
 * Handles setting of contrast
 */
void handleLCDContr(void* arg, char* name)
{
	int32_t t = (100l * OCR0A) / 255l;
	if(dlgSetVar(&t, 0l, 100l, glueSetContr, '%'))
		eeprom_write_byte(&eecontr, (t * 255l) / 100l);
	else
		lcd_setcontrast(eeprom_read_byte(&eecontr));
}

/*
 * Display current (system time)
 */
void handleClock(void *arg, char *name)
{
	tm t;

	while(encGetKey(KEYNOWAIT) != KEYSWITCH)
	{
		rtcGetSysTime(&t);
		lcd_setxy(0, 0);
		printf_P(PSTR("%02d:%02d:%02d      OK\n              %c%c"), t.tm_hour, t.tm_min, t.tm_sec, LCDCHARARRVERT, LCDCHARARRVERT);
	}
}

/*
 * Main function to handle "GUI"
 */
void startPT(void)
{
	menu_enter(&menu_context, &main_menu);

	while (1)
	{
		key_t k = encGetKey(KEYWAITFORKEY);

		if(k == KEYRIGHT)
			menu_next_entry(&menu_context);
		else if(k == KEYLEFT)
			menu_prev_entry(&menu_context);
		else if(k == KEYSWITCH)
			menu_select(&menu_context);

		_delay_ms(200); // extra delay when moving around menu
	}
}

/*
 * Displays battery voltage
 */
void handleBattVoltage(void* arg, char* name)
{
	uint16_t vol, res;

	while(encGetKey(false) != KEYSWITCH)
	{
		vol = hwGetBattVol();
		res = hwMulCoeff(vol, 11, 2560);

		lcd_setxy(0, 0);
		printf_P(PSTR("Vbatt = %1u.%02uV OK\n              %c%c"), res >> 8, res & 0xff, LCDCHARARRVERT, LCDCHARARRVERT);
		_delay_ms(100);
	}
}

/*
 * Handle setting of shutter release time
 */
void handleShutRelTime(void* arg, char* name)
{
	int32_t v = eeprom_read_word(&eetlshutreltime);
	if(dlgSetVar(&v, 1, 500, 0, LCDCHARMS))
		eeprom_write_word(&eetlshutreltime, v);
}
