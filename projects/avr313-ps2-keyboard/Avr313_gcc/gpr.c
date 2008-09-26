#include <inttypes.h>
#include "gpr.h"

void print_hexbyte(uint8_t i)
{
	uint8_t h, l;

	h = i & 0xF0;			  // High nibble
	h = h >> 4;
	h = h + '0';

	if (h > '9')
		h = h + 7;

	l = (i & 0x0F)+'0';		  // Low nibble
	if (l > '9')
		l = l + 7;

	putchar('0');
	putchar('x');
	putchar(h);
	putchar(l);
}
