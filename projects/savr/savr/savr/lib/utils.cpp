/*********************************************************************************
 Copyright (C) 2011 by Stefan Filipek

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*********************************************************************************/

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <savr/cpp_pgmspace.h>
#include <savr/utils.h>

namespace Utils {

static const char CPP_PROGMEM BYTEwPAD[] = "%02X ";
static const char CPP_PROGMEM dPAD[]     = "  ";


/**
 * @par Implementation notes:
 */
void
PrintHex(void *data, size_t size)
{

    uint8_t * input;
    size_t i;

    input = (uint8_t *)data;

    for(i=0; i<size; i++) {
        printf_P(BYTEwPAD, input[i]);
    }

}


/**
 * @par Implementation notes:
 */
void
PrintBlock(void *data, size_t size, uint32_t index, size_t width)
{

    uint8_t * input;
    size_t i;
    size_t j;

    input = (uint8_t *)data;

    for(i=0; i<size; i+=width, index+=width) {
        printf_P(PSTR("0x%08lX:"), index);
        printf_P(dPAD);

        for(j=0; j<width; j++) {
            if(i+j < size) {
                printf_P(BYTEwPAD, input[i+j]);
            } else {
                fputs_P(PSTR("-- "), stdout);
            }
        }

        printf_P(dPAD);

        for(j=0; j<width; j++) {
            if(i+j < size) {
                putchar((isprint(input[i+j]) ? input[i+j] : '.'));
            } else {
                putchar('.');
            }
        }
        putchar('\n');
        fflush(stdout); // Null op for avr-libc, included for copy/paste compatibility
    }
}

};
