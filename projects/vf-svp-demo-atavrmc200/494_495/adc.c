/**
* @file adc.c
*
* Copyright (c) 2004 Atmel.
*
* @brief Ce fichier permet de
*
* This file is included by all source files in order to access to system wide
* configuration.
* @version 1.0 (CVS revision : 1.3)
* @date 2006/02/13 12:31:05 (created on 06/04/2004)
* @author raubree (created by Emmanuel David)

*****************************************************************************/

#include <avr/io.h>
#include "lib_mcu/adc/adc_drv.h"

   	
/*****************************************************************************/
/*                       ADC Initialization                                  */
/*****************************************************************************/
/*!
 * This function initializes the ADC :
   *    - enable and run the ADC,
   *    - the end of conversion generates an interruption
   *    - 10 bit resolution
 */

void ADC_Init(void) {
    Enable_adc();
    //Left_adjust_adc_result();          // use  8 bits resolution
    Right_adjust_adc_result() ;          // use 10 bits resolution
    Enable_vcc_vref();
    Enable_adc_it();
//    Select_adc_channel(0<<MUX3 | 1<<MUX2 | 0<<MUX1 | 1<<MUX0);/* 0-10V Input */
    Select_adc_channel(1<<MUX3 | 0<<MUX2 | 1<<MUX1 | 0<<MUX0);/* pot */
    Set_prescaler( 0<<ADPS2 | 0<<ADPS1 | 0<<ADPS0);
    //Clear_adc_mux();
    //ADMUX |= 0x00;
    ADCSRB = 0x00;                       // free running mode
}

void ADC_start_conv(void) {
     Start_conv();
}
