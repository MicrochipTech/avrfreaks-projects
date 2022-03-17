/*****************************************************************************
 *
 * \file
 *
 * \brief System-specific implementation of the \ref __write function used by
          the standard library.
 *
 * Copyright (c) 2011 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 ******************************************************************************/


#include <avr32/io.h>
#include "usart.h"

//! Pointer to the base of the USART module instance to use for stdio.
volatile avr32_usart_t *volatile stdio_usart_base;
int __attribute__((weak))
_write (int file, char * ptr, int len);

/**
 * Low-level write command.
 * When newlib buffer is full or fflush is called, this will output
 * data to correct location.
 * 1 and 2 is stdout and stderr which goes to usart
 * 3 is framebuffer
 */
int __attribute__((weak))
_write (int file, char * ptr, int len)
{
  int nChars = 0;

  if ( (file != 1)
       && (file != 2) && (file!=3))
    return -1;

  for (; len != 0; --len)
  {
    if (usart_putchar(stdio_usart_base, *ptr++) < 0)
    {
      return -1;
    }

    ++nChars;
  }

  return nChars;
}
