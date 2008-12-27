#include <stdint.h>
#include <stdlib.h>

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

////////////////////// Configuration ////////////////////////////////////

// Send waveform data to serial port
#define UART_OSCOPE 1
// Send statistics to serial port
//#define UART_STATS 1

// Base frequency of filter.  If you want something outside of the 50Hz-60Hz
// range you may need to adjust the ADC sample rate.
#define LINE_FREQ 60.0

// Controls the selectivity of the filter.  A higher number gives a more
// precise (narrower bandwidth) filter but causes a slower response to changes
// in the base waveform.  The maximum value is 6.
#define RELAX_BITS 6
// How many cycles to wait between adjustments of the sampling rate
#define CYCLES_PER_ADJUST 1
// Magnitude of adjustments to sampling rate
#define MAJOR_ADJUST_AMOUNT 200
#define MINOR_ADJUST_AMOUNT 10
// The spacing used by the autocorrelation function (used for locking on to the
// target frequency)
#define CORR_DELTA 2

/////////////////////////////////////////////////////////////////////////

// These constants determine the sampling rate and must be adjusted carefully
// in order to not exhaust CPU or RAM resources
#define ADC_PRESCALAR_BITS (_BV(ADPS2) | _BV(ADPS0))
// this is limited by the amount of RAM available
#define SAMPLES_PER_CYCLE 400
#define INITIAL_ADCTIMER_RATE (uint32_t)(F_CPU / LINE_FREQ / SAMPLES_PER_CYCLE * 65536.0)
volatile uint32_t adctimer_rate = INITIAL_ADCTIMER_RATE;
volatile uint16_t adctimer_carry;

#define ADC_BITS 10
#define ADC_MAX ((1 << ADC_BITS)-1)
#define ADC_ZERO (1 << (ADC_BITS-1))
#define EXTRA_BITS (16-ADC_BITS)

#if RELAX_BITS < 0 || RELAX_BITS > EXTRA_BITS
#error RELAX_BITS out of range
#endif

// location in EEPROM to save calibration data
#define EE_ADCTIMER_RATE ((void*)2)

/////////////////////////////////////////////////////////////////////////

// The averaged base waveform
uint16_t waveform[SAMPLES_PER_CYCLE];
// Index of current location in waveform[]
volatile uint16_t wave_idx = 0;

// Autocorrelation accumulators
volatile uint32_t corr_mid=0;
volatile uint32_t corr_left=0, corr_right=0;

// Input and output signals
volatile uint16_t signal_in, signal_smoothed;
// Used for stats/oscilloscope
volatile uint8_t data_available, overrun;

// Some flags
volatile uint8_t cycle_complete;
volatile uint8_t clipped;

// The signal output
volatile uint8_t pwm_out;

// Current selectivity of the filter, ranges from 0 to RELAX_BITS
volatile uint8_t relax_bits;

ISR(ADC_vect) {
	uint16_t val = ADC;

	if(val < 20 || val > ADC_MAX-20) clipped = 1;

	if(data_available) overrun++;

	uint16_t *p = waveform + wave_idx;

	if(wave_idx >= CORR_DELTA && wave_idx < SAMPLES_PER_CYCLE-CORR_DELTA) {
		uint16_t val_real  = val >> (ADC_BITS-8);
		#define corr_accum(accum, delta) accum += val_real * (p[delta] >> 8);
		corr_accum(corr_mid, 0)
		corr_accum(corr_left, -CORR_DELTA)
		corr_accum(corr_right, CORR_DELTA)
	}

	uint16_t w = *p;
	w -= (w >> relax_bits);
	w += val << (EXTRA_BITS-relax_bits);
	*p = w;

	wave_idx++;
	if(wave_idx == SAMPLES_PER_CYCLE) {
		wave_idx = 0;
		cycle_complete = 1;
		if(relax_bits < RELAX_BITS) relax_bits++;
	}

	if(PIND & _BV(PD2)) {
		int16_t clean = ((int16_t)(signal_in - signal_smoothed) + 512) >> (ADC_BITS-8);
		if(clean < 0) clean = 0;
		if(clean > 255) clean = 255;
		pwm_out = clean;
	} else {
		pwm_out = signal_smoothed >> (ADC_BITS-8);
	}

	// this is done in TIMER0_OVF_vect
	//OCR0A = pwm_out;

	signal_in = val;
	signal_smoothed = (w >> EXTRA_BITS);

	data_available = 1;

	ADCSRA |= _BV(ADIE);
}

