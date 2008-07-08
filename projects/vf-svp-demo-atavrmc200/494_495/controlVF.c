/**
* @file controlVF.c
*
* Copyright (c) 2004 Atmel.
*
* @brief Ce fichier permet de
*
* This file is included by all source files in order to access to system wide
* configuration.
* @version 1.0 (CVS revision : 1.4)
* @date 2006/02/13 12:31:38 (created on 06/04/2004)
* @author raubree (created by Emmanuel David)

*****************************************************************************/

#include <avr/io.h>

#define Vf_SLOPE 67
#define OMEGA_TS_MAX 192 // 192
//#define OMEGA_TS_MIN 32 // 32
#define OMEGA_TS_MIN 64 // 64


/*
   w_max.Te_scal = (480/2*pi) * Te * w_max = 12
     Te = 1/1000
     w_max = 1500 .(pi/30) = 50.pi

    => w_max = 16 * w_max.Te_scal = 192

*/

// V/f law
uint16_t controlVF(uint16_t wTs)
{
	if (wTs <= OMEGA_TS_MIN )	// boost frequency
		wTs = OMEGA_TS_MIN;	// boost voltage
	else if (wTs > OMEGA_TS_MAX)
		wTs = OMEGA_TS_MAX;	// rated value

	return (Vf_SLOPE * wTs) / 10;	// V/f law
}
