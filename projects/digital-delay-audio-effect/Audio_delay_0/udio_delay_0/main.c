/*
 Written by Ozhan KD (www.knowledgeplus.ir)
 Based on ATXMEGA64A3
 VREF = REFB (PB0) , Input = PB1 , Output=PB2 ,
*/
#include "avr_compiler.h"
#include "adc_driver.h"
#include "dac_driver.h"

volatile unsigned char adc_result;
volatile bool new_adc=false;

void main(void)
{
 unsigned int i=0;
 unsigned int delayed_adc;
 unsigned char adc_samples[4000];
 bool delay_f=false;
 ADC_CalibrationValues_Load(&ADCB);
 ADC_ConvMode_and_Resolution_Config(&ADCB, ADC_ConvMode_Unsigned, ADC_RESOLUTION_8BIT_gc);
 ADC_Prescaler_Config(&ADCB, ADC_PRESCALER_DIV256_gc);
 ADC_Reference_Config(&ADCB, ADC_REFSEL_AREFB_gc);
 ADC_Ch_InputMode_and_Gain_Config(&ADCB.CH0,ADC_CH_INPUTMODE_SINGLEENDED_gc,ADC_DRIVER_CH_GAIN_NONE);
 ADC_Ch_InputMux_Config(&ADCB.CH0, ADC_CH_MUXPOS_PIN1_gc, ADC_CH_MUXNEG_PIN1_gc);
 ADC_Ch_Interrupts_Config(&ADCB.CH0, ADC_CH_INTMODE_COMPLETE_gc, ADC_CH_INTLVL_LO_gc);
 ADC_Enable(&ADCB);
 ADC_Wait_8MHz(&ADCB);
 ADC_FreeRunning_Enable(&ADCB);
 DAC_SingleChannel_Enable( &DACB,DAC_REFSEL_AREFB_gc,false );
 PMIC.CTRL |= PMIC_LOLVLEX_bm;
 sei();

 while(1)
 {
  while(!new_adc);
  new_adc=false;
  if(!delay_f)
  {
   adc_samples[i]=adc_result;
   if(++i==4000)
   {
    delay_f=true;
    i=0;
   }
   DAC_Channel_Write( &DACB,adc_result<<4 , CH0 );
  }
  else
  {
  delayed_adc=(adc_samples[i]+adc_result);
  adc_samples[i]=delayed_adc>>1;
  DAC_Channel_Write( &DACB,delayed_adc<<3 , CH0 );
  if(++i==4000) i=0;
  }
 }
}

ISR(ADCB_CH0_vect)
{
 adc_result=ADCB_CH0RES;
 new_adc=true;
}
