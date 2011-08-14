/**
 * \file
 *
 * \brief XMEGA device family definitions
 *
 * Copyright (C) 2010 Atmel Corporation. All rights reserved.
 *
 * \page License
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
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef XMEGA_PARTS_H
#define XMEGA_PARTS_H

//! Convenience macro for checking GCC and IAR part definitions
#define part_is_defined(part) \
	(defined(__ ## part ## __) || defined(__AVR_ ## part ## __))

// A1 Family
#define XMEGA_A1 ( \
		part_is_defined(ATxmega64A1) || \
		part_is_defined(ATxmega128A1) \
	)

// A3 Family
#define XMEGA_A3 ( \
		part_is_defined(ATxmega64A3) || \
		part_is_defined(ATxmega128A3) || \
		part_is_defined(ATxmega192A3) || \
		part_is_defined(ATxmega256A3) \
	)

// A3B Family
#define XMEGA_A3B ( \
		part_is_defined(ATxmega256A3B) \
	)

// A4 Family
#define XMEGA_A4 ( \
		part_is_defined(ATxmega16A4) || \
		part_is_defined(ATxmega32A4) \
	)

// Entire A Family
#define XMEGA_A (XMEGA_A1 || XMEGA_A3 || XMEGA_A3B || XMEGA_A4)


// D3 Family
#define XMEGA_D3 ( \
		part_is_defined(ATxmega64D3) || \
		part_is_defined(ATxmega128D3) || \
		part_is_defined(ATxmega192D3) || \
		part_is_defined(ATxmega256D3) \
	)

// D4 Family
#define XMEGA_D4 ( \
		part_is_defined(ATxmega16D4) || \
		part_is_defined(ATxmega32D4) \
	)

// Entire D Family
#define XMEGA_D (XMEGA_D3 || XMEGA_D4)

// Entire XMEGA Family
#define XMEGA (XMEGA_A  || XMEGA_D)

#endif /* XMEGA_PARTS_H */
