#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include "config.h"
#include "lcd_spi.h"
#include "ad7714.h"
#include "serial.h"
#include "rotary_switch.h"

volatile uint32_t time_ctr = 0;
volatile uint16_t menu_show_delay=0;
volatile uint8_t misc_delay=0;

uint8_t menu_item=0, menu_visible=0;
#define MENU_ZERO 0
#define MENU_GAIN 1
#define MENU_CHAN 2
#define MENU_BUFFER 3
#define MENU_RATE 4
#define MENU_BACKLIGHT 5
#define NUM_MENU_ITEMS 6
ad7714params_t params;
uint8_t rate_idx = 0;
uint8_t buffer = 1;
uint32_t softzero = 0x800000;
uint8_t want_set_params=0;
uint8_t backlight = 0;

volatile uint32_t average_val=0x800000;

void my_delay(uint8_t ms) {
	misc_delay = ms;
	while(misc_delay) {
		wdt_reset();
		set_sleep_mode(SLEEP_MODE_IDLE);
		sleep_mode();
	}
}

ISR(TIMER1_COMPA_vect) {
	time_ctr++;
	rotary_bounce_tick();
	if(menu_show_delay) menu_show_delay--;
	if(misc_delay) misc_delay--;
}

static void ioinit(void) {
	TCCR1A = 0;
	TCCR1B = _BV(CS10) | _BV(CS12) | _BV(WGM12);
	OCR1A = 15; // almost a millisecond
	TIMSK1 = _BV(OCIE1A);

	init_serial();

	init_rotary();

	// PB5 = SCK, PB4 = MISO, PB3 = MOSI
	// PB2 = !SS is set to output in ad7714.c but must be set here in
	// order for SPI to work during LCD initialization.
	DDRB |= (_BV(PB5) | _BV(PB3) | _BV(PB2));
	DDRB &= ~(_BV(PB4));
	PORTB |= _BV(PB4); // prevent floating input
	// enable, master, clock=fosc/128
	//SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1) | _BV(SPR0);
	// enable, master, clock=fosc/16
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);

	sei();

	wdt_enable(WDTO_2S);
}

///////////////////

void printAdcVal(uint32_t val, uint8_t is_line2) {
	char s[40];

	if(is_line2) LcdLine2();
	else LcdLine1();

	if(!is_line2) {
		print_uint32(time_ctr);
	}

	double dval = ((double)((int32_t)val - (int32_t)softzero) / (double)0x800000) * 2.5;
	dval /= (double)(1 << params.gain_param);

	dtostrf(dval, 11, 8, s);
	if(!is_line2) {
		putchr(' ');
		printstr(s);
		putchr('\n');
	}
	LcdWriteStr(s);
	LcdWriteStr("   ");

	ltoa(val, s, 16);
	zeropad(s, 6);
	LcdWriteStr(s);
}

static void do_menu() {
	char s[25];

	if(menu_visible && !menu_show_delay) {
		printAdcVal(average_val, 1);
		menu_visible = 0;
	}

	int8_t rotary1, rotary2;
	get_rotary(&rotary1, &rotary2);
	if(!rotary1 && !rotary2) return;

	if(!menu_show_delay) rotary2 = 0;
	menu_show_delay = 3000;
	menu_visible = 1;

	if(rotary2) {
		if(rotary2 ==  1) {
			if(menu_item == NUM_MENU_ITEMS-1) menu_item = 0;
			else menu_item++;
		} else if(rotary2 == -1) {
			if(menu_item) menu_item--;
			else menu_item = NUM_MENU_ITEMS-1;
		}
		rotary1 = rotary2 = 0;
	}

	LcdLine2();

	if(menu_item == MENU_ZERO) {
		if(rotary1) {
			softzero = average_val;
			menu_show_delay = 0;
		}
		LcdWriteStr("Zero=");
		ltoa(softzero, s, 16);
		zeropad(s, 6);
		LcdWriteStr(s);
	} else if(menu_item == MENU_GAIN) {
		if(rotary1) {
			params.gain_param = (params.gain_param + rotary1) & 7;
			want_set_params = 1;
		}
		LcdWriteStr("Gain=");
		ltoa((1<<params.gain_param), s, 10);
		LcdWriteStr(s);
	} else if(menu_item == MENU_CHAN) {
		if(rotary1) {
			params.chan = (params.chan + rotary1) & 7;
			want_set_params = 1;
		}
		LcdWriteStr("Channel=");
		LcdWriteStr(ad7714chan_name(params.chan));
	} else if(menu_item == MENU_BUFFER) {
		if(rotary1) {
			buffer = !buffer;
			ad7714set_buffer(buffer);
			LcdClearLine1();
			LcdLine2(); 
		}
		LcdWriteStr("Buffer=");
		LcdWriteStr(buffer ? "On" : "Off");
	} else if(menu_item == MENU_RATE) {
		uint8_t rate_changed = 0;
		if(rotary1==-1 && rate_idx) {
			rate_idx--;
			rate_changed++;
		} else if(rotary1==1 && ad7714standard_rate(rate_idx+1)) {
			rate_idx++;
			rate_changed++;
		}
		if(rate_changed) {
			params.rate = ad7714standard_rate(rate_idx);
			want_set_params = 1;
		}
		LcdWriteStr("Rate=");
		double hz = ad7714rate_to_hertz(params.rate);
		dtostrf(hz, 5, 2, s);
		LcdWriteStr(s);
		LcdWriteStr("Hz");
	} else if(menu_item == MENU_BACKLIGHT) {
		if(rotary1) {
			backlight = !backlight;
			LcdBacklight(backlight);
		}
		LcdWriteStr("Backlight ");
		LcdWriteStr(backlight ? "On" : "Off");
	}
	LcdClearToEnd();
}

