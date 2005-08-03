#ifndef __JOYSTICK_HW_H__
#define __JOYSTICK_HW_H__

#ifdef AVR

#include <avr/io.h>

# ifndef JOYSTICK_PORT
#  define JOYSTICK_PORT     PORTC
#  define JOYSTICK_PORT_IN  PINC
#  define JOYSTICK_DDR      DDRC
#  define JOYSTICK_PORT_MASK 0x1f

#  define JOYSTICK_UP     _BV(PC0)
#  define JOYSTICK_DOWN   _BV(PC1)
#  define JOYSTICK_LEFT   _BV(PC2)
#  define JOYSTICK_RIGHT  _BV(PC3)
#  define JOYSTICK_BUTTON _BV(PC4)
# endif // #ifndef JOYSTICK_PORT

# define joystick_hw_init() (JOYSTICK_DDR &= ~JOYSTICK_PORT_MASK)
# define joystick_hw_read() (JOYSTICK_PORT_IN & JOYSTICK_PORT_MASK)

#endif // #ifdef AVR

#endif //#ifndef __JOYSTICK_HW_H__

