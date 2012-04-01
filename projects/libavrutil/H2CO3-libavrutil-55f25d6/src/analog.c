/*
 * analog.c
 * libavrutil
 *
 * Created by Árpád Goretity on 11/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include <avr/interrupt.h>
#include "analog.h"


void avr_pwm_init()
{
	/*
	 * Prepare the internal timer(s) for producing PWM signal
	 *
	 * PWM signal can only be generated on hardware-supported outputs.
	 * All available timers will be set to produce phase-correct
	 * PWM output signal.
	 */
	 
	avr_interrupt_disable();

	/* Clear timer 1 bits, in case it was used before by avr_sound_*() */
#if defined(TCCR1A)
	TCCR1A = 0;
#endif /* TCCR1A */

#if defined(TCCR1B)
	TCCR1B = 0;
#endif /* TCCR1B */

#if defined(TCCR0A) && defined(WGM01)
	/* Set timer 0 to phase-correct PWM mode */
	TCCR0A |= _BV(WGM00);
#endif  

	/* Set timer 0 prescale factor to 64 */
#if defined(__AVR_ATmega128__)
	/* MCU-specific: different values for the ATmega128 */
	TCCR0 |= _BV(CS02);
#elif defined(TCCR0) && defined(CS01) && defined(CS00)
	/* This combination is for the standard ATmega8 */
	TCCR0 |= _BV(CS01) | _BV(CS00);
#elif defined(TCCR0B) && defined(CS01) && defined(CS00)
	/* This combination is for the standard ATmega168/328/1280/2560 */
	TCCR0B |= _BV(CS01) | _BV(CS00);
#elif defined(TCCR0A) && defined(CS01) && defined(CS00)
	/* This combination is for the __AVR_ATmega645__ series */
	TCCR0A |= _BV(CS01) | _BV(CS00);
#else
#error Timer 0 prescale factor 64 could not be set
#endif


	/* set timer 1 prescale factor to 64 */
#if defined(TCCR1B) && defined(CS11) && defined(CS10)
	TCCR1B |= _BV(CS11) | _BV(CS11);
#elif defined(TCCR1) && defined(CS11) && defined(CS10)
	TCCR1 |= _BV(CS11) | _BV(CS11);
#endif
	/* put timer 1 in 8-bit phase correct pwm mode */
#if defined(TCCR1A) && defined(WGM10)
	TCCR1A |= _BV(WGM10);
#elif defined(TCCR1)
	TCCR1 |= _BV(WGM10)
#endif

	/* set timer 2 prescale factor to 64 */
#if defined(TCCR2) && defined(CS22)
	TCCR2 |= _BV(CS22);
#elif defined(TCCR2B) && defined(CS22)
	TCCR2B |= _BV(CS22);
#else
#error Timer 2 setup incomplete (may not be present on this MCU)
#endif
	/* configure timer 2 for phase correct pwm (8-bit)  */
#if defined(TCCR2) && defined(WGM20)
	TCCR2 |= _BV(WGM20);
#elif defined(TCCR2A) && defined(WGM20)
	TCCR2A |= _BV(WGM20);
#else
#error Timer 2 setup incomplete (may not be present on this MCU)
#endif

#if defined(TCCR3B) && defined(CS31) && defined(WGM30)
	/* Set timer 3 prescale factor to 64 */
	TCCR3B |= _BV(CS31) | _BV(CS30);
	/* Put timer 3 into phase-correct PWM mode (8 bit) */
	TCCR3A |= _BV(WGM30);
#endif
	
#if defined(TCCR4B) && defined(CS41) && defined(WGM40)
	/* Set timer 4 prescale factor to 64 */
	TCCR4B |= _BV(CS41) | _BV(CS40);
	/* Put timer 4 into phase-correct PWM mode (8 bit) */
	TCCR4A |= _BV(WGM40);
#endif

#if defined(TCCR5B) && defined(CS51) && defined(WGM50)
	/* Set timer 5 prescale factor to 64 */
	TCCR5B |= _BV(CS51) | _BV(CS50);
	/* Put timer 5 into phase-correct PWM mode (8 bit) */
	TCCR5A |= _BV(WGM50);
#endif

	/* Finally, re-enable interrupts */
	avr_interrupt_enable();
}

