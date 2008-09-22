#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay_basic.h>

#include "config.h"
#include "serial.h"
#include "rms64.h"
#include "mmc.h"
#include "pcf-rtc.h"
#include "logger.h"
#include "stack-check.h"
#include "io_stuff.h"
#include "KS0108.h"

#define FUNC_SET_TIME 0
#define FUNC_CALIB_ADC 1
#define FUNC_ERASE_SDCARD 2
#define FUNC_TEST_SDCARD 3
#define FUNC_MENU 4
static const char menulab0[] PROGMEM = "Set time";
static const char menulab1[] PROGMEM = "Calibrate ADC";
static const char menulab2[] PROGMEM = "Erase SD card";
static const char menulab3[] PROGMEM = "Test SD card";
static PGM_P menu_labels[4] = { // FIXME - need memcpy_P?
	menulab0,
    menulab1,
    menulab2,
    menulab3
};

static uint8_t isnum(char p) {
	return p >= '0' && p <= '9';
}

static uint8_t number_setter(char *str) {
	int8_t p = 0;
	// str had better contain a digit!
	while(!isnum(str[p])) p++;
	for(;;) {
		for(int8_t q=0; str[q]; q++) {
			if(p == q) putchr(CHAR_INVERSE);
			putchr(str[q]);
			putchr(CHAR_NORMAL);
		}
		putchr('\r');

		while(!buttons_x && !buttons_y) {
			wdt_reset();
			poll_buttons();
		}

		if(buttons_x < 0) {
			buttons_x = 0;
			do { p--; } while(p >= 0 && !isnum(str[p]));
			if(p < 0) return 0;
		}
		if(buttons_x > 0) {
			buttons_x = 0;
			do { p++; } while(str[p] && !isnum(str[p]));
			if(!str[p]) return 1;
		}
		if(buttons_y < 0) {
			if(str[p] == '9') str[p] = '0';
			else str[p]++;
		}
		if(buttons_y > 0) {
			if(str[p] == '0') str[p] = '9';
			else str[p]--;
		}
		buttons_y = 0;
	}
}

static void set_time() {
	wdt_reset();

	clear_screen();
	printP("Set time\n");
	printP("MM-DD HH:MM:SS\n");

	char str[14];
	rtc_string_for_setter(0, str);

	uint8_t ok = number_setter(str);
	if(!ok) {
		printP("Cancelled.\n");
		return;
	}

	rtc_set_from_string(0, str);
}

static void write_calibration_to_eeprom() {
    eeprom_write_block(&calib, 0, sizeof(calib_t));
}

// range 0 for V
static void show_calib(uint8_t range) {
	printP("Calibration is\n");
	print_int32(calib.A[range]);
	printP("/");
	print_int32(calib.B[range]);
	printP("\n");
}

static void print_range_name(uint8_t range) {
	if(range==0) printP("Voltage");
	else {
		printP("Current ");
		if(range==1)      printP("lo");
		else if(range==2) printP("med");
		else              printP("hi");
		printP(" gain");
	}
}

static void calibrate_adc_range(uint8_t range) {
	wdt_reset();

	clear_screen();
	print_range_name(range);
	printP("\n");

	const char *template = PSTR("000.00");
	char str[7];
	strcpy_P(str, template);
	if(range == 0) str[5] = 0;

	uint8_t ok = number_setter(str);
	if(!ok) {
		printP("Cancelled.\n");
		return;
	}

	uint16_t realval = 0;
	for(uint8_t i=0; str[i]; i++) {
		if(isnum(str[i])) {
			realval *= 10;
			realval += str[i] - '0';
		}
	}

	if(range) adc_wanted_range = range;

	// first reading may already be full
	// second could be aborted due to range change
	// third will be valid
	for(uint8_t i=0; i<3; i++) {
		printP("Waiting for result\n");
		while(!adc_result_ready) wdt_reset();
		adc_result_ready = 0;
	}

	uint8_t oob;
	if(range) {
		oob = (adc_result.min_I < ADC_BOUND_MIN || adc_result.max_I > ADC_BOUND_MAX);
	} else {
		oob = (adc_result.min_V < ADC_BOUND_MIN || adc_result.max_V > ADC_BOUND_MAX);
	}
	if(oob) {
		printP("Error: too much signal is being applied to input.\n");
		return;
	}
	if(range && adc_result.range != range) {
		printP("Error: range could not be set.\n");
		return;
	}

	rms_t rms = compute_rms();

	calib.A[range] = realval;
	if(range) {
		calib.B[range] = rms.I;
	} else {
		calib.B[0] = rms.V;
	}

	show_calib(range);

	write_calibration_to_eeprom();
}

