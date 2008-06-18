/*-------------------------------------------------------------------------
  Example main program for LCD HD44780 that is binded to stdio function

  
  Copyright (c) 2008, Fandi Gunawan <fandigunawan@gmail.com>
   http://fandigunawan.wordpress.com
    
   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (__at your option) any
   later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
   
   In other words, you are welcome to use, share and improve this program.
   You are forbidden to forbid anyone else to use, share and improve
   what you give them.   Help stamp out software-hoarding!  

-------------------------------------------------------------------------*/

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <stdio.h>

#include "hd44780.h"
#include "hd44780_hw.h"


int main()
{
    
    
    FILE lcd_stream = FDEV_SETUP_STREAM(lcd_putc_stream, NULL, _FDEV_SETUP_WRITE);    
    
    stdout = &lcd_stream; //Binded to standard output

    lcd_init(LCD_DISP_ON); // init LCD

    printf_P(PSTR("Line1\nLine2")); //using printf_P for print string in ROM

    // you can also using :
    //fprintf(stdout,"Test");
    //printf("Line1\nLine2"); // print to line 1 and 2

    return 0;
    
}
