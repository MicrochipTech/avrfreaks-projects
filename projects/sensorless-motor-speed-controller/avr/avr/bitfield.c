/* copyright
   * (c) 2004 Stephen Humble stephenhumble at telstra.com
   *
   *  This file is part of robotx see http://Robotx.sourceforge.net
   *  dual sensorless motor driver firmware.
   *
   *  This is free software; you can redistribute it and/or modify
   *  it under the terms of the GNU General Public License as published by
   *  the Free Software Foundation; either version 2 of the License, or
   *  (at your option) any later version.
*/
/*
   * Bit Field I/O routines.
   * used to read or write byte or word sized bit field data 
   * to individual AVR io port pins.
   * any AVR port pins can be mapped to each bit in the bit field.
   * used mainly for cases when you need to read or write a byte or word 
   * across arbitirally located IO pins on the avr 
*/
#ifdef BITFIELD
#include "bitfield.h"
#include "avr.h"
#include <avr/io.h>
#include ATMEGA

/* these defines were especially for simultaneousley reading from the 8 opto coupled inputs
   and writing to 8 relay isolated outputs on an interface card */

#define BFI0_PORT PORTC
#define BFI0_DDR DDRC
#define BFI0_PIN PINC
#define BFI0_BIT 0

#define BFI1_PORT PORTC
#define BFI1_DDR DDRC
#define BFI1_PIN PINC
#define BFI1_BIT 2

#define BFI2_PORT PORTC
#define BFI2_DDR DDRC
#define BFI2_PIN PINC
#define BFI2_BIT 1

#define BFI3_PORT PORTC
#define BFI3_DDR DDRC
#define BFI3_PIN PINC
#define BFI3_BIT 4

#define BFI4_PORT PORTC
#define BFI4_DDR DDRC
#define BFI4_PIN PINC
#define BFI4_BIT 5

#define BFI5_PORT PORTC
#define BFI5_DDR DDRC
#define BFI5_PIN PINC
#define BFI5_BIT 7

#define BFI6_PORT PORTC
#define BFI6_DDR DDRC
#define BFI6_PIN PINC
#define BFI6_BIT 6

#define BFI7_PORT PORTC
#define BFI7_DDR DDRC
#define BFI7_PIN PINC
#define BFI7_BIT 3

#define BFO0_PORT PORTC
#define BFO0_DDR DDRC
#define BFO0_PIN PINC
#define BFO0_BIT 1

#define BFO1_PORT PORTC
#define BFO1_DDR DDRC
#define BFO1_PIN PINC
#define BFO1_BIT 3

#define BFO2_PORT PORTC
#define BFO2_DDR DDRC
#define BFO2_PIN PINC
#define BFO2_BIT 0

#define BFO3_PORT PORTC
#define BFO3_DDR DDRC
#define BFO3_PIN PINC
#define BFO3_BIT 4

#define BFO4_PORT PORTC
#define BFO4_DDR DDRC
#define BFO4_PIN PINC
#define BFO4_BIT 2

#define BFO5_PORT PORTC
#define BFO5_DDR DDRC
#define BFO5_PIN PINC
#define BFO5_BIT 7

#define BFO6_PORT PORTC
#define BFO6_DDR DDRC
#define BFO6_PIN PINC
#define BFO6_BIT 6

#define BFO7_PORT PORTC
#define BFO7_DDR DDRC
#define BFO7_PIN PINC
#define BFO7_BIT 5

// global adc verables

void   bitfield_init( void )
{
   // setup output port bits for output
   BFO0_DDR|=_BV(BFO0_BIT);
   BFO1_DDR|=_BV(BFO1_BIT);
   BFO2_DDR|=_BV(BFO2_BIT);
   BFO3_DDR|=_BV(BFO3_BIT);
   BFO4_DDR|=_BV(BFO4_BIT);
   BFO5_DDR|=_BV(BFO5_BIT);
   BFO6_DDR|=_BV(BFO6_BIT);
   BFO7_DDR|=_BV(BFO7_BIT);

   // setup input port bits for input
   BFI0_DDR&=~_BV(BFI0_BIT);
   BFI1_DDR&=~_BV(BFI1_BIT);
   BFI2_DDR&=~_BV(BFI2_BIT);
   BFI3_DDR&=~_BV(BFI3_BIT);
   BFI4_DDR&=~_BV(BFI4_BIT);
   BFI5_DDR&=~_BV(BFI5_BIT);
   BFI6_DDR&=~_BV(BFI6_BIT);
   BFI7_DDR&=~_BV(BFI7_BIT);
}

