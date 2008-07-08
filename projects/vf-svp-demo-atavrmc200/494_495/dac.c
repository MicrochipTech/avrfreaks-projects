/**
* @file dac.c
*
* Copyright (c) 2004 Atmel.
*
* @brief Ce fichier permet de
*
* This file is included by all source files in order to access to system wide
* configuration.
* @version 1.0 (CVS revision : 1.1.1.1)
* @date 2005/09/21 15:08:53 (created on 06/04/2004)
* @author jberthy (created by Emmanuel David)

*****************************************************************************/

#include <avr/io.h>

#include "lib_mcu/dac/Dac_drv.h"

/****************************************************************************/
/*                       DAC Initialization                                 */
/****************************************************************************/
 /*!
 * This function initializes the DAC :
   *    - enable and run the DAC (DAC_init),
   *    - the DAC output is only used during debug
   *    - 10 bit resolution
   */


/****************************************************************************/

void DAC_Init(void) {
    Enable_dac();
    Right_adjust_dac_result();              //  10 bits resolution
    Output_dac_enable();
}

/****************************************************************************/
