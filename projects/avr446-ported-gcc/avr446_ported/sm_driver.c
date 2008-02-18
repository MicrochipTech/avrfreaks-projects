/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Stepper motor driver.
 *
 * Stepper motor driver, increment/decrement the position and outputs the
 * correct signals to stepper motor.
 *
 * - File:               sm_driver.c
 * - Compiler:           AVR GCC
 * - Supported devices:  All devices with a 16 bit timer can be used.
 *                       The example is written for ATmega8535
 * - AppNote:            AVR446 - Linear speed control of stepper motor
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support email: avr@atmel.com
 *
 * $Name: RELEASE_1_0 $
 * $Revision: 1.2 $
 * $RCSfile: sm_driver.c,v $
 * $Date: 2008/02/18  $
 *****************************************************************************/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "global.h"
#include "sm_driver.h"


// Bit position for data in step table
#define BIT_A1 3
#define BIT_A2 2
#define BIT_B1 1
#define BIT_B2 0

//! Table with control signals for stepper motor
const unsigned char steptab[] PROGMEM = 
								  {((1<<BIT_A1) | (0<<BIT_A2) | (0<<BIT_B1) | (0<<BIT_B2)),
                                   ((1<<BIT_A1) | (0<<BIT_A2) | (1<<BIT_B1) | (0<<BIT_B2)),
                                   ((0<<BIT_A1) | (0<<BIT_A2) | (1<<BIT_B1) | (0<<BIT_B2)),
                                   ((0<<BIT_A1) | (1<<BIT_A2) | (1<<BIT_B1) | (0<<BIT_B2)),
                                   ((0<<BIT_A1) | (1<<BIT_A2) | (0<<BIT_B1) | (0<<BIT_B2)),
                                   ((0<<BIT_A1) | (1<<BIT_A2) | (0<<BIT_B1) | (1<<BIT_B2)),
                                   ((0<<BIT_A1) | (0<<BIT_A2) | (0<<BIT_B1) | (1<<BIT_B2)),
                                   ((1<<BIT_A1) | (0<<BIT_A2) | (0<<BIT_B1) | (1<<BIT_B2))};

//! Position of stepper motor (relative to starting position as zero)
int stepPosition = 0;

/*! \brief Init of io-pins for stepper motor.
 */
void sm_driver_Init_IO(void)
{
  // Init of IO pins
  SM_PORT &= ~((1<<A1) | (1<<A2) | (1<<B1) | (1<<B2)); // Set output pin registers to zero
  SM_DRIVE |= ((1<<A1) | (1<<A2) | (1<<B1) | (1<<B2)); // Set output pin direction registers to output
}

/*! \brief Move the stepper motor one step.
 *
 *  Makes the stepcounter inc/dec one value and outputs this to the
 *  steppermotor.
 *  This function works like a stepper motor controller, a call to the function
 *  is the stepping pulse, and parameter 'inc' is the direction signal.
 *
 *  \param inc  Direction to move.
 *  \return  Stepcounter value.
 */
unsigned char sm_driver_StepCounter(signed char inc)
{
  // Counts 0-1-...-6-7 in halfstep, 0-2-4-6 in fullstep
  static unsigned char counter = 0;
  // Update
  if(inc == CCW){
    stepPosition--;
  }
  else{
    stepPosition++;
  }

#ifdef HALFSTEPS
  if(inc){
    counter++;
  }
  else{
    counter--;
  }
#else
  if(inc){
    counter += 2;
  }
  else{
    counter -= 2;
  }
#endif

  // Stay within the steptab
  counter &= 0x07;
  sm_driver_StepOutput(counter);
  return(counter);
}

/*! \brief Convert the stepcounter value to signals for the stepper motor.
 *
 *  Uses the stepcounter value as index in steptab to get correct
 *  steppermotor control signals.
 *  Converts these signals to work with the stepper driver hardware.
 *
 *  \param pos  Stepcounter value.
 */
void sm_driver_StepOutput(unsigned char pos)
{
  unsigned char temp = (pgm_read_byte(&steptab[pos]));

  /*
  // Output bit by bit
  if(temp&(1<<BIT_A1))
    SM_PORT |= (1<<A1);
  else
    SM_PORT &= ~(1<<A1);

  if(temp&(1<<BIT_A2))
    SM_PORT |= (1<<A2);
  else
    SM_PORT &= ~(1<<A2);

  if(temp&(1<<BIT_B1))
    SM_PORT |= (1<<B1);
  else
    SM_PORT &= ~(1<<B1);

  if(temp&(1<<BIT_B2))
    SM_PORT |= (1<<B2);
  else
    SM_PORT &= ~(1<<B2);
  */

  // Output the fast way
  SM_PORT |= ((temp<<4)&0xF0);
  SM_PORT &= ((temp<<4)|0x0F);
}
