#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <avr/wdt.h>
#include <avr/pgmspace.h>

#include "config.h"
#include "stripchart.h"
#include "io_stuff.h"
#include "KS0108.h"
#include "serial.h"

#define CHART_LEN 128

static uint8_t scale = 0;
static uint8_t chart_buf[CHART_LEN];
static uint8_t chart_idx;

static void upscale() {
	for(uint8_t i=0; i<CHART_LEN; i++) {
		chart_buf[i] >>= 1;
	}
	scale++;
}

static void downscale() {
	for(uint8_t i=0; i<CHART_LEN; i++) {
		chart_buf[i] <<= 1;
	}
	scale--;
}

void push_stripchart(uint32_t val) {
	while(val >= (256L << scale)) {
		upscale();
	}
	chart_buf[chart_idx++] = val >> scale;
	if(chart_idx == CHART_LEN) chart_idx = 0;
}

#if USE_GLCD
void print_stripchart_glcd() {
	uint8_t max;
	max = chart_buf[0];
	for(uint8_t i=1; i<CHART_LEN; i++) {
		if(chart_buf[i] > max) max = chart_buf[i];
	}

	while(scale && max < 128) {
		downscale();
		max <<= 1;
	}

	#define nrows 7
	#define div ((nrows << 3) - 1)

	uint8_t thresh[9];
	for(uint8_t row=0; row<nrows; row++) {
		enable_glcd();
		GLCD_GoTo(0, row+(8-nrows));
		for(uint8_t i=0; i<8; i++) {
			thresh[i] = ((uint16_t)(max) * (uint16_t)(div - (row+1)*8 + i + 1)) / div;
		}
		for(uint8_t col=0; col<CHART_LEN; col++) {
			wdt_reset();
			uint8_t val = chart_buf[(col+chart_idx)%CHART_LEN];

			uint8_t byte = 0;
			for(uint8_t bit=0; bit<8; bit++) {
				byte <<= 1;
				if(thresh[bit] <= val) {
					byte++;
				}
			}
			GLCD_WriteData(byte);
		}
		release_glcd();
	}
	
	enable_glcd();
	GLCD_GoTo(0, 0);
	release_glcd();
}
#endif
