
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
#define SM_PORT         PORTD
#define SM_DRIVE        DDRD
#define A1    PD7 //!< Stepper motor winding A positive pole.
#define A2    PD6 //!< Stepper motor winding A negative pole.
#define B1    PD5 //!< Stepper motor winding B positive pole.
#define B2    PD4 //!< Stepper motor winding B negative pole.

void sm_driver_Init_IO(void);
unsigned char sm_driver_StepCounter(signed char inc);
void sm_driver_StepOutput(unsigned char pos);

//! Position of stepper motor.
extern int stepPosition;

#endif
