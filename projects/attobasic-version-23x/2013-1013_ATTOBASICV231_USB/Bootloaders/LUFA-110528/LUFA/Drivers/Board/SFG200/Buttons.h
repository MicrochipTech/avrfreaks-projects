/*
             LUFA Library
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2011  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *  \brief Board specific Buttons driver header for the SFG200.
 *  \copydetails Group_Buttons_SFG200
 *
 *  \note This file should not be included directly. It is automatically included as needed by the Buttons driver
 *        dispatch header located in LUFA/Drivers/Board/Buttons.h.
 */

/** \ingroup Group_Buttons
 *  \defgroup Group_Buttons_SFG200 SFG200
 *  \brief Board specific Buttons driver header for the SFG200.
 *
 *  Board specific Buttons driver header for the SFG200.
 *
 *  @{
 */

#ifndef __BUTTONS_SFG200_H__
#define __BUTTONS_SFG200_H__

	/* Includes: */
		#include "../../../Common/Common.h"

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Preprocessor Checks: */
		#if !defined(__INCLUDE_FROM_BUTTONS_H)
			#error Do not include this file directly. Include LUFA/Drivers/Board/Buttons.h instead.
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			/** Button mask for the SWA button on the board. */
			#define BUTTONS_BUTTON1      (1 << 6)
			/** Button mask for the SWB button on the board. */
//			#define BUTTONS_BUTTON1      (1 << 4)

		/* Inline Functions: */
		#if !defined(__DOXYGEN__)
			static inline void Buttons_Init(void)
			{
			/** Button port for the SWA button on the board. */
				DDRE  &= ~BUTTONS_BUTTON1;
				PORTE |=  BUTTONS_BUTTON1;
			/** Button port for the SWB button on the board. */
//				DDRB  &= ~BUTTONS_BUTTON1;
//				PORTB |=  BUTTONS_BUTTON1;
			}

			static inline uint8_t Buttons_GetStatus(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t Buttons_GetStatus(void)
			{
			/** Button port for the SWA button on the board. */
				return ((PINE & BUTTONS_BUTTON1) ^ BUTTONS_BUTTON1);
			/** Button port for the SWB button on the board. */
//				return ((PINB & BUTTONS_BUTTON1) ^ BUTTONS_BUTTON1);
			}
		#endif

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif

#endif

/** @} */

