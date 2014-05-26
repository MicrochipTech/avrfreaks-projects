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
 * rb_erase - ring-buffer erase
 *
 *  Rewind the buffer back one byte.
 *
 * returns:  1 - echo pointer was moved
 *           0 - echo pointer was not moved
 */
uint8_t rb_erase(struct ring_buffer * const rb)
{
    uint8_t * put;

    if (rb_empty(rb)) return 0;

    /* space will be available for put */
    rb_clr_cantput(rb);

    /* update pointer */
    put = rb->put;
    rb_dec_ptr(rb, put);
    rb->put = put;

    /* if can't echo on entry we backed over an echoed byte */
    if (rb_is_cantecho(rb)) {
        rb->echo = put;
        if (put == rb->get) rb_set_cantget(rb);

        return 1;
    }

    if (put == rb->echo) rb_set_cantecho(rb);

    return 0;
}
