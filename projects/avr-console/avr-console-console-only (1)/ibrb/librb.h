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
#ifndef _LIBRB_H_
#define _LIBRB_H_

#include <avr/io.h>
#include "bits.h"

/*
 * ring-buffer control stucture
 */
struct ring_buffer {
    uint8_t * start;
    uint8_t * limit;
    uint8_t * volatile put;
    uint8_t * volatile echo;
    uint8_t * volatile get;

    uint8_t flags;
};

/*
 * ring-buffer flag bits
 */
enum {
    RB_CANTPUT      = 0,
    RB_CANTECHO     = 1,
    RB_CANTGET      = 2,
    RB_SPARE0       = 3,
    RB_SPARE1       = 4,
    RB_SPARE2       = 5,
    RB_SPARE3       = 6,
    RB_SPARE4       = 7,
    RB_CANTPUT_MSK  = _BV(RB_CANTPUT),
    RB_CANTECHO_MSK = _BV(RB_CANTECHO),
    RB_CANTGET_MSK  = _BV(RB_CANTGET),
    RB_SPARE0_MSK   = _BV(RB_SPARE0),
    RB_SPARE1_MSK   = _BV(RB_SPARE1),
    RB_SPARE2_MSK   = _BV(RB_SPARE2),
    RB_SPARE3_MSK   = _BV(RB_SPARE3),
    RB_SPARE4_MSK   = _BV(RB_SPARE4),
};

/*
 * ring-buffer state
 */
#define rb_set_cantput(a)    set_bit((a)->flags,RB_CANTPUT)
#define rb_clr_cantput(a)  clear_bit((a)->flags,RB_CANTPUT)
#define rb_is_cantput(a)    test_bit((a)->flags,RB_CANTPUT)
#define rb_set_cantecho(a)   set_bit((a)->flags,RB_CANTECHO)
#define rb_clr_cantecho(a) clear_bit((a)->flags,RB_CANTECHO)
#define rb_is_cantecho(a)   test_bit((a)->flags,RB_CANTECHO)
#define rb_set_cantget(a)    set_bit((a)->flags,RB_CANTGET)
#define rb_clr_cantget(a)  clear_bit((a)->flags,RB_CANTGET)
#define rb_is_cantget(a)    test_bit((a)->flags,RB_CANTGET)
#define rb_empty(a)     (rb_is_cantecho(a) && rb_is_cantget(a))
#define rb_full(a)      rb_is_cantput(a)

/*
 * clear ring-buffer
 */
#define rb_clear(a) do {                                                       \
    set_clear_bits((a)->flags,RB_CANTECHO_MSK|RB_CANTGET_MSK,RB_CANTPUT_MSK);  \
    (a)->get = (a)->echo = (a)->put = (a)->start;                              \
} while (0)

/*
 * increment and wrap buffer pointer
 */
#define rb_inc_ptr(a,b) do {                                                   \
    b++;                                                                       \
    if (b == (a)->limit) b = (a)->start;                                       \
} while (0)

/*
 * (un)wrap and decrement buffer pointer
 */
#define rb_dec_ptr(a,b) do {                                                   \
    if (b == (a)->start) b = (a)->limit;                                       \
    b--;                                                                       \
} while (0)

/*
 * ring-buffer control/access functions
 */
extern void rb_init(struct ring_buffer * const rb, uint8_t * const p, size_t s);
extern int8_t rb_put(struct ring_buffer * const rb, volatile uint8_t const * const b);
extern int8_t rb_put_echo(struct ring_buffer * const rb, volatile uint8_t const * const b);
extern int8_t rb_unput(struct ring_buffer * const rb, volatile uint8_t * const b);
extern int8_t rb_echo(struct ring_buffer * const rb, volatile uint8_t * const b);
extern int8_t rb_get(struct ring_buffer * const rb, volatile uint8_t * const b);
extern uint8_t rb_erase(struct ring_buffer * const rb);
extern uint8_t rb_kill(struct ring_buffer * const rb, uint8_t * p);

#endif /* _LIBRB_H_ */