ISR(TIMER0_OVF_vect) {
	OCR0A = pwm_out;
}

ISR(TIMER1_COMPB_vect) {
	uint32_t val = adctimer_rate + adctimer_carry;
	adctimer_carry = (uint16_t)val;
	OCR1A = val >> 16;
	TIFR1 |= _BV(OCF1B);
}

#if UART_OSCOPE
static inline void send_val(uint8_t chan, uint16_t val) {
	uint8_t v = 0x80;
	if(overrun) v |= 0x40;
	overrun = 0;
	v |= chan << 3;
	v |= (val >> 7) & 0x07;

	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = v;
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = val & 0x7f;
}
#endif

#if UART_STATS
void putchr(char val) {
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = val;
}

void printstr(char *txt) {
	while(*txt) putchr(*txt++);
}

void print_hex8(uint8_t val) {
	const char *nib2hex = PSTR("0123456789abcdef");
	putchr(pgm_read_byte(nib2hex + (val >>  4)));
	putchr(pgm_read_byte(nib2hex + (val & 0xf)));
}

void print_hex16(uint16_t val) {
	print_hex8((uint8_t)(val >> 8));
	print_hex8((uint8_t)val);
}

void print_hex32(uint32_t val) {
	print_hex8((uint8_t)(val >> 24));
	print_hex8((uint8_t)(val >> 16));
	print_hex8((uint8_t)(val >> 8));
	print_hex8((uint8_t)val);
}
#endif

