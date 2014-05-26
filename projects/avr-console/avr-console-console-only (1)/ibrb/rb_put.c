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
 * rb_put - ring-buffer put
 *
 *  If space available, add byte to the tail of the ring-buffer and update the
 *  control structure.  This API also disables the echo facility.  Use this API
 *  when echo is not required.
 *
 * returns:  1 - byte added, more space available
 *           0 - byte added, no more space available
 *          -1 - byte not added, no space available
 */
int8_t rb_put(struct ring_buffer * const rb, volatile uint8_t const * const b)
{
    uint8_t * put;

    if (rb_is_cantput(rb)) return -1;

    /* byte will not be available for echo */
    rb_set_cantecho(rb);

    /* byte will be available for get */
    rb_clr_cantget(rb);

    /* add byte, update pointers */
    put = rb->put;
    *put = *b;
    rb_inc_ptr(rb, put);
    rb->echo = rb->put = put;

    if (put == rb->get) {
        /* no more space on exit */
        rb_set_cantput(rb);
        return 0;
    }

    /* more space on exit */
    return 1;
}