/*
void bitfield_put2(uint8_t z)
{
  //egister uint8_t z=in;
   if(bit_is_clear(z,0))BFO0_PORT&=~_BV(BFO0_BIT);
   if(bit_is_clear(z,1))BFO1_PORT&=~_BV(BFO1_BIT);
   if(bit_is_clear(z,2))BFO2_PORT&=~_BV(BFO2_BIT);
   if(bit_is_clear(z,3))BFO3_PORT&=~_BV(BFO3_BIT);
   if(bit_is_clear(z,4))BFO4_PORT&=~_BV(BFO4_BIT);
   if(bit_is_clear(z,5))BFO5_PORT&=~_BV(BFO5_BIT);
   if(bit_is_clear(z,6))BFO6_PORT&=~_BV(BFO6_BIT);
   if(bit_is_clear(z,7))BFO7_PORT&=~_BV(BFO7_BIT);

   if(bit_is_set(z,0))BFO0_PORT|=_BV(BFO0_BIT);
   if(bit_is_set(z,1))BFO1_PORT|=_BV(BFO1_BIT);
   if(bit_is_set(z,2))BFO2_PORT|=_BV(BFO2_BIT);
   if(bit_is_set(z,3))BFO3_PORT|=_BV(BFO3_BIT);
   if(bit_is_set(z,4))BFO4_PORT|=_BV(BFO4_BIT);
   if(bit_is_set(z,5))BFO5_PORT|=_BV(BFO5_BIT);
   if(bit_is_set(z,6))BFO6_PORT|=_BV(BFO6_BIT);
   if(bit_is_set(z,7))BFO7_PORT|=_BV(BFO7_BIT);
}
*/

void bitfield_put(uint8_t z)
{
 //register uint8_t z=in;
   if(bit_is_clear(z,0)){
			   BFO0_PORT&=~_BV(BFO0_BIT);
			}else{  BFO0_PORT|=_BV(BFO0_BIT);
			     }
   if(bit_is_clear(z,1)){
			   BFO1_PORT&=~_BV(BFO1_BIT);
			}else{  BFO1_PORT|=_BV(BFO1_BIT);
			     }
   if(bit_is_clear(z,2)){
			   BFO2_PORT&=~_BV(BFO2_BIT);
			}else{  BFO2_PORT|=_BV(BFO2_BIT);
			     }
   if(bit_is_clear(z,3)){
			   BFO3_PORT&=~_BV(BFO3_BIT);
			}else{  BFO3_PORT|=_BV(BFO3_BIT);
			     }
   if(bit_is_clear(z,4)){
			   BFO4_PORT&=~_BV(BFO4_BIT);
			}else{  BFO4_PORT|=_BV(BFO4_BIT);
			     }
   if(bit_is_clear(z,5)){
			   BFO5_PORT&=~_BV(BFO5_BIT);
			}else{  BFO5_PORT|=_BV(BFO5_BIT);
			     }
   if(bit_is_clear(z,6)){
			   BFO6_PORT&=~_BV(BFO6_BIT);
			}else{  BFO6_PORT|=_BV(BFO6_BIT);
			     }
   if(bit_is_clear(z,7)){
			   BFO7_PORT&=~_BV(BFO7_BIT);
			}else{  BFO7_PORT|=_BV(BFO7_BIT);
			     }
}

/*
void bitfield_put(uint8_t z)
{
   BFO0_PORT=(uint8_t)(BFO0_PORT&~_BV(BFO0_BIT))|(uint8_t)(z&1<<BFO0_BIT);
   z>>=1;
   BFO1_PORT=(uint8_t)(BFO1_PORT&~_BV(BFO1_BIT))|(uint8_t)((z&1)<<BFO1_BIT);
   z>>=1;
   BFO2_PORT=(uint8_t)(BFO2_PORT&~_BV(BFO2_BIT))|(uint8_t)((z&1)<<BFO2_BIT);
   z>>=1;
   BFO3_PORT=(uint8_t)(BFO3_PORT&~_BV(BFO3_BIT))|(uint8_t)((z&1)<<BFO3_BIT);
   z>>=1;
   BFO4_PORT=(uint8_t)(BFO4_PORT&~_BV(BFO4_BIT))|(uint8_t)((z&1)<<BFO4_BIT);
   z>>=1;
   BFO5_PORT=(uint8_t)(BFO5_PORT&~_BV(BFO5_BIT))|(uint8_t)((z&1)<<BFO5_BIT);
   z>>=1;
   BFO6_PORT=(uint8_t)(BFO6_PORT&~_BV(BFO6_BIT))|(uint8_t)((z&1)<<BFO6_BIT);
   z>>=1;
   BFO7_PORT=(uint8_t)(BFO7_PORT&~_BV(BFO7_BIT))|(uint8_t)((z&1)<<BFO7_BIT);
}
*/

uint8_t bitfield_get()
{
   uint8_t z=0;
   if(bit_is_set(BFI0_PIN,BFI0_BIT))z|=1;
   if(bit_is_set(BFI1_PIN,BFI1_BIT))z|=2;
   if(bit_is_set(BFI2_PIN,BFI2_BIT))z|=4;
   if(bit_is_set(BFI3_PIN,BFI3_BIT))z|=8;
   if(bit_is_set(BFI4_PIN,BFI4_BIT))z|=16;
   if(bit_is_set(BFI5_PIN,BFI5_BIT))z|=32;
   if(bit_is_set(BFI6_PIN,BFI6_BIT))z|=64;
   if(bit_is_set(BFI7_PIN,BFI7_BIT))z|=128;
   return z;
}
#endif