int main(void) {
	uint8_t till_adjust = 0;
	uint8_t serial_enabled = 0;

#if UART_OSCOPE || UART_STATS
	uint16_t cL=0, cR=0;
#endif

#if UART_STATS
	uint16_t num_cycles = 0;
#endif

	// drift indicator
	DDRB |= _BV(PB5) | _BV(PB4) | _BV(PB3) | _BV(PB2) | _BV(PB1);
	// pullup for configuration switches
	PORTD |= _BV(PD2) | _BV(PD3) | _BV(PD4);

	// wait for PD4 to settle
	_delay_ms(10);

	uint8_t calibration_mode = 0;
	if(PIND & _BV(PD4)) {
		eeprom_read_block((void*)&adctimer_rate, EE_ADCTIMER_RATE, sizeof(adctimer_rate));
	} else {
		adctimer_rate = INITIAL_ADCTIMER_RATE;
		calibration_mode = 1;
	}

	// initialize PWM
	DDRD |= _BV(PD6);
	TCCR0A = _BV(COM0A1) | _BV(COM0A0) | _BV(WGM01) | _BV(WGM00);
	TCCR0B = _BV(CS00);
	TIMSK0 = _BV(TOIE0);
	OCR0A = 0;

	// initialize ADC
	ADMUX = 0; // ADC0 input with external reference
	ADCSRB = 0;
	ADCSRA = _BV(ADEN) | _BV(ADIE) | _BV(ADATE) | _BV(ADSC) | ADC_PRESCALAR_BITS;
	DIDR0 = 0x3f; // disable digital input on these pins

	// initialize ADC timer
	TCCR1A = 0;
	TCCR1B = _BV(CS10) | _BV(WGM12);
	OCR1A = adctimer_rate >> 16;
	OCR1B = 0; // trigger ADC on counter reset
	TIMSK1 = _BV(OCIE1B);
	ADCSRB |= _BV(ADTS2) | _BV(ADTS0); // trigger ADC on OCR1B compare

#if UART_OSCOPE || UART_STATS
	UCSR0A = _BV(U2X0);
	// do this later
	//UCSR0B = _BV(TXEN0);
	UBRR0L = 0; // 2M baud
#endif

	uint16_t i;
	for(i=0; i<SAMPLES_PER_CYCLE; i++) waveform[i] = 0x8000;

	wdt_enable(WDTO_1S);

	sei();

	for (;;) {
		wdt_reset();

		while(!data_available);

		uint8_t did_cycle = 0;

		if(cycle_complete) {
			cycle_complete = 0;
			did_cycle++;

			if(++till_adjust == CYCLES_PER_ADJUST) {
				till_adjust = 0;

				PORTB &= ~(_BV(PB5) | _BV(PB4) | _BV(PB3) | _BV(PB2) | _BV(PB1));
				if(clipped) PORTB |= _BV(PB1);
				clipped = 0;

				int32_t delta = 0;
				if(corr_left > corr_mid) {
					PORTB |= _BV(PB5);
					delta = MAJOR_ADJUST_AMOUNT;
				} else if(corr_right > corr_mid) {
					PORTB |= _BV(PB2);
					delta = -MAJOR_ADJUST_AMOUNT;
				} else if(corr_left > corr_right) {
					PORTB |= _BV(PB4);
					delta = MINOR_ADJUST_AMOUNT;
				} else { // corr_left <= corr_right
					PORTB |= _BV(PB3);
					delta = -MINOR_ADJUST_AMOUNT;
				}

				adctimer_rate += delta;

#if UART_OSCOPE || UART_STATS
				cL = (corr_left -corr_mid) >> 8;
				cL += 512;
				cR = (corr_right-corr_mid) >> 8;
				cR += 512;
#endif

				corr_left = corr_right = 0;
				corr_mid = 0;
			}
		}

		#if UART_OSCOPE
		uint16_t v1 = signal_in;
		uint16_t v2 = pwm_out << (ADC_BITS-8);
		uint16_t idx = wave_idx;
		#endif

		data_available = 0;

		if(calibration_mode && (PIND & _BV(PD4))) {
			eeprom_write_block((void*)&adctimer_rate, EE_ADCTIMER_RATE, sizeof(adctimer_rate));
			calibration_mode = 0;
		}

		if(serial_enabled != (PIND & _BV(PD3))) {
			serial_enabled = (PIND & _BV(PD3));
			if(serial_enabled) {
				UCSR0B |= _BV(TXEN0);
			} else {
				UCSR0B &= ~_BV(TXEN0);
			}
		}
		
		if(serial_enabled) {
			#if UART_OSCOPE
			// we only send one trace per sample, to avoid overruns
			// (waiting for the UART slows down the code too much)
			if(idx & 1) send_val(0, v1);
			else        send_val(1, v2);

			// This sends status on a third scope trace, but it
			// slows down the code too much and causes overruns.
			//} else if(idx && idx < 256) {
			//	uint8_t i8 = idx;
			//		 if(i8 ==  4 || i8==28) send_val(3, cL);
			//	else if(i8 == 30 || i8==48) send_val(3, cR);
			//	else if(i8 == 50 || i8==118) send_val(3, 512);
			//	else if(i8 == 120 || i8==148) send_val(3, (adctimer_rate >> 10) & 1023);
			//	else if(i8 == 150 || i8==2) send_val(3, 512);
			//}
			#endif

			#if UART_STATS
			if(did_cycle) {
				//print_hex32(cL);
				//putchr(' ');
				//print_hex32(cR);
				//putchr(' ');
	//			print_hex32(adctimer_rate);
				putchr(' '); print_hex8(overrun); overrun = 0;
				putchr(' '); print_hex16(num_cycles++);
				putchr('\r');
				putchr('\n');
			}
			#endif
		}
	}
}
