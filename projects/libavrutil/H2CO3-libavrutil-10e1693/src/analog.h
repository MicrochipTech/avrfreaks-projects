/*
 * analog.h
 * libavrutil
 *
 * Created by Árpád Goretity on 11/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

/*
 * Analog write (hardware PWM) and read (ADC) functions
 * Before using these functions, avr_analog_init()
 * should be called.
 */

#ifndef __AVRUTIL_ANALOG_H__
#define __AVRUTIL_ANALOG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <avr/io.h>
#include "base.h"
#include "interrupt.h"


/*
 * This function needs to be called before calls to
 * avr_analog_write().
 */
void avr_pwm_init();

/*
 * Generate a PWM value in the range 0...1023 on a specified timer/pin.
 * The given duty cycle is not bound checked against the valid range.
 */
void avr_analog_write(uint8_t timer, uint16_t val);

/*
 * Turn off the PWM signal on the specified timer/pin.
 */

void avr_pwm_stop(uint8_t timer);

#ifdef ADCSRA

/*
 * Analog reference voltage macros
 * These control the ADC to properly select
 * an external or internal reference voltage for conversions.
 */

#define AVR_ANALOG_REF_EXTERN 0
#define AVR_ANALOG_REF_VCC _BV(REFS0)
#define AVR_ANALOG_REF_INTERN (_BV(REFS0) | _BV(REFS1))

/*
 * This function must be called before any avr_analog_read()
 * function call.
 */
void avr_adc_init();

/*
 * Read an analog value from the specified pin
 * in the range 0...1023.
 */
uint16_t avr_analog_read(uint8_t bit);

/*
 * Set the reference type of the ADC
 * The AVR_ANALOG_REF_* macros should be used as the argument of this function.
 */
void avr_analog_ref_set(uint8_t ref);

/*
 * Get the analog reference setting
 * Compare the return value to one of the AVR_ANALOG_REF_* macros
 */
uint8_t avr_analog_ref_get();

#endif /* ADCSRA */

/*
 * Convenience function for applications using both
 * analog input and output
 */
__avr_inline void avr_analog_init();

void avr_analog_init()
{
	avr_pwm_init();
#ifdef ADCSRA
	avr_adc_init();
#endif /* ADCSRA */
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !__AVRUTIL_ANALOG_H__ */

