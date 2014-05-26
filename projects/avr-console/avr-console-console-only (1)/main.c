/*
 * Copyright 2013, 2014 Chris Rhodin <chris@notav8.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "project.h"

#include <stdio.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>

#include "librb.h"
#include "console.h"


void main(void)
{
    uint16_t attr;

    /*
     * initialize
     */
    ATOMIC_BLOCK(ATOMIC_FORCEON);
    /* interrupts are enabled */

    /*
     * console demo
     */
    printf_P(PSTR("\n\navr-console, canonical input processing\n\n"));

    /*
     * display console attributes in an stty like way
     */
    attr = console_getattr();
    printf_P(PSTR("stty: %cicrnl %cecho %cicanon %cinonblock %conlcr\n\n"),
             (attr & ICRNL)     ? ' ' : '-',
             (attr & ECHO)      ? ' ' : '-',
             (attr & ICANON)    ? ' ' : '-',
             (attr & INONBLOCK) ? ' ' : '-',
             (attr & ONLCR)     ? ' ' : '-');

    /*
     * echo back whatever you type with line-editing
     */
    for (;;)
    {
        printf_P(PSTR("\nInput: "));

        for (;;)
        {
            int c = getchar();

            if (c >= 0)
            {
                putchar(c);
                if (c == '\n') break;
            }
        }
    }
}
