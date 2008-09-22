#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <avr/interrupt.h>
#include <avr/eeprom.h>

#define USE_ASM 1

#include "config.h"
#include "rms64.h"
#if USE_ASM
#include "mulacc.h"
#endif

#define CHAN_V 0

// This depends on how many samples.  Make sure there is no overflow.
#define SC1 27
#define NS_INV ((1L << SC1) / ADC_CONV_PER_PERIOD)

volatile power_t adc_result;
volatile uint8_t adc_result_ready;
// 0 for auto-range
volatile uint8_t adc_wanted_range = 0;

#if USE_OSCOPE
#include "oscope.h"
volatile uint8_t oscope_channel = 0;
#endif

calib_t calib;

ISR(ADC_vect) {
	// see http://osdir.com/ml/hardware.avr.libc.devel/2007-02/msg00083.html
	static uint8_t oldchan;
	uint8_t curchan = ADMUX & 0xf;

	static uint8_t last_I_valid;
	static uint16_t last_I, last_V;
	static power_t acc;
	static uint8_t CHAN_I = 1;

	uint16_t val = ADC;

	uint16_t I, V;
	if(oldchan == CHAN_V) {
#if USE_OSCOPE
		if(oscope_channel == 0) {
			// sample tail of sequence, to prevent overwriting
			// the buffer as it is being drawn
			int16_t ptr = acc.nsamps - (ADC_CONV_PER_PERIOD - (1<<OSCOPE_STRIDE)*OSCOPE_LEN);
			ptr >>= OSCOPE_STRIDE;
			if(ptr >= 0) {
				oscope_buf[ptr] = val >> 2;
				if(ptr == OSCOPE_LEN-1) oscope_full++;
			}
		}
#endif
		V = last_V + val;
		I = last_I << 1;
		last_V = val;
		oldchan = curchan;
		curchan = CHAN_I;
		if(!last_I_valid) goto bail;
	} else if(oldchan == CHAN_I) {
#if USE_OSCOPE
		if(oscope_channel == 1) {
			// sample tail of sequence, to prevent overwriting
			// the buffer as it is being drawn
			int16_t ptr = acc.nsamps - (ADC_CONV_PER_PERIOD - (1<<OSCOPE_STRIDE)*OSCOPE_LEN);
			ptr >>= OSCOPE_STRIDE;
			if(ptr >= 0) {
				oscope_buf[ptr] = val >> 2;
				if(ptr == OSCOPE_LEN-1) oscope_full++;
			}
		}
#endif
		if(last_I_valid) {
			I = last_I + val;
		} else {
			I = val << 1;
		}
		V = last_V << 1;
		last_I = val;
		last_I_valid = 1;
		oldchan = curchan;
		curchan = CHAN_V;
	} else {
		// out of sync...
		// this can happen after a range select
		oldchan = curchan;
		curchan = CHAN_V;
		goto bail;
	}

	acc.sum_I  += I;
	acc.sum_V  += V;
	#if USE_ASM
	mulacc_12_64(I, V, (uint64_t *)&acc.sum_P);
	sqracc_12_64(I, (uint64_t *)&acc.sum_I2);
	sqracc_12_64(V, (uint64_t *)&acc.sum_V2);
	#else
	acc.sum_P  += (uint32_t)I*(uint32_t)V;
	acc.sum_I2 += (uint32_t)I*(uint32_t)I;
	acc.sum_V2 += (uint32_t)V*(uint32_t)V;
	#endif

	if(acc.nsamps == 0) {
		acc.min_I = acc.max_I = I;
		acc.min_V = acc.max_V = V;
	} else {
		if(I < acc.min_I) acc.min_I = I;
		if(I > acc.max_I) acc.max_I = I;
		if(V < acc.min_V) acc.min_V = V;
		if(V > acc.max_V) acc.max_V = V;
	}

	acc.nsamps++;

	uint8_t new_range = 0;
	uint8_t oob = (acc.min_I < ADC_BOUND_MIN || acc.max_I > ADC_BOUND_MAX);
	if(!adc_wanted_range && oob) {
		new_range = CHAN_I - 1;
	} else if(adc_wanted_range && adc_wanted_range != CHAN_I) {
		new_range = adc_wanted_range;
	}

	uint8_t send_and_reset = 0;

	if(new_range) {
		CHAN_I = new_range;
		last_I_valid = 0;
		acc.is_aborted = 1;
		send_and_reset = 1;
	} else if(acc.nsamps == ADC_CONV_PER_PERIOD) {
		send_and_reset = 1;
	}

	if(send_and_reset) {
		adc_result = acc;
		adc_result_ready = 1;

		// FIXME - need to get these from calibration
		// (we would need information on the zero-offset values)
		uint8_t do_uprange = (
			!adc_wanted_range &&
			!acc.is_aborted && 
			acc.min_I > 1024-70 &&
			acc.max_I < 1024+70 &&
			CHAN_I < NUM_I_CHANNELS
		);

		acc.nsamps = 0;
		memset(&acc, 0, sizeof(power_t));
		if(do_uprange) CHAN_I++;
		acc.range = CHAN_I;
	}

	bail:
	// FIXME - do this earlier?
	ADMUX = (ADMUX & 0xf0) | curchan;
	ADCSRA |= _BV(ADIE);
}

