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
 * rb_put_echo - ring-buffer put with echo
 *
 *  If space available, add byte to the tail of the ring-buffer and update the
 *  control structure.  This API also enables the echo facility.  Use this API
 *  when echo is required.
 *
 * returns:  1 - byte added, more space available
 *           0 - byte added, no more space available
 *          -1 - byte not added, no space available
 */
int8_t rb_put_echo(struct ring_buffer * rb, volatile uint8_t const * const b)
{
    uint8_t * put;

    if (rb_is_cantput(rb)) return -1;

    /* byte will be available for echo */
    rb_clr_cantecho(rb);

    /* add byte, update pointer */
    put = rb->put;
    *put = *b;
    rb_inc_ptr(rb, put);
    rb->put = put;

    if (put == rb->get) {
        /* no more space on exit */
        rb_set_cantput(rb);
        return 0;
    }

    /* more space on exit */
    return 1;
}
