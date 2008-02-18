/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Header file for sm_driver.c.
 *
 * - File:               sm_driver.h
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
 * $RCSfile: sm_driver.h,v $
 * $Date: 2008/02/18 $
 *****************************************************************************/

#ifndef SM_DRIVER_H
#define SM_DRIVER_H

// Direction of stepper motor movement
#define CW  0
#define CCW 1

/*! \Brief Define stepping mode to use in stepper motor.
 *
 * Either halfsteps (HALFSTEPS) or fullsteps (FULLSTEPS) are allowed.
 *
 */
#define HALFSTEPS
//#define FULLSTEPS

/*! \Brief Define IO port and pins
 *
 * Set the desired drive port and pins to support your device
 *
 */
#define SM_PORT         PORTB
#define SM_DRIVE        DDRB
#define A1    PB7 //!< Stepper motor winding A positive pole.
#define A2    PB6 //!< Stepper motor winding A negative pole.
#define B1    PB5 //!< Stepper motor winding B positive pole.
#define B2    PB4 //!< Stepper motor winding B negative pole.

void sm_driver_Init_IO(void);
unsigned char sm_driver_StepCounter(signed char inc);
void sm_driver_StepOutput(unsigned char pos);

//! Position of stepper motor.
extern int stepPosition;

#endif
