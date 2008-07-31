#ifndef GLOBAL_H
#define GLOBAL_H

#include "servo.h"

volatile uint8_t  servo_channel_A_updated;  		// this flag variable is set when channel A can be updated with new servo values.  This is not static because it is used externally in main()
volatile uint8_t  servo_channel_B_updated;		// this flag variable is set when channel B can be updated with new servo values.  This is not static because it is used externally in main()
uint16_t servo_out[SERVO_OUTPUT_CHANNELS];			// array holds the 20 servo position values

#endif
