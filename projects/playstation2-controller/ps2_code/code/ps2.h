/* 
 * PS2 controller on Atmel MCU
 * 
 * V1.0  
 * 
 * By: Lars Ivar Miljeteig, September 2010
 * Platform: AVR ATmega168 (avr-gcc)
 * Dependencies: avr/io avr/interrupt delay.h stdint.h larslib.h
 *
 * Reading data from a PlayStation 2 controller
 */

#ifndef PS2_H
#define PS2_H

#define PS2_MOTOR_ON 0x01
#define PS2_MOTOR_OFF 0x00

volatile uint8_t rx_buffer[20];



struct
{
    uint8_t lx, ly, rx, ry;
    uint8_t pressure[12];
} ps2;

// Pressure:
// R	L	U	D	Tri	O	X	Sqr	L1	R1	L2	R2


void ps2_init();
void ps2_poll(uint8_t speed, uint8_t smallmotor);

#endif // PS2_H
