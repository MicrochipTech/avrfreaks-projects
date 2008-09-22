#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <avr/wdt.h>

#include "config.h"
#include "oscope.h"
#include "io_stuff.h"
#include "KS0108.h"
#include "serial.h"

uint8_t oscope_buf[OSCOPE_LEN];
uint8_t oscope_full = 0;

#if USE_GLCD
void print_scope_glcd() {
	uint8_t min, max;
	min = max = oscope_buf[0];
	for(uint8_t i=1; i<OSCOPE_LEN; i++) {
		if(oscope_buf[i] < min) min = oscope_buf[i];
		if(oscope_buf[i] > max) max = oscope_buf[i];
	}

	#define nrows 7
	#define div ((nrows << 3) - 1)

	uint8_t thresh[9];
	uint8_t last_val = 0;
	for(uint8_t row=0; row<nrows; row++) {
		enable_glcd();
		GLCD_GoTo(0, row+(8-nrows));
		for(uint8_t i=0; i<9; i++) {
			thresh[i] = ((uint16_t)(max-min) * (uint16_t)(div - (row+1)*8 + i + 1)) / div;
		}
		for(uint8_t col=0; col<OSCOPE_LEN; col++) {
			wdt_reset();
			uint8_t val = oscope_buf[col] - min;
			if(!col) last_val = val;
			uint8_t top, bot;
			if(val > last_val) {
				//top = val; bot = last_val+1;
				top = val; bot = last_val;
			} else if(val < last_val) {
				//bot = val; top = last_val-1;
				bot = val; top = last_val;
			} else {
				top = bot = val;
			}

			uint8_t byte = 0;
			for(uint8_t bit=0; bit<8; bit++) {
				byte <<= 1;
				if(thresh[bit] <= top && thresh[bit+1] > bot) {
					byte++;
				}
			}
			GLCD_WriteData(byte);

			last_val = val;
		}
		release_glcd();
	}
	
	enable_glcd();
	GLCD_GoTo(0, 0);
	release_glcd();
}
#endif
