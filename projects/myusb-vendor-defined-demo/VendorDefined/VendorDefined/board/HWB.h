/*
             MyUSB Library
     Copyright (C) Dean Camera, 2007.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
 
  Adapted by P.Harvey-Smith, 2008-02-24, for the HWB on
  an AT90USB162.
  
*/


#ifndef __HWB_USER_H__
#define __HWB_USER_H__

	/* Includes: */
		#include <avr/io.h>
		#include <stdbool.h>

		#include "../../../MyUSB/Common/Common.h"
		
	/* Public Interface - May be used in end-application: */
		/* Inline Functions: */
			static inline void HWB_Init(void)
			{
				DDRD  &= ~(1 << 7);
				PORTD |=  (1 << 7);
			}

			static inline bool HWB_GetStatus(void) ATTR_WARN_UNUSED_RESULT;
			static inline bool HWB_GetStatus(void)
			{
				return (!(PIND & (1 << 7)));
			}
			
#endif
