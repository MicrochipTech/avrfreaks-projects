/*
    This file is part of GLCDL-AVR - Copyright 2004 Lorenz Christophe
    christophelorenz magicathere gmail.com
    https://sites.google.com/site/donutscience

    GLCDL-AVR is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    GLCDL-AVR is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    This project is under LPGL, and you are free to donate if you find it usefull.
    https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=W6HV9YP7GK4S8&lc=BE&item_name=Donut%20Science&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted
    
*/
#include <stdlib.h>
#include <avr\io.h>
#include <avr/pgmspace.h>

void LCD_char(uint8_t* fontSet, uint8_t ch,uint8_t scale,uint8_t x,uint8_t y,uint8_t mode); 
void LCD_drawString(uint8_t fontSet, uint8_t* buf, uint8_t is_pstr,uint8_t scale,uint8_t x,uint8_t y,uint8_t mode) ;
