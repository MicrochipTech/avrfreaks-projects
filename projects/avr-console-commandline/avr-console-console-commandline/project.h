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
#ifndef _PROJECT_H_
#define _PROJECT_H_

#include "bits.h"

#ifndef BAUD
#define BAUD (250000L)
#endif

/*
 * initialize console before main
 */
void console_init(void) __attribute__((__constructor__));

/*
 * these functions never return
 */
void main(void) __attribute__((__noreturn__));
void cmdline(void) __attribute__((__noreturn__));

#endif /* _PROJECT_H_ */
