/* ********************************************************************************
 *  textdisplay module for use with AVR series microcontrollers from Atmel        *
 ******************************************************************************** */
 
/*
    Copyright (C) 2005  Erik Häggström <xpress@xpress.mine.nu>

    Copyright notice:

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _TEXTDISPLAY_EXTRAS_H_
#define _TEXTDISPLAY_EXTRAS_H_ 1

void     td_putBar(uint8_t value, uint8_t size, uint8_t x, uint8_t y);

#endif
