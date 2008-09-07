/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
 
 Adapted by P.Harvey-Smith, 2008-03-01 for a Sega Genesis/Megadrive 
 Joypad conencted to port B (selectable via defines).
 Adapted to also support the Atari standard single button Joystick,
 selectable by jumper. 2008-03-02, PHS.
 
 The joypad is connected as follows :-
 
 Function			D9-pin		AVR-PIN
 Up					1			PortB 0
 Down				2			PortB 1
 Left/GND			3			PortB 2
 Right/GND			4			PortB 3
 VCC				5
 Button B/Button A	6			PortB 4 
 Button Select		7			PortB 6
 Gnd				8			GND
 Button C/Start		9			PortB 5	
 
 Joypad/Joystick mode select	PortB 7	
 
 In the joypad the Left, Right, Button B/A, ButtonC /Start are wired through a 
 74LS157 2to1 multiplexer, the Button Select line selects which group of buttons 
 are being read, when high it reads Left, Right, Button B, Button C, when low
 Gnd, GND, Button A, Start.
 
 The mode select input selects between a standard Atari type joystick (unconnected),
 or a Megadrive/Genesis gamepad (grounded), when Atari mode is selected then the
 joystick button would activate button b on the joypad controler, this is remapped
 to button a.

*/

#ifndef __JOYSTICK_USER_H__
#define __JOYSTICK_USER_H__

	/* Includes: */
		#include <avr/io.h>
		#include <util/delay.h>
	
		#include <MyUSB/Common/Common.h>
		
//		#include "../../../Common/Common.h"

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			#define JOY_DOWN					(1 << 0)
			#define JOY_UP						(1 << 1)
			#define JOY_LEFT					(1 << 2)
			#define JOY_RIGHT					(1 << 3)
			#define JOY_PRESS					(1 << 4)

			#define JOY_PRESSB					JOY_PRESS
			#define JOY_PRESSC					(1 << 5)
			#define JOY_PRESSA					(1 << 6)
			#define JOY_START					(1 << 7)

	/* Private Interface - For use in library only: */
		/* define the port to use */
			#define JOYSTICKDDR					DDRB
			#define JOYSTICKPORT				PORTB
			#define	JOYSTICKPIN					PINB
			
		/* Macros: */
			#define JOYSTICK_MASK_UP			(1 << 0)
			#define JOYSTICK_MASK_DOWN			(1 << 1)
			#define JOYSTICK_MASK_LEFT			(1 << 2)
			#define JOYSTICK_MASK_RIGHT			(1 << 3)
			#define JOYSTICK_MASK_A				(1 << 4)
			#define JOYSTICK_MASK_B				JOYSTICK_MASK_A
			#define JOYSTICK_MASK_C				(1 << 5)
			#define JOYSTICK_MASK_START			JOYSTICK_MASK_C
			#define JOYSTICK_MASK_MODE			(1 << 7)
			
			#define JOYSTICK_MASK				(JOYSTICK_MASK_B | JOYSTICK_MASK_RIGHT | JOYSTICK_MASK_LEFT | JOYSTICK_MASK_DOWN | JOYSTICK_MASK_UP | JOYSTICK_MASK_C)
			#define JOYSTICK_MASK_ATARI			(JOYSTICK_MASK_B | JOYSTICK_MASK_RIGHT | JOYSTICK_MASK_LEFT | JOYSTICK_MASK_DOWN | JOYSTICK_MASK_UP) 
			#define JOYSTICK_MASK_ALT			(JOYSTICK_MASK_A | JOYSTICK_MASK_START)
			#define JOYSTICK_SELECT				(1 << 6)
			
			
		/* Inline Functions: */
			static inline void Joystick_Init(void)
			{
				JOYSTICKDDR &= ~(JOYSTICK_MASK);	// Setup inputs	
				JOYSTICKDDR |= JOYSTICK_SELECT;		// Setup output
				
				JOYSTICKPORT |= (JOYSTICK_MASK | JOYSTICK_SELECT | JOYSTICK_MASK_MODE); // normal button set
			};
			
			static inline uint8_t Joystick_GetStatus(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t Joystick_GetStatus(void)
			{
				uint8_t Buttons;
				
				JOYSTICKPORT |= JOYSTICK_SELECT;			// Select L,R,B,c
				_delay_us(5);								// let them settle
				Buttons=JOYSTICKPIN;						// Read normal buttons
				
				if(Buttons & JOYSTICK_MASK_MODE)
				{
					// Standard Atari joystick selected.
					Buttons &= JOYSTICK_MASK_ATARI;
					
					// If button pressed, it will appear as button B, convert it to button A
					if(Buttons & JOYSTICK_MASK_B) 
					{
						Buttons &= ~JOY_PRESSB;		// Turn off button B
						Buttons |= JOY_PRESSA;			// Turn on button A
					}
				}
				else
				{	
					// Megadrive/Genesis gamepad selected, read alt buttons as well
					Buttons &= JOYSTICK_MASK;
					
					JOYSTICKPORT &= ~JOYSTICK_SELECT;						// select A,Start
					_delay_us(5);										// let them settle
					Buttons |= ((JOYSTICKPIN & JOYSTICK_MASK_ALT) << 2);	// Read alt buttons

					JOYSTICKPORT |= JOYSTICK_SELECT;						// Reset select to normal buttons
				}
				
				return ~Buttons;
			}

#endif
