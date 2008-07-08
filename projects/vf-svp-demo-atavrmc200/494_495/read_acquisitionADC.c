/**
* @file space_vector_PWM.c
*
* Copyright (c) 2004 Atmel.
*
* @brief Ce fichier permet de
*
* This file is included by all source files in order to access to system wide
* configuration.
* @version 1.0 (CVS revision : 1.2)
* @date 2006/03/08 17:03:49 (created on 06/04/2004)
* @author raubree (created by Emmanuel David)

*****************************************************************************/

#include <avr/io.h>
#if 1
// speed measurement thanks to the ADC
int16_t read_acquisition(void)
{
    uint16_t     resultADC ;

    resultADC = ADC ;  // the ADC output has 10 bit resolution

    // convert from shifted code to 2's complement code
    // when the ADC output is higher than 512, the measure is positive
    // when the ADC output is lower  then 512, the measure is negative
//    if ( (resultADC.b[1] & 0x02) == 2) resultADC.b[1] &= 0x01 ;
//      else resultADC.b[1] |= 0xFE ;

    return (resultADC) ;
}
#endif
