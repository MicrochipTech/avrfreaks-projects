/*
 * shift.h
 * libavrutil
 *
 * Created by Árpád Goretity on 26/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef __AVRUTIL_SHIFT_H__
#define __AVRUTIL_SHIFT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdlib.h>
#include <stdint.h>
#include "base.h"

/*
 * Shift-register input and output functions
 * To understand shift register logic, see Wikipedia for an explanation:
 * http://en.wikipedia.org/wiki/Shift_register#Serial-in.2C_serial-out_.28SISO.29
 *
 * The shift register functions have a small built-in delay
 * in order to be compatible with slower circuits without data loss.
 * This limits the bandwidth to a few hundred kilobytes per second.
 *
 * All of these functions expect a _pointer_ to I/O port registers,
 * e. g. &PORTB or &PIND.
 * They all use *clockport and clockpin as the clock signal input/output
 * and *dataport and datapin as the data input/output, respectively.
 *
 * `Master' functions generate the clock signal on the output clock port,
 * so you'll have to supply them with an _output_ I/O register, such as &PORTB.
 * In contrast, `slave' functions expect an external clock signal to be present
 * (by another, master mode shift register device), so they must be given
 * a pointer to an _input_ register, such as &PINB.
 *
 * The input functions assume that the least significant bit (bit 0) of the data
 * is being sent first, and the output functions send the least significant bit first.
 */

/*
 * Reads a byte from the specified port/pin, master mode
 */
uint8_t avr_shift_byte_in_master(volatile uint8_t *dataport, uint8_t datapin, volatile uint8_t *clockport, uint8_t clockpin);

/*
 * Writes a byte to the specified port/pin, master mode
 */
void avr_shift_byte_out_master(volatile uint8_t *dataport, uint8_t datapin, volatile uint8_t *clockport, uint8_t clockpin, uint8_t byte);

/*
 * Reads a byte from the specified port/pin, slave mode
 */
uint8_t avr_shift_byte_in_slave(volatile uint8_t *dataport, uint8_t datapin, volatile uint8_t *clockport, uint8_t clockpin);

/*
 * Writes a byte to the specified port/pin, slave mode
 */
void avr_shift_byte_out_slave(volatile uint8_t *dataport, uint8_t datapin, volatile uint8_t *clockport, uint8_t clockpin, uint8_t byte);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AVRUTIL_SHIFT_H__ */

