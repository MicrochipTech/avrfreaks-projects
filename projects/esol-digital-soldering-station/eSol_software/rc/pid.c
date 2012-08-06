/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief General PID implementation for AVR.
 *
 * Discrete PID controller implementation. Set up by giving P/I/D terms
 * to Init_PID(), and uses a struct PID_DATA to store internal values.
 *
 * - File:               pid.c
 * - Compiler:           IAR EWAAVR 4.11A
 * - Supported devices:  All AVR devices can be used.
 * - AppNote:            AVR221 - Discrete PID controller
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support email: avr@atmel.com
 *
 * $Name$
 * $Revision: 456 $
 * $RCSfile$
 * $Date: 2006-02-16 12:46:13 +0100 (to, 16 feb 2006) $
 *****************************************************************************/

#include "pid.h"
#include "stdint.h"

/*! \brief Initialisation of PID controller parameters.
 *
 *  Initialise the variables used by the PID algorithm.
 *
 *  \param p_factor  Proportional term.
 *  \param i_factor  Integral term.
 *  \param d_factor  Derivate term.
 *  \param pid  Struct with PID status.
 */
void pid_Init(int32_t p_factor, int32_t i_factor, int32_t d_factor, struct PID_DATA *pid)
// Set up PID controller parameters
{
  // Start values for PID controller
  pid->sumError = 0;
  pid->lastProcessValue = 0;
  // Tuning constants for PID loop
  pid->P_Factor = p_factor;
  pid->I_Factor = i_factor;
  pid->D_Factor = d_factor;
  // Limits to avoid overflow
  pid->maxError = MAX_LONG / (pid->P_Factor + 1);

  /* Set the output to a harmless value */
  pid->i32PidResult = (UPPER_LIMIT - LOWER_LIMIT)/2 + LOWER_LIMIT;
}

/*! \brief PID control algorithm.
 *
 *  Calculates output from setpoint, process value and PID status.
 *
 *  \param setPoint  Desired value.
 *  \param processValue  Measured value.
 *  \param pid_st  PID status struct.
 */
int32_t pid_Controller(int32_t setPoint, int32_t processValue, struct PID_DATA *pid_st)
{
  int32_t error;
  int32_t i32Pterm,i32Dterm;
  static int32_t i32Iterm = 0;

  error = setPoint - processValue;

  // Calculate Pterm and limit error overflow
  if (error > pid_st->maxError)
  {
    i32Pterm = pid_st->maxError;
  }
  else if (error < -pid_st->maxError)
  {
    i32Pterm = -pid_st->maxError;
  }
  else
  {
    i32Pterm = pid_st->P_Factor * error;
  }

  // Calculate Iterm and limit integral runaway
  if ((pid_st->i32PidResult < UPPER_LIMIT) && ( pid_st->i32PidResult > LOWER_LIMIT))
  {
    /* Accumulate the error */
    pid_st->sumError += error;

    /* Store the I term for current&future use */
    i32Iterm= pid_st->I_Factor * pid_st->sumError;
  }

  // Calculate Dterm
  i32Dterm = pid_st->D_Factor * (pid_st->lastProcessValue - processValue);

  pid_st->lastProcessValue = processValue;

  pid_st->i32PidResult = (i32Pterm + i32Iterm + i32Dterm) / SCALING_FACTOR + MIDDLE;


  if (pid_st->i32PidResult > UPPER_LIMIT)
  {
    pid_st->i32PidResult = UPPER_LIMIT;
  }
  else if (pid_st->i32PidResult < LOWER_LIMIT)
  {
    pid_st->i32PidResult = LOWER_LIMIT;
  }

  return(pid_st->i32PidResult);
}

/*! \brief Resets the integrator.
 *
 *  Calling this function will reset the integrator in the PID regulator.
 */
void pid_Reset_Integrator(pidData_t *pid_st)
{
  pid_st->sumError = 0;
}

