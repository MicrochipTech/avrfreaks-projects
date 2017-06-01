/*
 * ADC.h
 *
 * Created: 5/26/2017 7:21:38 PM
 *  Author: Big Daddy
 */ 


#ifndef ADC_H_
#define ADC_H_

/* Interrupt Level 0 */
#define ADC_INTERRUPT_PRIORITY  0

/* Interrupt Group is not defined in IAR compiler's device part header file */
#ifndef AVR32_ADCIFA_IRQ_GROUP
#define AVR32_ADCIFA_IRQ_GROUP 36
#endif
#define ADC_CHANNELS	1
#define P8V192sns_INN         AVR32_ADCIFA_INN_ADCIN8
#define P8V192sns_INP         AVR32_ADCIFA_INP_GNDANA
#define P8V192sns_PIN         AVR32_ADCIN8_PIN
#define P8V192sns_FUNCTION    AVR32_ADCIN8_FUNCTION

// local or extern?
#ifdef _ADC_
#define GLOBAL
#define _I(x) x
	/* GPIO pin/adc-function map. */
	const gpio_map_t ADCIFA_GPIO_MAP ={{AVR32_ADCREF0_PIN, AVR32_ADCREF0_FUNCTION},{AVR32_ADCREFP_PIN, AVR32_ADCREFP_FUNCTION},{AVR32_ADCREFN_PIN, AVR32_ADCREFN_FUNCTION},{P8V192sns_PIN, P8V192sns_FUNCTION}};
#else // end #ifdef _ADC_
#define GLOBAL extern
#define _I(x)
	/* GPIO pin/adc-function map. */
//	extern const gpio_map_t ADCIFA_GPIO_MAP;
#endif // end #ifndef _ADC_

GLOBAL void ADCIFA_init(void);
// one ADC channel
GLOBAL	int16_t adc_values[ _I(ADC_CHANNELS) ];

// cleanup
#undef _I
#undef GLOBAL

#endif /* ADC_H_ */