void avr_analog_write(uint8_t timer, uint16_t val)
{
	uint8_t value = (val + 2) >> 2; /* +2 for rounding */

	switch (timer)
	{
		/* TODO: needs to be fixed on the ATmega8 */

#if defined(TCCR0) && defined(COM00) && !defined(__AVR_ATmega8__)
		case TIMER0A:
			/* Connect PWM to pin on timer 0 */
			TCCR0 |= _BV(COM00);
			/* Set PWM duty cycle */
			OCR0 = value;
			break;
#endif

#if defined(TCCR0A) && defined(COM0A1)
		case TIMER0A:
			/* Connect PWM to pin on timer 0, channel A */
			TCCR0A |= _BV(COM0A1);
			/* Set PWM duty cycle */
			OCR0A = value;
			break;
#endif

#if defined(TCCR0A) && defined(COM0B1)
		case TIMER0B:
			/* Connect PWM to pin on timer 0, channel B */
			TCCR0A |= _BV(COM0B1);
			/* Set PWM duty cycle */
			OCR0B = value;
			break;
#endif

#if defined(TCCR1A) && defined(COM1A1)
		case TIMER1A:
			/* Connect PWM to pin on timer 1, channel A */
			TCCR1A |= _BV(COM1A1);
			/* Set PWM duty cycle */
			OCR1A = value;
			break;
#endif

#if defined(TCCR1A) && defined(COM1B1)
		case TIMER1B:
			/* Connect PWM to pin on timer 1, channel B */
			TCCR1A |= _BV(COM1B1);
			/* Set PWM duty cycle */
			OCR1B = value;
			break;
#endif

#if defined(TCCR2) && defined(COM21)
		case TIMER2:
			/* Connect PWM to pin on timer 2 */
			TCCR2|= _BV(COM21);
			/* Set PWM duty cycle */
			OCR2 = value;
			break;
#endif

#if defined(TCCR2A) && defined(COM2A1)
		case TIMER2A:
			/* Connect PWM to pin on timer 2, channel A */
			TCCR2A |= _BV(COM2A1);
			/* Set PWM duty cycle */
			OCR2A = value;
			break;
#endif

#if defined(TCCR2A) && defined(COM2B1)
		case TIMER2B:
			/* Connect PWM to pin on timer 2, channel B */
			TCCR2A |= _BV(COM2B1);
			/* Set PWM duty cycle */
			OCR2B = value;
			break;
#endif

#if defined(TCCR3A) && defined(COM3A1)
		case TIMER3A:
			/* Connect pwm to pin on timer 3, channel A */
			TCCR3A |= _BV(COM3A1);
			/* Set PWM duty cycle */
			OCR3A = value;
			break;
#endif

#if defined(TCCR3A) && defined(COM3B1)
		case TIMER3B:
			/* Connect PWM to pin on timer 3, channel B */
			TCCR3A |= _BV(COM3B1);
			/* Set PWM duty cycle */
			OCR3B = value;
			break;
#endif

#if defined(TCCR3A) && defined(COM3C1)
		case TIMER3C:
			/* Connect PWM to pin on timer 3, channel C */
			TCCR3A |= _BV(COM3C1);
			/* Set PWM duty cycle */
			OCR3C = value;
			break;
#endif

#if defined(TCCR4A) && defined(COM4A1)
		case TIMER4A:
			/* Connect PWM to pin on timer 4, channel A */
			TCCR4A |= _BV(COM4A1);
			/* Set PWM duty cycle */
			OCR4A = value;
			break;
#endif

#if defined(TCCR4A) && defined(COM4B1)
		case TIMER4B:
			/* Connect PWM to pin on timer 4, channel B */
			TCCR4A |= _BV(COM4B1);
			/* Set PWM duty cycle */
			OCR4B = value;
			break;
#endif

#if defined(TCCR4A) && defined(COM4C1)
		case TIMER4C:
			/* Connect PWM to pin on timer 4, channel C */
			TCCR4A |= _BV(COM4C1);
			/* Set PWM duty cycle */
			OCR4C = value;
			break;
#endif

#if defined(TCCR5A) && defined(COM5A1)
		case TIMER5A:
			/* Connect pwm to pin on timer 5, channel A */
			TCCR5A |= _BV(COM5A1);
			/* Set PWM duty cycle */
			OCR5A = value;
			break;
#endif

#if defined(TCCR5A) && defined(COM5B1)
		case TIMER5B:
			/* Connect PWM to pin on timer 5, channel B */
			TCCR5A |= _BV(COM5B1);
			/* Set PWM duty cycle */
			OCR5B = value;
			break;
#endif

#if defined(TCCR5A) && defined(COM5C1)
		case TIMER5C:
			/* Connect PWM to pin on timer 5, channel C */
			TCCR5A |= _BV(COM5C1);
			/* Set PWM duty cycle */
			OCR5C = value;
			break;
#endif
			default:
			/* No such timer */
			break;
	}
}