static void calibrate_adc() {
	uint8_t range = 0;

	for(;;) {
		wdt_reset();
		poll_buttons();

		if(buttons_y < 0) {
			if(range) range--;
			else range = 3;
			buttons_y = 0;
		}

		if(buttons_y > 0) {
			if(range==3) range = 0;
			else range++;
			buttons_y = 0;
		}

		if(buttons_x < 0) {
			buttons_x = 0;
			return;
		}

		if(buttons_x > 0) {
			buttons_x = 0;

			calibrate_adc_range(range);
			for(uint8_t i=30; i; i--) {
				_delay_loop_2(65535);
				wdt_reset();
			}
		}

		#if GLCD_OVERPAINT && USE_GLCD
		cursor_to_top();
		#else
		clear_screen();
		#endif

		printP("\3Calibration\1\n");
		for(uint8_t i=0; i<4; i++) {
			putchr(range==i ? CHAR_INVERSE : CHAR_NORMAL);
			print_range_name(i);
			putchr('\n');
		}
		putchr(CHAR_NORMAL);

		#if GLCD_OVERPAINT && USE_GLCD
		clear_to_screen_end();
		#endif
	}
}

static void test_sdcard() {
	for(;;) {
		#if GLCD_OVERPAINT && USE_GLCD
		clear_to_screen_end();
		#endif

		_delay_loop_2(65535);

		#if GLCD_OVERPAINT && USE_GLCD
		cursor_to_top();
		#else
		clear_screen();
		#endif

		wdt_reset();
		poll_buttons();
		if(buttons_x || buttons_y) {
			buttons_x = buttons_y = 0;
			return;
		}

		pcf_time_t time;
		pcf_read_time(0, &time);
		rtc_print_pretty_date(&time);
		putchr(' ');
		rtc_print_pretty_time(&time);
		putchr('\n');

		wdt_reset();
		release_glcd(); // FIXME - not needed
		#if DEBUG_MMC_INIT
		show_mmc_debug_init = 1;
		#endif
		DSTATUS disk_stat = disk_initialize();
		#if DEBUG_MMC_INIT
		show_mmc_debug_init = 0;
		#endif
		wdt_reset();
		print_keyP_val32("disk_initialize=", disk_stat);
		if(disk_stat) continue;

		wdt_reset();
		// FIXME - this corrupts the accumulator, so clear it upon exit
		DRESULT disk_rstat = disk_read(0);
		wdt_reset();
		print_keyP_val32("disk_rstat=", disk_rstat);
		if(disk_rstat != RES_OK) continue;

		printP("read:");
		for(uint8_t i=0; i<8; i++)
			print_hex8(mmc_buf[i]);
		putchr('\n');
	}
}

void settings_menu() {
	uint8_t menu_pos = 0;

	for(;;) {
		wdt_reset();
		poll_buttons();

		if(buttons_y < 0) {
			if(menu_pos) menu_pos--;
			else menu_pos = FUNC_MENU-1;
			buttons_y = 0;
		}

		if(buttons_y > 0) {
			if(menu_pos==FUNC_MENU-1) menu_pos = 0;
			else menu_pos++;
			buttons_y = 0;
		}

		if(buttons_x < 0) {
			buttons_x = 0;
			return;
		}

		if(buttons_x > 0) {
			buttons_x = 0;
			if(menu_pos == FUNC_SET_TIME) {
				set_time();
			} else if(menu_pos == FUNC_CALIB_ADC) {
				calibrate_adc();
			} else if(menu_pos == FUNC_ERASE_SDCARD) {
				clear_screen();
				logger_erase_disk();
				for(uint8_t i=30; i; i--) {
					_delay_loop_2(65535);
					wdt_reset();
				}
			} else if(menu_pos == FUNC_TEST_SDCARD) {
				test_sdcard();
			}
		}

		#if GLCD_OVERPAINT && USE_GLCD
		cursor_to_top();
		#else
		clear_screen();
		#endif

		printP("\3Calibration\1\n");
		for(uint8_t i=0; i<FUNC_MENU; i++) {
			putchr(menu_pos==i ? CHAR_INVERSE : CHAR_NORMAL);
			printstr_p(menu_labels[i]);
			putchr('\n');
		}
		putchr(CHAR_NORMAL);

		#if GLCD_OVERPAINT && USE_GLCD
		clear_to_screen_end();
		#endif
	}
}
