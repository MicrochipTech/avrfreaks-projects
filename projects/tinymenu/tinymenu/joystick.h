#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include <inttypes.h>

#include "joystick_hw.h"


/* joystick_init: initialize ports for reading joystick
 */

void joystick_init(void);

/* joystick_read: returns OR'd values of any of the
 *  following which are closed:
 *
 *  JOYSTICK_UP
 *  JOYSTICK_DOWN
 *  JOYSTICK_LEFT
 *  JOYSTICK_RIGHT
 *  JOYSTICK_BUTTON
 *
 * -- definitions in joystick_hw.h
 */

uint8_t joystick_read(void);

#endif // #ifndef __JOYSTICK_H__