void init_rms() {
	//ADMUX = _BV(REFS0) | _BV(REFS1);
	ADMUX = 0; // external Vref
	ADCSRB = 0;
	ADCSRA = _BV(ADEN) | _BV(ADIE) | _BV(ADATE) | _BV(ADSC) | ADC_PRESCALAR_BITS;
	// FIXME - set DIDR0
}

void read_calibration_from_eeprom() {
    eeprom_read_block(&calib, 0, sizeof(calib_t));
}

static uint16_t sqrt_32(int32_t x) {
	if(x <= 0) return 0;
	int32_t r = 1;
	for(;;) {
		int32_t r2 = (r + x/r) >> 1;
		// tolerance of one prevents infinite loop
		if(r2 == r || r2 == r+1) return (uint16_t)r2;
		else r = r2;
	}
}

int32_t crank(volatile int64_t *q, volatile int32_t *a, volatile int32_t *b, uint8_t do_root) {
	int64_t dif = (*q * ADC_CONV_PER_PERIOD - (int64_t)(*a) * (int64_t)(*b));
	// FIXME - right shift with sign?
	uint8_t sgn = dif < 0;
	if(sgn) dif = -dif;
	dif = (dif * NS_INV) >> SC1;
	dif = (dif * NS_INV * 100L) >> SC1;
	if(do_root) {
		return sqrt_32(dif);
	} else {
		return sgn ? -dif : dif;
	}
}

rms_t compute_rms() {
	rms_t rms;
	
	rms.P = crank(&adc_result.sum_P , &adc_result.sum_I, &adc_result.sum_V, 0);
	rms.I = crank(&adc_result.sum_I2, &adc_result.sum_I, &adc_result.sum_I, 1);
	rms.V = crank(&adc_result.sum_V2, &adc_result.sum_V, &adc_result.sum_V, 1);

	return rms;
}

void rms_to_decimal(rms_t *rms, uint8_t range) {
	rms->I = (int32_t)rms->I * calib.A[range] / calib.B[range];
	rms->V = (int32_t)rms->V * calib.A[0] / calib.B[0];
	// divide first to prevent overflow
	// multiply first to avoid roundoff
	if(rms->P < -500000 || rms->P > 500000) {
		rms->P = rms->P / calib.B[range] * calib.A[range]
			/ calib.B[0] * calib.A[0] / 10;
	} else if(rms->P < -10000 || rms->P > 10000) {
		rms->P = rms->P / 10 * calib.A[range] / calib.B[range] 
			* calib.A[0] / calib.B[0];
	} else {
		rms->P = rms->P * calib.A[range] / calib.B[range] 
			* calib.A[0] / calib.B[0] / 10;
	}
	// because of the inverting amplifier configuration:
	if(range == 2) rms->P = -rms->P;
}
