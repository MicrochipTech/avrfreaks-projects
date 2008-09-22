#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>

#include "config.h"
#include "serial.h"
#include "io_stuff.h"
#if USE_GLCD
#include "KS0108.h"
#endif

uint8_t output_device;

#if USE_GLCD
#define GLCD_ROWS 8
#define GLCD_COLS 21
uint8_t glcd_row, glcd_col;
#endif

#if USE_UART
static char rx;

uint8_t peekchr() {
	if(!rx && bit_is_set(UCSR0A, RXC0)) {
		rx = UDR0;
		if(bit_is_set(UCSR0A, FE0)) rx = 0;
	}
	return rx;
}

char getchr() {
	while(!rx) {
		peekchr();
		wdt_reset();
	}
	char c = rx;
	rx = 0;
	return c;
}
#else
uint8_t peekchr() { return 0; }
char getchr() { return 0; }
#endif

void putchr(char c) {
#if USE_GLCD
	if(output_device == OUT_GLCD) {
		if(glcd_row >= GLCD_ROWS) return;
		enable_glcd();
		if(c == '\n') {
			#if GLCD_OVERPAINT
			GLCD_ClearToRowEnd();
			#endif
			glcd_row++;
			if(glcd_row >= GLCD_ROWS) return;
			GLCD_GoTo(0, glcd_row);
			glcd_col = 0;
		} else if(c == '\r') {
			GLCD_GoTo(0, glcd_row);
			glcd_col = 0;
		} else {
			if(glcd_col == GLCD_COLS) {
				glcd_col = 0;
				glcd_row++;
				if(glcd_row >= GLCD_ROWS) return;
				GLCD_GoTo(0, glcd_row);
			}
			GLCD_WriteChar(c);
			// less than 32 are control chars which are
			// not drawn to screen
			if(c < 32) glcd_col++;
		}
		release_glcd();
	}
#endif
#if USE_UART
	if(output_device == OUT_UART) {
		if(c == '\n') putchr('\r');
		loop_until_bit_is_set(UCSR0A, UDRE0);
		UDR0 = c;
	}
#endif
}

void clear_screen() {
#if USE_GLCD
	if(output_device == OUT_GLCD) {
		enable_glcd();
		GLCD_ClearScreen();
		glcd_row = glcd_col = 0;
		GLCD_GoTo(0, 0);
		release_glcd();
	}
#endif
#if USE_UART
	if(output_device == OUT_UART) {
		printP("\e[H\e[2J");
	}
#endif
}

#if GLCD_OVERPAINT && USE_GLCD
void cursor_to_top() {
	enable_glcd();
	glcd_row = glcd_col = 0;
	GLCD_GoTo(0, 0);
	release_glcd();
}

void clear_to_screen_end() {
	while(glcd_row < GLCD_ROWS)
		putchr('\n');
	cursor_to_top();
}
#endif

void printstr(const char *s) {
	while(*s) putchr(*s++);
}

void printstr_p(const char *s) {
	char c;
	while((c = pgm_read_byte(s++))) putchr(c);
}

void print_int32(int32_t val) {
	char s[11];
	ltoa(val, s, 10);
	printstr(s);
}

void print_keyp_val32(const char *key, int32_t val) {
	printstr_p(key);
	char s[11];
	ltoa(val, s, 10);
	printstr(s);
	putchr('\n');
}

void print_decimal(int32_t val, uint8_t dp) {
	char s[12];
	ltoa(val, s, 10);
	if(val < 0) {
		while(strlen(s) < dp+2) {
			uint8_t l = strlen(s);
			for(int i=l; i>=1; i--) s[i+1] = s[i];
			s[1] = '0';
		}
	} else {
		while(strlen(s) < dp+1) {
			uint8_t l = strlen(s);
			for(int i=l; i>=0; i--) s[i+1] = s[i];
			s[0] = '0';
		}
	}
	uint8_t l = strlen(s);
	s[l+1] = 0;
	for(int i=0; i<dp; i++)
		s[l-i] = s[l-i-1];
	s[l-dp] = '.';
	printstr(s);
}

void print_hex8(uint8_t val) {
	const char *nib2hex = PSTR("0123456789abcdef");
	putchr(pgm_read_byte(nib2hex + (val >>  4)));
	putchr(pgm_read_byte(nib2hex + (val & 0xf)));
}

int32_t read_int32() {
	uint8_t num_chars = 0;
	uint8_t sign = 0;
	int32_t accum = 0;
	for(;;) {
		char c = getchr();
		if(c >= '0' && c <= '9') {
			accum *= 10;
			accum += c - '0';
			putchr(c);
			num_chars++;
		} else if(c == '-' && !num_chars) {
			putchr('-');
			sign = 1;
			num_chars++;
		} else if(c == '\b' && num_chars) {
			accum /= 10;
			printP("\b \b");
			num_chars--;
			if(!num_chars) sign = 0;
		} else if(c == '\r' || c == '\n') {
			putchr('\n');
			return sign ? -accum : accum;
		} else { // FIXME
			printP("bad[");
			print_int32((uint8_t)c);
			printP("]\n");
		}
	}
}
