/**
 * \file
 *
 * \brief Commonly used includes, types and macros.
 *
 * Copyright (c) 2011-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
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
 */
#ifndef UTILS_COMPILER_H
#define UTILS_COMPILER_H

#if defined(__GNUC__)
#	include <avr/io.h>
#elif defined(__ICCAVR__)
#	include <ioavr.h>
#	include <intrinsics.h>
#else
#	error "Unsupported compiler."
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <parts.h>

#ifdef __ICCAVR__
/*! \name Compiler Keywords
 *
 * Port of some keywords from GCC to IAR Embedded Workbench.
 */
//! @{
#define __asm__              asm
#define __inline__           inline
#define __volatile__
//! @}
#endif

/**
 * \def UNUSED
 * \brief Marking \a v as a unused parameter or value.
 */
#define UNUSED(v)          (void)(v)

/**
 * \def unused
 * \brief Marking \a v as a unused parameter or value.
 */
#define unused(v)          do { (void)(v); } while(0)

/**
 * \def barrier
 * \brief Memory barrier
 */
#ifdef __GNUC__
#  define barrier()        asm volatile("" ::: "memory")
#else
#  define barrier()        asm ("")
#endif

//_____ M A C R O S ________________________________________________________


/**
 * \def __always_inline
 * \brief The function should always be inlined.
 *
 * This annotation instructs the compiler to ignore its inlining
 * heuristics and inline the function no matter how big it thinks it
 * becomes.
 */
#if (defined __GNUC__)
#	define __always_inline   inline __attribute__((__always_inline__))
#elif (defined __ICCAVR__)
#	define __always_inline   _Pragma("inline=forced")
#endif


/*! \brief This macro is used to test fatal errors.
 *
 * The macro tests if the expression is false. If it is, a fatal error is
 * detected and the application hangs up. If TEST_SUITE_DEFINE_ASSERT_MACRO
 * is defined, a unit test version of the macro is used, to allow execution
 * of further tests after a false expression.
 *
 * \param expr  Expression to evaluate and supposed to be nonzero.
 */
#if defined(_ASSERT_ENABLE_)
#  if defined(TEST_SUITE_DEFINE_ASSERT_MACRO)
	// Assert() is defined in unit_test/suite.h
#    include "unit_test/suite.h"
#  else
#    define Assert(expr) \
	{\
		if (!(expr)) while (true);\
	}
#  endif
#else
#  define Assert(expr) ((void) 0)
#endif


/*! \name MCU Endianism Handling
 */
//! @{
#define MSB(u16)             (((uint8_t* )&u16)[1])
#define LSB(u16)             (((uint8_t* )&u16)[0])
//! @}

#include "interrupt.h"

#endif  // UTILS_COMPILER_H
