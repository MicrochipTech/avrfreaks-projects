/*
 * lanaCOM - 4 bit logic analyzer using COM-port interface
 * Copyright (c) 2008, Dipl.-Ing. Hans-Juergen Heinrichs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <sys/io.h>

/* -------------------------------------------------------------------------------- */
/*  Function name:      _ i n p _ p ()                                              */
/*                      _ i n p ()                                                  */
/*                      _ o u t p ()                                                */
/*                                                                                  */
/*  Description:        I/O register access functions                               */
/* -------------------------------------------------------------------------------- */
int  _inp_p(int p)                  { return inb_p(p); }    // inb_p() reads back port 0x80
int  _inp(int p)                    { return inb(p);   }
void _outp(int p, unsigned char d)  { outb( d, p );    }
