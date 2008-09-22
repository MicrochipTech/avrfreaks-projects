#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <util/delay_basic.h>

#include "config.h"
#include "serial.h"
#include "rms64.h"
#include "mmc.h"
#include "pcf-rtc.h"
#include "calibrate.h"
#include "calib2.h"
#include "logger.h"
#include "stack-check.h"
#include "oscope.h"
#include "stripchart.h"
#include "io_stuff.h"
#include "KS0108.h"

#define FUNC_VALS 0
#define FUNC_SCOPE_V 1
#define FUNC_SCOPE_I 2
#define FUNC_CHART 3
#define FUNC_CALIB 4
#define FUNC_MENU 5
static uint8_t current_function = 0;
static uint8_t menu_pos = 0;
static const char menulab0[] PROGMEM = "Show power values";
static const char menulab1[] PROGMEM = "Scope: voltage";
static const char menulab2[] PROGMEM = "Scope: current";
static const char menulab3[] PROGMEM = "Power chart";
static const char menulab4[] PROGMEM = "Calibrate";
static PGM_P menu_labels[5] = { // FIXME - need memcpy_P?
	menulab0,
    menulab1,
    menulab2,
    menulab3,
    menulab4
};

pcf_time_t sample_time;
rms_t rms;

ISR(TIMER1_COMPA_vect) {
#if USE_MMC
	disk_timerproc();
#endif
	iostuff_tick();
}

static void timer_init(void) {
	TCCR1A = 0;
	TCCR1B = _BV(CS10) | _BV(CS12) | _BV(WGM12);
	OCR1A = 156; // 100Hz
	TIMSK1 = _BV(OCIE1A);
}

static uint8_t poll_adc_accum() {
	if(!adc_result_ready) return 0;

	pcf_read_time(0, &sample_time);

	if(!adc_result.is_aborted) {
		rms = compute_rms();
		rms_to_decimal(&rms, adc_result.range);

		// FIXME - don't use labs() here
		push_stripchart(labs(rms.P));
	}

#if USE_MMC
	accum_powerlog(&sample_time);
#endif

	adc_result_ready = 0;

	return 1;
}

static uint8_t do_menu() {
	if(buttons_y < 0) {
		if(menu_pos) menu_pos--;
		else menu_pos = FUNC_MENU-1;
	}
	if(buttons_y > 0) {
		if(menu_pos==FUNC_MENU-1) menu_pos = 0;
		else menu_pos++;
	}
	buttons_y = 0;

	if(buttons_x > 0) {
		current_function = menu_pos;
		buttons_x = 0;
		return 1;
	}
	buttons_x = 0;

	#if GLCD_OVERPAINT && USE_GLCD
	cursor_to_top();
	#else
	clear_screen();
	#endif

	rtc_print_pretty_time(&sample_time);
	putchr(' ');
	print_decimal(rms.P, 2);
	printP("W\n");

	for(uint8_t i=0; i<FUNC_MENU; i++) {
		putchr(menu_pos==i ? CHAR_INVERSE : CHAR_NORMAL);
		printstr_p(menu_labels[i]);
		putchr('\n');
	}
	putchr(CHAR_NORMAL);

	#if GLCD_OVERPAINT && USE_GLCD
	clear_to_screen_end();
	#endif

	return 0;
}

static void show_power_vals() {
	#if GLCD_OVERPAINT && USE_GLCD
	cursor_to_top();
	#else
	clear_screen();
	#endif

	//printP("buttons=");
	//print_int32(buttons_x);
	//putchr(',');
	//print_int32(buttons_y);
	//putchr('\n');

	rtc_print_pretty_date(&sample_time);
	putchr(' ');
	rtc_print_pretty_time(&sample_time);
	putchr('\n');

	wdt_reset();

	if(adc_result.is_aborted) {
		printP("aborted\n");
	} else {
		print_decimal(rms.V, 1);
		printP("V ");
		print_decimal(rms.I, 2);
		printP("A\n");
		print_decimal(rms.P, 2);
		printP("W\n");
	}

	print_powerlog_idx();

	show_free_stack();

	#if GLCD_OVERPAINT && USE_GLCD
	clear_to_screen_end();
	#endif
}

