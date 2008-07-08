/**
* @file mc_control.c
*
* Copyright (c) 2004 Atmel.
*
* @brief This module provide services to control speed for AT90PWM3 Only
* Type of control : PID means proportionnal, integral and derivative.
*
* @version 1.0 (CVS revision : 1.3)
* @date 2006/02/13 12:32:54
* @author raubree
*****************************************************************************/

#include <avr/io.h>

/* Speed control variables */
int16_t speed_error=0;      //!<Error calculation
int16_t speed_integral = 0;
int16_t speed_integ = 0;
int16_t speed_proportional = 0;

/**************************************************************************************/
/*                          Speed Control                                             */
/**************************************************************************************/
/**
* @brief  speed controller
* @return value of speed (duty cycle on 16 bits)
*         speed_measure has 10 bits resolution
*/
int16_t mc_control_speed_16b(int16_t speed_ref , int16_t speed_measure)
{
  int16_t Duty = 0;
  int16_t increment = 0;

  // Error calculation
  speed_error = speed_ref - speed_measure ;

  // proportional term calculation : Kp= 7/64=0.1
  speed_proportional = ( speed_error/8 - speed_error/64 );

  // integral term calculation
  speed_integral = speed_integral + speed_error;

  // speed integral saturation
  if(speed_integral >  32000) speed_integral =  32000;
  if(speed_integral < -32000) speed_integral = -32000;

  // speed_integ = Ki_speed*speed_integral, with Ki_speed = 29/8192=3e-3
  speed_integ = (speed_integral - speed_integral/8 + speed_integral/32) / 256 ;

  // Duty Cycle calculation
  increment = speed_proportional + speed_integ;

  increment = (increment/2 + increment/4) ; // PI output normalization

  // saturation of the PI output
  if( increment > (int16_t)(0) ) {
    if  (increment <= (int16_t)(192)) Duty = (int16_t)increment ;
     else Duty = 192 ;
  }
  else {
    if  (increment < (int16_t)(-192)) Duty = -192 ;
     else Duty = (int16_t)increment ;
  }

  // return Duty Cycle
  return Duty;
}

