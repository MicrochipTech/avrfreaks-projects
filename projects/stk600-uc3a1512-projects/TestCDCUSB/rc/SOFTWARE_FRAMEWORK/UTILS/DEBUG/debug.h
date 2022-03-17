/* This header file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3A-1.4.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Macros and functions dedicated to debug purposes.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a USART module can be used.
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/* Copyright (C) 2006-2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "stringz.h"

/*! \brief These macros are used to add traces memory.
 *
 * First, initialise the trace with Uc3_trace_init(pointer), giving the start address
 * of the memory location where will be stored the trace.
 * Use Uc3_trace(something) to store "something" into the memory. The end of the trace
 * is signaled by a dead beef.
 */
#define Uc3_trace_init(debug_addr)   \
      *(U32*)(debug_addr)=debug_addr+4

#define Uc3_trace(debug_addr, x)   \
      *(U32*)(*(U32*)(debug_addr)  ) = (U32)(x)   ;\
      *(U32*)(*(U32*)(debug_addr)+4) = 0xdeadbeef ;\
      *(U32*)(debug_addr  ) = *(U32*)(debug_addr)+4

/*! \brief This macro is used to insert labels into assembly output.
 *
 */
#define Insert_label(name)         \
    __asm__ __volatile__ (STRINGZ(name)":");

/*! \brief Returns the number of free bytes in the HEAP.
 *
 * \retval number of free bytes.
 */
extern U32 get_heap_free_size( void );


#endif  // _DEBUG_H_
