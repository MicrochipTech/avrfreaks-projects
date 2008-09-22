#include "config.h"

#define ADC_BOUND_MIN 10
#define ADC_BOUND_MAX (2046-10)
#define NUM_I_CHANNELS 3

// Note: ADC clock must be slow enough so that we have
// time do do all of the math for each cycle.
#define ADC_CYC_PER_CONV 13L
#define ADC_PRESCALAR 64L
#define ADC_PRESCALAR_BITS (_BV(ADPS2) | _BV(ADPS1))
#define ADC_CONV_PER_SEC (F_CPU / ADC_PRESCALAR / ADC_CYC_PER_CONV)
#define ADC_CONV_PER_PERIOD ((ADC_CONV_PER_SEC/1L + ADC_CLOCK_ERROR) / 2)

typedef struct {
	// these vars are sorted by alignment length
	// to avoid padding, which is an issue for
	// disk_reader.c which is compiled to run on
	// an actual computer (ie. 32-bit or 64-bit
	// machine)
	int64_t sum_P, sum_I2, sum_V2;
	uint32_t nsamps;
	int32_t sum_I, sum_V;
	uint16_t min_I, max_I, min_V, max_V;
	uint8_t is_aborted, range;
} power_t;

typedef struct {
	int32_t P;
	int16_t I, V;
} rms_t;

typedef struct {
	int16_t A[NUM_I_CHANNELS+1];
	int16_t B[NUM_I_CHANNELS+1];
} calib_t;

extern volatile power_t adc_result;
extern volatile uint8_t adc_result_ready;
extern volatile uint8_t adc_wanted_range;
extern calib_t calib;
#if USE_OSCOPE
extern volatile uint8_t oscope_channel;
#endif

void init_rms();
void read_calibration_from_eeprom();
rms_t compute_rms();
void rms_to_decimal(rms_t *rms, uint8_t range);
