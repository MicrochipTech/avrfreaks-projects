/* copyright
 * (c) 2004 Stephen Humble stephenhumble at telstra.com
 *
 *  This file is part of robotx see http://Robotx.sourceforge.net
 *  dual sensorless motor driver firmware.
 *
 *  This is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
*/

#ifndef _STRING_H_
#define _STRING_H_

#include <io.h>

void equal(char *t);

/*************************************************************************
 *
 * String and number output functions
 *
 */
extern void
puts(
	const char *		s
);


extern void
put_hexdigit(
	uint8_t			i
);


extern void
put_uint8_t(
	uint8_t			i
);


extern void
put_uint12_t(
	uint16_t		i
);


extern void
put_uint16_t(
	uint16_t		i
);

#endif
