#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "config.h"
#include "ad7714.h"

// These are suitable for a clock rate of 2.4576MHz and
// a powerline frequency of 60Hz.
uint16_t ADC_RATES[] = { 4000, 3840, 1920, 1280, 960, 640, 320, 160, 80, 40, 20, 0 }; 

// FIXME - used but not changed in ISR - volatile needed?
static volatile ad7714params_t params;

static volatile uint32_t av_accum=0;
static volatile uint8_t av_count=0;
static volatile uint8_t num_captures=0;
static volatile uint32_t last_capture=0;
static volatile uint8_t can_set_params=0;

static void captureEnable(uint8_t state) {
#if ADC_USE_ISR
	if(state) EIMSK |=  _BV(INT0);
	else      EIMSK &= ~_BV(INT0);
#endif
}

static uint8_t SPI(uint8_t v) {
	ADC_CS_PORT &= ~_BV(ADC_CS_BIT); // clear !SS
	SPDR = v;
	while(!(SPSR & (1<<SPIF)));
	uint8_t ret = SPDR;
	ADC_CS_PORT |= _BV(ADC_CS_BIT); // set !SS
	return ret;
}

static void grabport() {
	SPCR |= (_BV(CPOL) | _BV(CPHA));
}

static void freeport() {
}

static void writereg(uint8_t reg, uint8_t chan, uint8_t len, uint32_t data) {
	uint8_t cr = (reg << 4) | chan;
	SPI(cr);
	while(len) {
		len--;
		SPI(data >> (len * 8));
	}
}

static uint32_t readreg(uint8_t reg, uint8_t chan, uint8_t len) {
	uint8_t cr = (reg << 4) | 0x8 | chan;
	SPI(cr);
	uint32_t data = 0;
	while(len) {
		len--;
		data <<= 8;
		data |= SPI(0);
	}
	return data;
}

static void reset_serial() {
	SPI(0xff); SPI(0xff); SPI(0xff); SPI(0xff);
}

void ad7714init() {
	captureEnable(0);
	ADC_CS_DDR |= _BV(ADC_CS_BIT);
#if ADC_USE_ISR
	EICRA &= ~(_BV(ISC00) | _BV(ISC01));
	DDRD &= ~_BV(PD2); // DRDY pin from ADC
#endif
#ifdef ADC_BUFFER_BIT
	ADC_BUFFER_DDR |= _BV(ADC_BUFFER_BIT);
#endif

	grabport();
	reset_serial();

// Calibrate all channels ahead of time:
// (this doesn't work so just call ad7714setup on demand)
//	uint8_t chan;
//	for(chan=0; chan<8; chan++) {
//		ad7714setup(chan, 1, 4000);
//	}

	freeport();
}

void ad7714setup(ad7714params_t new_params) {
	captureEnable(0);
	grabport();

	params = new_params;

	// limit bits for safety
	params.chan &= 7;
	params.gain_param &= 7;
	params.rate &= 0xfff;

	reset_serial();
	// gain = (2 ** gain_param), so gain_param>2 means a gain of more than 4
	uint8_t boost_bit = (params.gain_param > 2) ? 0x20 : 0x00;
	writereg(2, params.chan, 1, 0x40 | boost_bit | (params.rate >> 8)); // filter hi
	writereg(3, params.chan, 1, (params.rate & 0xff)); // filter lo
	writereg(1, params.chan, 1, 0x20 | (params.gain_param<<2));

	num_captures = 0;
	av_count = 0;
	// can't set params again till data comes in (don't know why)
	can_set_params = 0;

	freeport();
	captureEnable(1);
}

static uint8_t check_ready_reg() {
	grabport();
	uint8_t status = readreg(0, params.chan, 1);
	// not needed per spec, but it helps to fix comm errors
	while((status & 0xf) != (0x8 | params.chan)) {
		reset_serial();
		status = readreg(0, params.chan, 1);
	}
	uint8_t ready = !(status & 0x80);
	freeport();
	return ready;
}

static uint32_t read_data_reg() {
	grabport();
	uint32_t ret = readreg(5, params.chan, 3);
	freeport();
	return ret;
}

static void update_if_avail() {
	if(check_ready_reg()) {
		last_capture = read_data_reg();
		can_set_params = 1;
		if(num_captures < 255) num_captures++;

		if(av_count < 255) {
			av_accum += last_capture;
			av_count++;
		}
	}
}

#if ADC_USE_ISR
ISR(INT0_vect) {
	// This pin is always low in this interrupt handler
	//uint8_t ready = !(PIND & _BV(PD2));
	//if(ready)
	update_if_avail();
}
#endif

uint8_t ad7714have_capture() {
#if ADC_USE_ISR
	return num_captures;
#else
	return check_ready_reg();
#endif
}

uint32_t ad7714get_capture() {
	captureEnable(0);

#if !ADC_USE_ISR
	update_if_avail();
#endif

	uint32_t ret = last_capture;
	num_captures=0;

	captureEnable(1);
	return ret;
}

uint8_t ad7714accum_count() {
	return av_count;
}

uint32_t ad7714pull_average() {
	captureEnable(0);

	uint32_t ret = av_accum / av_count;
	av_count = 0;
	av_accum = 0;

	captureEnable(1);
	return ret;
}

#ifdef ADC_BUFFER_BIT
void ad7714set_buffer(uint8_t state) {
	if(state) ADC_BUFFER_PORT |=  _BV(ADC_BUFFER_BIT);
	else      ADC_BUFFER_PORT &= ~_BV(ADC_BUFFER_BIT);
}
#endif

uint8_t ad7714can_set_params() {
	return can_set_params;
}

char *ad7714chan_name(uint8_t chan) {
	switch(chan) {
		case 0: return "AIN1-AIN6";
		case 1: return "AIN2-AIN6";
		case 2: return "AIN3-AIN6";
		case 3: return "AIN4-AIN6";
		case 4: return "AIN1-AIN2";
		case 5: return "AIN3-AIN4";
		case 6: return "AIN5-AIN6";
		case 7: return "AIN6-AIN6";
		default: return "invalid";
	}
}

double ad7714rate_to_hertz(uint16_t rate) {
	return (double)(F_ADC / 128) / (double)rate;
}

uint16_t ad7714standard_rate(uint8_t idx) {
	return ADC_RATES[idx];
}
