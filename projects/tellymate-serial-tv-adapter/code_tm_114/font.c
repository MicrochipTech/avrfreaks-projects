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

#include "font.h"

/*
Note:
The format of this font data is slightly unusual.

each fontbank is a sequence of 256 characters.
The number of fontbanks in flash-rom is determined by how much flash
space this device has.

FontBank0
	Slice 0 of each character (256 bytes)
	slice 1 of each character
	...
	slice 9 of each character
FontBank1 [if flash space is available]
	Slice 0 of each character
	slice 1 of each character
	...
	slice 9 of each character
FontBank2 [if flash space is available]
	...
FontBank3 [if flash space is available]
	...
	slice 9 of each character


*/

uint8_t __attribute__((section (".font"))) f_font[] =
{
#include "fontbank0.c"
#if FONTBANK_COUNT > 1
,
#include "fontbank1.c"
#endif
#if FONTBANK_COUNT > 2
,
#include "fontbank2.c"
#endif
#if FONTBANK_COUNT > 3
,
#include "fontbank3.c"
#endif
#if FONTBANK_COUNT > 4
,
#include "fontbank4.c"
#endif
#if FONTBANK_COUNT > 5
,
#include "fontbank5.c"
#endif
#if FONTBANK_COUNT > 6
,
#include "fontbank6.c"
#endif
#if FONTBANK_COUNT > 7
,
#include "fontbank7.c"
#endif
#if FONTBANK_COUNT > 8
,
#include "fontbank8.c"
#endif
#if FONTBANK_COUNT > 9
,
#include "fontbank9.c"
#endif
#if FONTBANK_COUNT > 10
,
#include "fontbank10.c"
#endif
};
