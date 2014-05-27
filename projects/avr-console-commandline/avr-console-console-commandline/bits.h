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
#ifndef _BITS_H_
#define _BITS_H_

#define set_bits(a,b)         ((a)=(a)|(b))
#define clear_bits(a,b)       ((a)=(a)&~(b))
#define set_clear_bits(a,b,c) ((a)=((a)|(b))&~(c))
#define set_field_bits(a,b,c) ((a)=((a)&~(b))|((b)&(c))) //a-reg, b-mask, c-value
#define set_bits_cond(a,b,c)  set_field_bits((a),(b),(c)?(b):0)
#define test_bits(a,b)        (((a)&(b))!=0)
#define set_bit(a,b)          set_bits((a),_BV(b))
#define clear_bit(a,b)        clear_bits((a),_BV(b))
#define test_bit(a,b)         test_bits((a),_BV(b))

#endif /* _BITS_H_ */