void main_loop() {
	wdt_reset();

	uint8_t got_adc = poll_adc_accum();

	wdt_reset();

	poll_buttons();

	if(current_function != FUNC_MENU) {
		if(buttons_x) {
			buttons_x = buttons_y = 0;
			current_function = FUNC_MENU;
		}
	}

	wdt_reset();

	uint8_t changed_mode = 0;
	if(current_function == FUNC_MENU) {
		// FIXME - relocate this? it needs to be called when scope exits
		adc_wanted_range = 0; // auto-range
		// FIXME - only redraw on button press or adc
		changed_mode = do_menu();
	}

	if(current_function == FUNC_VALS) {
		if(got_adc || changed_mode) show_power_vals();
	} 
	else if(current_function == FUNC_CALIB) {
		settings_menu();
		current_function = FUNC_MENU;
	}
#if USE_OSCOPE && USE_GLCD
	else if(current_function == FUNC_SCOPE_V || current_function == FUNC_SCOPE_I) {
		if(changed_mode) {
			if(current_function == 1) {
				oscope_channel = 0;
				oscope_full = 0;
			} else if(current_function == 2) {
				oscope_channel = 1;
				oscope_full = 0;
			}
			clear_screen();
			printP("Wait for signal");
		} else if(oscope_full) {
			cursor_to_top();
			if(current_function == FUNC_SCOPE_V) {
				print_int32(adc_result.min_V);
				putchr(',');
				print_int32(adc_result.max_V);
			} else if(current_function == FUNC_SCOPE_I) {
				if(adc_wanted_range) putchr('R');
				else                 putchr('A'); // auto range
				print_int32(adc_result.range);
				putchr(',');
				print_int32(adc_result.min_I);
				putchr(',');
				print_int32(adc_result.max_I);
				if(buttons_y < 0) {
					adc_wanted_range = adc_wanted_range ? adc_wanted_range-1 : 3;
				}
				if(buttons_y > 0) {
					adc_wanted_range = adc_wanted_range==3 ? 0 : adc_wanted_range+1;
				}
				buttons_y = 0;
			}
			putchr('\n');

			print_scope_glcd();

			oscope_full = 0;
		}
	}
#endif
#if USE_GLCD
	else if(current_function == FUNC_CHART) {
		if(got_adc || changed_mode) {
			cursor_to_top();
			print_decimal(rms.P, 2);
			printP("W\n");

			print_stripchart_glcd();
		}
	}
#endif

	wdt_reset();
}

int main(void) {
	uint8_t init_mcusr = MCUSR;
	MCUSR = 0;
	wdt_enable(WDTO_2S);

	io_init();
	output_device = OUT_UART;
	printP("Inited IO\n");

	timer_init();
	printP("Inited timer\n");

	sei();
	printP("Enabled interrupts\n");

#if USE_GLCD
	// wait a bit, just in case it helps the
	// GLCD to warm up
	for(uint8_t i=10; i; i--) {
		_delay_loop_2(65535);
		wdt_reset();
	}
	enable_glcd();
	for(uint8_t i=10; i; i--) {
		_delay_loop_2(65535);
		wdt_reset();
	}
	GLCD_Initalize();
	for(uint8_t i=10; i; i--) {
		_delay_loop_2(65535);
		wdt_reset();
	}
	release_glcd();
	output_device = OUT_GLCD;
	clear_screen();
	printP("Inited GLCD\n");
#endif

	print_keyP_val32("Reset source was ", init_mcusr);

	init_stackcheck();
	printP("Inited stack check\n");

	read_calibration_from_eeprom();
	init_rms();
	printP("Inited analog\n");

	// give the user time to read startup messages
	for(uint8_t i=10; i; i--) {
		printP("Wait ");
		print_int32(i);
		putchr(' ');
		putchr('\r');
		_delay_loop_2(65535);
		wdt_reset();
	}
	printP("Begin.         \n");

	for(;;) {
		#if USE_UART
		if(peekchr()) {
			output_device = OUT_UART;
			char c = getchr();
			if(c == 0) { } // syntactical trick
			#if USE_OSCOPE
			else if(c == 'v') {
				printP("Scope will show voltage.\n");
				oscope_channel = 0;
			} else if(c == 'i') {
				printP("Scope will show current.\n");
				oscope_channel = 1;
			}
			#endif
			#if USE_CALIBRATE
			else {
				do_calibration();
			}
			#endif
			#if USE_GLCD
			output_device = OUT_GLCD;
			#endif
		}
		#endif
		main_loop();
	}
}
