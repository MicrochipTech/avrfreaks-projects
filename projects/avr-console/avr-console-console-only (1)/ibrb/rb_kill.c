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
 * rb_kill - ring-buffer kill
 *
 *  Rewind the buffer back to the passed pointer position.  Return the number of
 *  echoed bytes removed.  The caller is responsible for passing a p value in
 *  the range get to put inclusive.
 */
uint8_t rb_kill(struct ring_buffer * const rb, uint8_t * p)
{
    uint8_t * put = rb->put;
    uint8_t * echo = rb->echo;
    uint8_t unecho = 0;
    uint8_t s = rb->limit - rb->start;

    /* unwrap the buffer pointers relative to get */
    if (p < rb->get) p += s;
    if (rb_is_cantput(rb) || (put < rb->get)) put += s;
    if (!rb_is_cantget(rb) && (echo <= rb->get)) echo += s;

    if (p < put) {
        rb_clr_cantput(rb);
        put = p;
        if (p < echo) {
            unecho = echo - p;
            echo = p;
            if (rb->get == p) rb_set_cantget(rb);
        }
        if (echo == p) rb_set_cantecho(rb);
    }

    /* wrap the buffer pointers */
    if (put >= rb->limit) put -= s;
    if (echo >= rb->limit) echo -= s;

    rb->put = put;
    rb->echo = echo;

    return unecho;
}
