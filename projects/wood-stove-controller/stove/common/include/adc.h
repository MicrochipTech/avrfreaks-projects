/*
 * adc.h
 *
 * Created: 12/28/2012 12:49:15 PM
 *  Author: sasquach
 */ 


#ifndef ADC_H_
#define ADC_H_

#define ADC_CLOCK_SCALER ( _BV(ADPS2) | _BV(ADPS1) )
#define ADC_DELAY 500




#endif /* ADC_H_ */


extern uint8_t ADC_VALUE1;
extern uint8_t ADC_VALUE2;
extern uint8_t ADC_VALUE3;

void adc_init(void);

void adc(void);
