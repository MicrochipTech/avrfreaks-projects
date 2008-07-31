#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>

#define SetBit8(ADDRESS,BIT) (ADDRESS |= (uint8_t)(1<<BIT)) 
#define ClearBit8(ADDRESS,BIT) (ADDRESS &= (uint8_t)~(1<<BIT)) 
#define FlipBit8(ADDRESS,BIT) (ADDRESS ^= (uint8_t)(1<<BIT)) 
#define CheckBit8(ADDRESS,BIT) (ADDRESS & (uint8_t)(1<<BIT)) 
#define SetBitMask8(x,y) (x |= (uint8_t)(y)) 
#define ClearBitMask8(x,y) (x &= (uint8_t)(~y)) 
#define FlipBitMask8(x,y) (x ^= (uint8_t)(y)) 
#define CheckBitMask8(x,y) ((uint8_t)x & (uint8_t)(y))

#define SetBit(ADDRESS,BIT) (ADDRESS |= (1<<BIT)) 
#define ClearBit(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT)) 
#define FlipBit(ADDRESS,BIT) (ADDRESS ^= (1<<BIT)) 
#define CheckBit(ADDRESS,BIT) (ADDRESS & (1<<BIT)) 
#define SetBitMask(x,y) (x |= (y)) 
#define ClearBitMask(x,y) (x &= (~y)) 
#define FlipBitMask(x,y) (x ^= (y)) 
#define CheckBitMask(x,y) (x & (y))

extern volatile uint8_t  servo_channel_A_updated;  	// this flag variable is set when channel A can be updated with new servo values.  
extern volatile uint8_t  servo_channel_B_updated;	// this flag variable is set when channel B can be updated with new servo values.
extern uint16_t servo_out[SERVO_OUTPUT_CHANNELS];	// array holds the 20 servo position values

#endif
