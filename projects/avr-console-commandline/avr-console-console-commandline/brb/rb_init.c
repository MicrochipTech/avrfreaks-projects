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
 * initialize a ring-buffer control structure
 */
void rb_init(struct ring_buffer * const rb, uint8_t * const p, size_t s)
{
    rb->start = p;
    rb->limit = p + s;
    rb_clear(rb);
}
