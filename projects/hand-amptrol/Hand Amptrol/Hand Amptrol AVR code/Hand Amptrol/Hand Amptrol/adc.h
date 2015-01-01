/*
 * adc.h
 *
 * Created: 8/30/2014 4:56:39 PM
 *  Author: Mike
 */ 


#ifndef ADC_H_
#define ADC_H_

#define ADC_GAIN_HILOW 0b01   //GSEL MUX0
#define ADC_MUX 0b1100        //MUX5:2
#define ADC_MUX_POL MUX1         //MUX1
#define ADC_BUFFER_POS_len 32
#define ADC_BUFFER_POS_bits 5

#define SET_SWITCH_LVL			10
#define SET_SWITCH_COUNT_LEN	2
#define SET_SWITCH_COUNT_LEN_2  254

extern int16_t ADC_VALUE;


void adc_setup(void);
void adc_read(void);
void adc(void);

#endif /* ADC_H_ */