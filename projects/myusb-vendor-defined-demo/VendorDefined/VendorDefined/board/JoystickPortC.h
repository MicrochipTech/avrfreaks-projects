/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
 
 Adapted by P.Harvey-Smith, 2008-02-24 for a standard Atari Joystick
 connected to port C of the ATUSB162.
 
 The joystick is connected as follows :-
 
 Function	D9-pin	AVR-PIN
 Up			1		PortC 7
 Down		2		PortC 6
 Left		3		PortC 5
 Right		4		PortC 4
 Button		6		PortC 2
 Gnd		8		GND
 
 This could fairly easily be extended to support multi-button joysticks such
 as Sega Genesis/Megadrive Pads, which use the same D9 connector but multiplex
 the buttons.
 
*/

#ifndef __JOYSTICK_USER_H__
#define __JOYSTICK_USER_H__

	/* Includes: */
		#include <avr/io.h>
		
		#include "../../../MyUSB/Common/Common.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define JOY_PRESS                 (1 << 2)
			#define JOY_RIGHT                 (1 << 4)
			#define JOY_LEFT                  (1 << 5)
			#define JOY_DOWN                  (1 << 6)
			#define JOY_UP                    (1 << 7)

	/* Private Interface - For use in library only: */
		/* Macros: */
			#define JOY_MASK                 (JOY_PRESS | JOY_RIGHT | JOY_LEFT | JOY_DOWN | JOY_UP)
			
		/* Inline Functions: */
			static inline void Joystick_Init(void)
			{
				DDRC &= ~(JOY_MASK);
				
				PORTC |= JOY_MASK; // Pullups enabled
			};
			
			static inline uint8_t Joystick_GetStatus(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t Joystick_GetStatus(void)
			{
				return (~PINC & JOY_MASK);	
			}

#endif
