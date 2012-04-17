#ifndef __ADC_H__
#define __ADC_H__

#include <inttypes.h>

void adc_init(void);
uint16_t read_adc(uint8_t channel);

#endif
