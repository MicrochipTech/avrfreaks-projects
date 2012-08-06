/*
 * adc.h
 *
 *  Created on: Jun 12, 2011
 *      Author: dani
 */

#include <avr/io.h>
#include <stdlib.h>
#include "adc.h"

#define AVG_SIZE 128ul                  /* Must be 2^n multiple to work as circular buffer*/

static uint16_t ADC_u16Avg[AVG_SIZE];   /* Circular buffer of ADC samples*/

void ADC_vInit(void)
{
  /* AVCC external VREF, ADC0/1 differential inputs , gain 10x */
  ADMUX = (1<<REFS0) | (1<<MUX3) | (1<<MUX0);

  /* Enable ADC, select XTAL/32 prescaler, start one first conversion to initialize ADC */
  ADCSRA = (1<<ADEN) | (1<<ADSC) | (1<<ADPS2) | (1<<ADPS0);// | (1<<ADPS0);

  /* Disable digital input buffer for ADC Ch 0,1,2 */
  DIDR0  = (1<<ADC3D) | (1<<ADC1D) | (1<<ADC0D);

  /* VREF internal AVCC, ADC0/1 differential inputs*/
  ADMUX = (1<<REFS0) | (1<<MUX3) | (1<<MUX0);
}

/*uint16_t ADC_u16GetSampleFromCh(teAdcChId eAdcCh)
{
  uint16_t u16Sample;
  switch (eAdcCh)
  {
    case enAdc01Ch:
       VREF internal AVCC, ADC0/1 differential inputs
      ADMUX = (1 << REFS0) | (1 << MUX3) | (1 << MUX0);
      break;
    case enAdc3Ch:
       VREF internal AVCC, ADC2 single ended input
      ADMUX = (1 << REFS0) | (1 << MUX1) | (1 << MUX0);
      break;
    default:
      break;
  }
   Start conversion ADCSRA |= (1 << ADSC);

   Wait for End-of-conversion
  while (ADCSRA & (1 << ADSC))
    ;

  u16Sample = ADCW;

   Look for the sign bit
  if ((u16Sample & 0x0200) != 0)
  {
     Sign extend from 10bit 2'os complement to 16bit
    u16Sample |= 0xFC00;
  }

   Add some offset to match V_to_Temp conversion
  u16Sample += 550;

  return (u16Sample);
}*/

uint16_t ADC_u16GetSample(void)
{
  uint16_t u16Sample;

  /* Start conversion */
  ADCSRA |= (1<<ADSC);

  /* Wait for End-of-conversion */
  while (ADCSRA & (1<<ADSC));

  u16Sample = ADCW;

  /* Look for the sign bit */
  if ((u16Sample & 0x0200) != 0 )
  {
    /* Sign extend from 10bit 2'os complement to 16bit */
    u16Sample |= 0xFC00;
  }

  /* Add some offset to match V_to_Temp conversion */
  u16Sample += 570;

  return (u16Sample);
}

uint16_t ADC_u16ConvertTempToLsb(uint16_t u16Input)
{
  volatile uint32_t u32OutVal;

  u32OutVal = (uint32_t) u16Input;
  u32OutVal *= 110ul;
  u32OutVal /= 100ul;

  return ((uint16_t)u32OutVal);

}

/* Moving average filter */
uint16_t ADC_vGetMovingAvg(void)
{ static uint8_t u8Index = 0;
  static uint32_t u32Sum = 0;
  uint16_t u16Avg;

  u32Sum -= ADC_u16Avg[u8Index];                    /* subtract the oldest sample */
  ADC_u16Avg[u8Index] = ADC_u16GetSample();
  u32Sum += ADC_u16Avg[u8Index];                    /* add  the newest sample */
  u8Index ++;

  u8Index &= (AVG_SIZE -1);                         /* make it circular*/

  /* Calculate moving average, use over sampling & decimation to increase ADC resolution (hence /4) */
  u16Avg = (uint16_t) (u32Sum / (AVG_SIZE / 4));

  return (u16Avg);
}

/* Increase resolution by 2 bits */
/*
uint16_t ADC_vGetAvg(void)
{ static uint8_t u8Index ;
  static uint32_t u32Sum;

  u32Sum = 0;

  for (u8Index = 0; u8Index < 16; u8Index++)
  {
    u32Sum += ADC_u16GetSample();
  }

  return ((uint16_t) (u32Sum / 16));
}
*/
