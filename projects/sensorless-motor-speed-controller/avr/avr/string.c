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

#include "uart.h"

// formatted display for printing varables
void equal(char *t)
{
   putc(' ');
   puts(t);
   putc('=');
}

void   puts(   const char *s  )
{
   syncwait();
   while( *s ){
		 putc( *s++ );
	      }
}

void  put_hexdigit(   uint8_t	i  )
{
   if( i < 0x0A )
      i += '0';
   else
      i += 'A' - 0x0A;
   putc( i );
}

void   put_uint8_t(   uint8_t	i   )
{
   put_hexdigit( (i >> 4) & 0x0F );
   put_hexdigit( (i >> 0) & 0x0F );
}
/*
void
   put_uint12_t(
   uint16_t		i
   )
{
   put_hexdigit( (i >> 8) & 0x0F );
   put_uint8_t(  (i >> 0) & 0xFF );
}
*/
/*
void
   put_uint16_t(
   uint16_t		i
   )
{
   put_uint8_t(  (i >> 8) & 0xFF );
   put_uint8_t(  (i >> 0) & 0xFF );
}
*/