int main(void) {
	ioinit();

	printstr_p(PSTR("\nHello.\n"));

	LcdSetup();
	LcdBacklight(backlight);
	LcdClearLine2();
	LcdClearLine1();
	LcdWriteStr_p(PSTR("Initializing..."));

	params.gain_param = 0;
	params.chan = 4;
	params.rate = ad7714standard_rate(0);
	ad7714init();
	ad7714set_buffer(buffer);
	ad7714setup(params);

	uint16_t num_idle = 0;
	for(;;) {
		wdt_reset();

		if(ad7714have_capture()) {
			num_idle=0;

			if(ad7714accum_count() > 20) {
				average_val = ad7714pull_average();
				if(!menu_visible) printAdcVal(average_val, 1);
			}

			printAdcVal(ad7714get_capture(), 0);
		} else {
			num_idle++;
			// wake on timer tick or on DRDY interrupt
			set_sleep_mode(SLEEP_MODE_IDLE);
			sleep_mode();
		}

		/*
		if(chr_avail() && getchr() == 27) {
			captureEnable(0);
			spi_debug();
			captureEnable(1);
		}
		*/

		do_menu();

		if(want_set_params && ad7714can_set_params()) {
			ad7714setup(params);

			LcdClearLine1();
			want_set_params = 0;
			softzero = 0x800000;
		}
	}
}

/*
static uint8_t SPIverbose(uint8_t v) {
	uint8_t ret = SPIquiet(v);
	char s[15];
	printstr_p(PSTR("("));
	ltoa(v, s, 16); zeropad(s, 2); printstr(s);
	printstr_p(PSTR(","));
	ltoa(ret, s, 16); zeropad(s, 2); printstr(s);
	printstr_p(PSTR(") "));
//	printstr_p(PSTR(" / "));
//	ltoa(v, s, 2); zeropad(s, 8); printstr(s);
//	printstr_p(PSTR(" => "));
//	ltoa(ret, s, 2); zeropad(s, 8); printstr(s);
//	putchr('\n');

	return ret;
}

static uint8_t hex2nib(char c) {
	if(c >= '0' && c <= '9') return c-'0';
	if(c >= 'a' && c <= 'f') return c-'a'+10;
	if(c >= 'A' && c <= 'F') return c-'A'+10;
	return 0xff;
}

void spi_debug() {
	for(;;) {
		OUTER:
		printstr_p(PSTR("DebugSPI> "));

		int i;
		uint8_t v=0;
		for(i=0; i<2; i++) {
			v <<= 4;
			char c;
			uint8_t nib;
			do {
				c = getchr();
				if(c == 27) {
					return;
				} else if(c == 'q') {
					freeport();
					grabport();
					printstr_p(PSTR("Toggled CS\n"));
					goto OUTER;
				}
				nib = hex2nib(c);
			} while(nib == 0xff);
			putchr(c);
			v += nib;
		}
		putchr('\n');
		grabport();
		SPIverbose(v);
		freeport();
		putchr('\n');
	}
}
*/
