/* Copyright (c) 2005, 2006, 2008  Paulo Costa, Paulo Marques
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

#ifndef	_BIT_TOOLS_H
#define	_BIT_TOOLS_H	1

#include <inttypes.h>

#define bits_set_8(var, mask) ((var) |= (uint8_t)(mask))
#define bits_clear_8(var, mask) ((var) &= (uint8_t)~(mask))
#define bits_toggle_8(var, mask) ((var) ^= (uint8_t)(mask))
#define bits_read_8(var, mask) ((var) & (uint8_t)(mask))
#define bits_write_8(var, mask) ((var) = (uint8_t)(mask))

#define bits_set_16(var, mask)   ((var) |= (uint16_t)(mask))
#define bits_clear_16(var, mask)   ((var) &= (uint16_t)~(mask))
#define bits_toggle_16(var, mask)   ((var) ^= (uint16_t)(mask))
#define bits_read_16(var, mask)   ((var) & (uint16_t)(mask))
#define bits_write_16(var, mask)   ((var) = (uint16_t)(mask))


// "Public" versions for the common operation.
#define bits_set(var, mask)   bits_set_8(var, mask)
#define bits_clear(var, mask) bits_clear_8(var, mask)
#define bits_toggle(var, mask) bits_toggle_8(var, mask)
#define bits_read(var, mask) bits_read_8(var, mask)
#define bits_write(var, mask) bits_write_8(var, mask)

#define bit(x)   ((uint8_t)1 << (x))
#define bit_long(x)   ((uint32_t)1 << (x))

#define bit_set(var, x)   bits_set_8(var, bit(x))
#define bit_clear(var, x) bits_clear_8(var, bit(x))
#define bit_toggle(var, x) bits_toggle_8(var, bit(x))
#define bit_read(var, x) bits_read_8(var, bit(x))


#endif
