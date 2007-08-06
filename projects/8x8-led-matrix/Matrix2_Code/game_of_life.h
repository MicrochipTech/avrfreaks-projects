/*********************************************
* Author: Leah Buechley
* Filename: game_of_life.h
* Chip: ATMEGA16
* Date: 10/1/2005
* Purpose:
*	This program was written for a wearable LED tank top.
*	More information in game_of_life.c and at: 
*	http://www.cs.colorado.edu/~buechley/diy/diy_tank.html
* Copyright information: http://www.gnu.org/copyleft/gpl.html

Copyright (C) 2005 Leah Buechley

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*********************************************/

//MACROS FOR AVRmega16
#define row0_output DDRB|= _BV(PB2)
#define row1_output DDRB|= _BV(PB1)
#define row2_output DDRA|= _BV(PA0)
#define row3_output DDRA|= _BV(PA1)
#define row4_output DDRA|= _BV(PA2)
#define row5_output DDRA|= _BV(PA3)

#define row0_high PORTB|= _BV(PB2)
#define row1_high PORTB|= _BV(PB1)
#define row2_high PORTA|= _BV(PA0)
#define row3_high PORTA|= _BV(PA1)
#define row4_high PORTA|= _BV(PA2)
#define row5_high PORTA|= _BV(PA3)

#define row0_low PORTB &= ~_BV(PB2)
#define row1_low PORTB &= ~_BV(PB1)
#define row2_low PORTA &= ~_BV(PA0)
#define row3_low PORTA &= ~_BV(PA1)
#define row4_low PORTA &= ~_BV(PA2)
#define row5_low PORTA &= ~_BV(PA3)

#define col0_output DDRA|= _BV(PA4)
#define col1_output DDRA|= _BV(PA5)
#define col2_output DDRA|= _BV(PA6)
#define col3_output DDRA|= _BV(PA7)
#define col4_output DDRC|= _BV(PC1)
#define col5_output DDRC|= _BV(PC7)
#define col6_output DDRC|= _BV(PC6)
#define col7_output DDRD|= _BV(PD2)
#define col8_output DDRD|= _BV(PD1)
#define col9_output DDRD|= _BV(PD4)
#define col10_output DDRD|= _BV(PD3)
#define col11_output DDRB|= _BV(PB7)
#define col12_output DDRB|= _BV(PB6)
#define col13_output DDRB|= _BV(PB4)

#define col0_high PORTA|= _BV(PA4)
#define col1_high PORTA|= _BV(PA5)
#define col2_high PORTA|= _BV(PA6)
#define col3_high PORTA|= _BV(PA7)
#define col4_high PORTC|= _BV(PC1)
#define col5_high PORTC|= _BV(PC7)
#define col6_high PORTC|= _BV(PC6)
#define col7_high PORTD|= _BV(PD2)
#define col8_high PORTD|= _BV(PD1)
#define col9_high PORTD|= _BV(PD4)
#define col10_high PORTD|= _BV(PD3)
#define col11_high PORTB|= _BV(PB7)
#define col12_high PORTB|= _BV(PB6)
#define col13_high PORTB|= _BV(PB4)

#define col0_low PORTA &= ~_BV(PA4)
#define col1_low PORTA &= ~_BV(PA5)
#define col2_low PORTA &= ~_BV(PA6)
#define col3_low PORTA &= ~_BV(PA7)
#define col4_low PORTC &= ~_BV(PC1)
#define col5_low PORTC &= ~_BV(PC7)
#define col6_low PORTC &= ~_BV(PC6)
#define col7_low PORTD &= ~_BV(PD2)
#define col8_low PORTD &= ~_BV(PD1)
#define col9_low PORTD &= ~_BV(PD4)
#define col10_low PORTD &= ~_BV(PD3)
#define col11_low PORTB &= ~_BV(PB7)
#define col12_low PORTB &= ~_BV(PB6)
#define col13_low PORTB &= ~_BV(PB4)

//initialization functions
void initialize_tank_pins_as_output (void);

//basic LED array display functions
void row_column_display(unsigned char i, unsigned char j);
void column_display (unsigned char j);
void all_on (void);
void all_off (void);
void loop_lights (void);

//Game of Life (GOL) cellular automaton functions
void zero_array (void);
void display_array (void);
uint8_t reset_array(void);
void gol (void);
void glider (unsigned char i, unsigned char j);
void blinker (unsigned char i, unsigned char j);
void r_pentomino (unsigned char i, unsigned char j);

