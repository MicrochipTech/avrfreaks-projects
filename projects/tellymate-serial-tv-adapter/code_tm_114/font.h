/*
Copyright (c) 2009, Nigel Batten.
Contactable at <firstname>.<lastname>@mail.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

	1.	Redistributions of source code must retain the above copyright
		notice, this list of conditions and the following disclaimer.
	2.	Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.

===============================================
*/

#ifndef _font_h__
#define _font_h__

#include <avr/io.h>
#include <avr/pgmspace.h>

#define FONT_CHAR_HEIGHT 9
#define FONT_CHAR_COUNT 256

/*
Use FLASHEND to determine how many font banks the flash memory in this processor can support...
M8    = 0x1FFF
M88   = 0x1FFF
M168  = 0x3FFF
M328p = 0x7FFF

Each fontbank is 2304 bytes long (9 * 256 bytes).
1  fits into a M8.
4  fit in a M168.
11 fit in a M328p (!), [but only 4 are defined]

*/

#ifndef FONTBANK_COUNT
#define FONTBANK_COUNT 1
#endif

#if FLASHEND > 0x3FFF
// this is a 32kb flash part.
	#if FONTBANK_COUNT > 11
	#error This device only has room for 11 fontbanks. You have set the FONTBANK_COUNT macro to be more than this.
	#endif
#elif FLASHEND > 0x1FFF
// this is a 16kb flash part.
	#if FONTBANK_COUNT > 4
	#error This device only has room for 4 fontbanks. You have set the FONTBANK_COUNT macro to be more than this.
	#endif
#else
// this is a 8kb flash part
	#if FONTBANK_COUNT > 1
	#error This device only has room for 1 fontbank. You have set the FONTBANK_COUNT macro to be more than this.
	#endif
#endif

#if FONTBANK_COUNT < 1
	#error You must have at least 1 fontbank.  You have set the FONTBANK_COUNT macro to be less than this!
#endif

extern uint8_t PROGMEM f_font[ FONT_CHAR_HEIGHT * FONT_CHAR_COUNT * FONTBANK_COUNT ];
#endif