void avr_pwm_stop(uint8_t timer)
{
	switch (timer)
	{
#if defined(TCCR1A) && defined(COM1A1)
		case TIMER1A:
			TCCR1A &= ~_BV(COM1A1);
			break;
#endif

#if defined(TCCR1A) && defined(COM1B1)
		case TIMER1B:
			TCCR1A &= ~_BV(COM1B1);
			break;
#endif
		
#if defined(TCCR2) && defined(COM21)
		case  TIMER2:
			TCCR2 &= ~_BV(COM21);
			break;
#endif
		
#if defined(TCCR0A) && defined(COM0A1)
		case  TIMER0A:
			TCCR0A &= ~_BV(COM0A1);
			break;
#endif
		
#if defined(TIMER0B) && defined(COM0B1)
		case  TIMER0B:
			TCCR0A &= ~_BV(COM0B1);
			break;
#endif

#if defined(TCCR2A) && defined(COM2A1)
		case  TIMER2A: 
			TCCR2A &= ~_BV(COM2A1);
			break;
#endif

#if defined(TCCR2A) && defined(COM2B1)
		case  TIMER2B:
			TCCR2A &= ~_BV(COM2B1);
			break;
#endif
		
#if defined(TCCR3A) && defined(COM3A1)
		case  TIMER3A:
			TCCR3A &= ~_BV(COM3A1);
			break;
#endif

#if defined(TCCR3A) && defined(COM3B1)
		case  TIMER3B: 
			TCCR3A &= ~_BV(COM3B1);
			break;
#endif

#if defined(TCCR3A) && defined(COM3C1)
		case  TIMER3C:
			TCCR3A &= ~_BV(COM3C1);
			break;
#endif

#if defined(TCCR4A) && defined(COM4A1)
		case  TIMER4A:
			TCCR4A &= ~_BV(COM4A1);
			break;
#endif

#if defined(TCCR4A) && defined(COM4B1)
		case  TIMER4B:
			TCCR4A &= ~_BV(COM4B1);
			break;
#endif

#if defined(TCCR4A) && defined(COM4C1)
		case  TIMER4C:
			TCCR4A &= ~_BV(COM4C1);
			break;
#endif

#if defined(TCCR5A)
		case  TIMER5A:
			TCCR5A &=  ~_BV(COM5A1);
			break;
		case  TIMER5B:
			TCCR5A &= ~_BV(COM5B1);
			break;
		case  TIMER5C: 
			TCCR5A &= ~_BV(COM5C1);
			break;
#endif
		default:
			/* Do nothing */
			break;
	}
}

#ifdef ADCSRA

static uint8_t __avr_analog_ref = AVR_ANALOG_REF_VCC;

void avr_adc_init()
{
	/*
	 * Initialize the ADC converter for analog input
	 *
 	 * Internal pull-ups interfere with the ADC. Disable all
	 * pull-ups on the port if it is used for ADC. Either
	 * writing 0x00 to the port register or setting it to output
	 * should be enough to disable pull-ups.
	 */
	PORTC = 0x00;
	DDRC = 0x00;
	
	/*
	 * Set up the prescaler according to the processor clock frequency
	 */

#if F_CPU >= 12800000ULL
	/* Use a prescale of 128 */
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
#elif F_CPU >= 6400000ULL
	/* Use a prescale of 64 */
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1);
#elif F_CPU >= 3200000ULL
	/* Use a prescale of 32 */
	ADCSRA |= _BV(ADPS2) | _BV(ADPS0);
#elif F_CPU >= 1600000ULL
	/* Use a prescale of 16 */
	ADCSRA |= _BV(ADPS2);
#elif F_CPU >= 800000ULL
	/* use a prescale of 8 */
	ADCSRA |= _BV(ADPS1) | _BV(ADPS0);
#else
	/* Use a prescale of 4 */
	ADCSRA |= _BV(ADPS1);
#endif

	/*
	 * Set the analog reference to Vcc
	 */
	ADMUX = __avr_analog_ref | 0x0f;

	/*
	 * Enable the ADC system and start a sample
	 */
	ADCSRA |= _BV(ADEN) | _BV(ADSC);
	
	/*
	 * Wait for the ADC to return the sample (indicated by clearing ADSC):
	 * the first conversion requires extra time for internal setup
	 */
	loop_until_bit_is_clear(ADCSRA, ADSC);
}

uint16_t avr_analog_read(uint8_t bit)
{

	/* Select the input channel */
	uint8_t ch = (ADMUX & 0xf0) | (bit & 0x0f);
	ADMUX = ch;
	
#if defined(ADCSRB) && defined(MUX5)
	/*
	 * The MUX5 bit of ADCSRB selects whether we are reading from
	 * channels 0 to 7 (MUX5 clear) or 8 to 15 (MUX5 set).
	 */
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((bit >> 3) & 0x01) << MUX5);
#endif
	
	/* Start the conversion */
	ADCSRA |= _BV(ADSC);
	/* Wait for the conversion to finish */
	loop_until_bit_is_clear(ADCSRA, ADSC);
	
	/* Read the low byte first */
	uint8_t res_low = ADCL;
	/* And the high byte second */
	uint8_t res_high = ADCH;
	
	/* Merge the two bytes into a word */
	return ((uint16_t)res_high << 8) | res_low;
}

void avr_analog_ref_set(uint8_t ref)
{
	__avr_analog_ref = ref;

	/*
	 * Actually set the reference voltage and set the multiplexer.
	 * MUX 0x0f samples the ground (0.0 V).
	 */
	ADMUX = __avr_analog_ref | 0x0f;
}

uint8_t avr_analog_ref_get()
{
	return __avr_analog_ref;
}

#endif /* ADCSRA */

