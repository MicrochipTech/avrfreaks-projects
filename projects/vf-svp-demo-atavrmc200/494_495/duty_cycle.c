/**
* @file duty_cycle.c
*
* Copyright (c) 2004 Atmel.
*
* @brief Ce fichier permet de
*
* This file is included by all source files in order to access to system wide
* configuration.
* @version 1.0 (CVS revision : 1.3)
* @date 2006/02/02 16:30:28 (created on 06/04/2004)
* @author raubree (created by Emmanuel David)

*****************************************************************************/

#include <avr/io.h>

/* table_sin120.h : no third harmonic */
#include "table_sin120.h"

/* table_sin120b.h : with third harmonic */
//#include "table_sin120b.h"

#define   MAX_SIN       128 // if no third harmonic
//#define   MAX_SIN       110 // if third harmonic

#define   MAX_THETA     120 // pi/2
#define   MAX_THETAx2   240
#define   MAX_THETAx3   360
#define   MAX_THETAx4   480
#define	  MAX_PWM       2666

/****************************************************************************/
/*                       duty cycle                                         */
/****************************************************************************/
/*!
 * This function computes the PWM duty cycles :
   *    - use the sine table to compute sin(theta),
   *    - multiply by Vm (comming from the V/f law)
   *    - shift by MAX_PWM/2 to generate positive and negative voltages
   *    - only one quarter of the period is stored in the sine table
*/

uint16_t duty_cycle(uint16_t theta, uint16_t Vm)
{
  signed char   sinus ;
  int32_t           Vmxsin ;

   if (theta <= MAX_THETA)                     // x in [0;pi/2]
    sinus = tab_sin[theta];                    // sin(x)
   else
    {
      if (theta <= MAX_THETAx2)                // x in [pi/2;pi]
       sinus = tab_sin[MAX_THETAx2-theta];     // sin(pi-x)
      else
      {
        if (theta<=MAX_THETAx3)                // x in [pi;3*pi/2]
         sinus = - tab_sin[theta-MAX_THETAx2]; // sin(x-pi)
        else                                   // x in [3*pi/2;2*pi]
         sinus = - tab_sin[MAX_THETAx4-theta]; // sin(2*pi-x)
      }
    }
   Vmxsin = sinus;
   Vmxsin = (Vmxsin * Vm)/MAX_SIN ;
   return ((unsigned int) (MAX_PWM/2 + Vmxsin));
}
