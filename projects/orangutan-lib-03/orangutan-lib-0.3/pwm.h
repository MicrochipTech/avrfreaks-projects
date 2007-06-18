// pwm.h
//
// Tom Benedict
// Steve Judd

// Only include this code if we have an Orangutan device that has an
// onboard LB1836M motor driver (set in "device.h").
#ifdef _ENABLE_LB1836M_MOTORS_

#ifndef _PWM_
#define _PWM_

// Initialize timer 0 to do PWM for our motors:
void pwm_init(void);

// Set speed on motor A (Speed = 0 disconnects the PWM system)
void pwm_a(int16_t speed);

// Set speed on motor B (Speed = 0 disconnects the PWM system)
void pwm_b(int16_t speed);

// Apply brakes to motor A
void pwm_a_brake(void);

// Apply brakes to motor B
void pwm_b_brake(void);

// To coast you'd use:
//   pwm_a(0);
// or
//   pwm_b(0);

// That's really it for the motor stuff.

#endif // _PWM_

#endif // _ENABLE_LB1836M_MOTORS_
