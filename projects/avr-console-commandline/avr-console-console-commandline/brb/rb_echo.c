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
#include <stddef.h>
#include <stdint.h>
#include "librb.h"


/*
 * rb_echo - ring-buffer echo
 *
 *  If available, return echo byte from the ring-buffer and update the control
 *  structure.
 *
 * returns:  1 - byte returned, more available
 *           0 - byte returned, no more available
 *          -1 - byte not returned, none available
 */
int8_t rb_echo(struct ring_buffer * const rb, volatile uint8_t * const b)
{
    uint8_t * echo;

    if (rb_is_cantecho(rb)) return -1;

    /* byte will be available for get */
    rb_clr_cantget(rb);

    /* return byte, update pointer */
    echo = rb->echo;
    *b = *echo;
    rb_inc_ptr(rb, echo);
    rb->echo = echo;

    if (echo == rb->put) {
        /* echo byte not available on exit */
        rb_set_cantecho(rb);
        return 0;
    }

    /* echo byte available on exit */
    return 1;
}
