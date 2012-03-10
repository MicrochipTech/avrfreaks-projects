/*
 * globals.c
 *
 * Created: 3/7/2012 11:35:33 PM
 *  Author: Wedge
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <asf.h>
#include "FreeRTOS.h"
#include "task.h"
#include "gloabals.h"


static char buf[25];

/* Copyright (C) 1989, 1990, 1991, 1992 Free Software Foundation, Inc.
     Written by James Clark (jjc@jclark.com)

This file is part of groff.

groff is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

groff is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with groff; see the file COPYING.  If not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#define INT_DIGITS 25		/* enough for 64 bit integer */

char *itoa(int i)
{

  /* Room for INT_DIGITS digits, - and '\0' */
  //static char buf[INT_DIGITS + 2];
  char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
  *p = '\0';
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {			/* i < 0 */
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
}

char * cpRightJust( char* data, long from, long to )
{
//	static char str[25];
//	int i;
	// lets clear this bitch
    buf[0] = '\0';

	strcat(buf, ESC);
	strcat(buf, itoa(((to-from)-strlen(data))));
	strcat(buf, "C");
	strcat(buf, data );
	
	return buf;
}
