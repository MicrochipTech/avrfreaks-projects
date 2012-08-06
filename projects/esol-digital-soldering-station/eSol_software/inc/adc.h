/*
 * adc.h
 *
 *  Created on: Jun 12, 2011
 *      Author: dani
 */

#ifndef ADC_H_
#define ADC_H_

#define ADC_MAX_VAL        (1024ul)   /* in LSBs: 2^10 */
#define ADC_HALF           (  512l)   /* in LSBs: 2^9 */

typedef enum {
  enAdc01Ch,
  enAdc3Ch,
  enMaxNoAdcCh
} teAdcChId;

extern void ADC_vInit(void);
extern uint16_t ADC_u16GetSample(void);
extern uint16_t ADC_u16ConvertTempToLsb(uint16_t u16Input);
uint16_t ADC_vGetMovingAvg(void);
uint16_t ADC_vGetAvg(void);

#endif /* ADC_H_ */
