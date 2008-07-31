#ifndef SERVO_H
#define SERVO_H

#include <avr/interrupt.h>  

#define SERVO_OUTPUT_CHANNELS		20			// number of servos 

#define SetBit(ADDRESS,BIT) (ADDRESS |= (1<<BIT)) 
#define ClearBit(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT)) 
#define FlipBit(ADDRESS,BIT) (ADDRESS ^= (1<<BIT)) 
#define CheckBit(ADDRESS,BIT) (ADDRESS & (1<<BIT)) 
#define SetBitMask(x,y) (x |= (y)) 
#define ClearBitMask(x,y) (x &= (~y)) 
#define FlipBitMask(x,y) (x ^= (y)) 
#define CheckBitMask(x,y) (x & (y))

void Init_servo_out(void);			// function to initialize things

#endif

