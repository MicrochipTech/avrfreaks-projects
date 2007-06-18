// relay-motor.h
//
// Tom Benedict

#ifndef _RELAY_MOTOR_
#define _RELAY_MOTOR_

// Initialize our relay-style routines for our motors:
void motor_init(void);

// Spin motor A forward
void motor_a_fwd(void);

// Spin motor B forward
void motor_b_fwd(void);

// Spin motor A reverse
void motor_a_rev(void);

// Spin motor B reverse
void motor_b_rev(void);

// Let motor A coast
void motor_a_coast(void);

// Let motor B coast
void motor_b_coast(void);

// Apply brakes to motor A
void motor_a_brake(void);

// Apply brakes to motor B
void motor_b_brake(void);

// And that's it!  With those routines, we've got complete
// relay-style control over two motors including coast and
// brake.

#endif //_RELAY_MOTOR_
