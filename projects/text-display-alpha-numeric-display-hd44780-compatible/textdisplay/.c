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
/*

	Thanks to erafrmu for updating the td_getRowAddr(row) function to work with displays with four lines.

*/
#include <avr/io.h>

#include "textdisplay.conf.h"
#include "textdisplay.h"

#define nop() { asm volatile("nop"::); }
#define _td_delay() { nop(); nop(); nop(); nop(); }

#define _td_set_rs() TEXTDISPLAY_RS_PORT|=(1<<TEXTDISPLAY_RS_P)
#define _td_clr_rs() TEXTDISPLAY_RS_PORT&=~(1<<TEXTDISPLAY_RS_P)

#define _td_set_rw() TEXTDISPLAY_RW_PORT|=(1<<TEXTDISPLAY_RW_P)
#define _td_clr_rw() TEXTDISPLAY_RW_PORT&=~(1<<TEXTDISPLAY_RW_P)

#define _td_set_e() TEXTDISPLAY_E_PORT|=(1<<TEXTDISPLAY_E_P)
#define _td_clr_e() TEXTDISPLAY_E_PORT&=~(1<<TEXTDISPLAY_E_P)
#define _td_pulse_e() { _td_delay(); _td_set_e(); _td_delay(); _td_clr_e(); }
#define _td_inv_pulse_e() { _td_delay(); _td_clr_e(); _td_delay(); _td_set_e(); }

// Common variables
uint8_t _td_display_width;
uint8_t _td_display_height;

// Internal functions prototypes
void _td_set_data_in(void);
void _td_set_data_out(void);
uint8_t _td_get_data(void);
void _td_set_data(uint8_t data);
void _td_wait_while_busy();


void initTextDisplay(uint8_t width,uint8_t height) {
	uint16_t i=0;
	
	/* Set display width and height */
	_td_display_width=width;
	_td_display_height=height;
	
	/* Set RS, RW and E to outputs */
	_td_clr_rs();
	TEXTDISPLAY_RS_DDR|=(1<<TEXTDISPLAY_RS_P);
	_td_clr_rw();
	TEXTDISPLAY_RW_DDR|=(1<<TEXTDISPLAY_RW_P);
	_td_clr_e();
	TEXTDISPLAY_E_DDR|=(1<<TEXTDISPLAY_E_P);
	
	/* Wait more than 15 ms @ 16MHz*/
	for(i=0;i<65535;i++);

#if (TEXTDISPLAY_BUS_WIDTH==8)
	td_functionSet(TD_FUNC_DATA_LENGTH_8);
#endif
	
	/* Wait more than 4.1 ms */
	for(i=0;i<32800;i++);
	
#if (TEXTDISPLAY_BUS_WIDTH==8)
	td_functionSet(TD_FUNC_DATA_LENGTH_8);
#endif
	
	/* Wait more than 100 us */
	for(i=0;i<800;i++);
	
#if (TEXTDISPLAY_BUS_WIDTH==8)
	td_functionSet(TD_FUNC_DATA_LENGTH_8);
#endif
	
	/* Set function */
#if (TEXTDISPLAY_BUS_WIDTH==8)
	td_functionSet(TD_FUNC_DATA_LENGTH_8 | TD_FUNC_DUAL_LINE | TD_FUNC_FONT_5X8);
#else
	td_functionSet(TD_FUNC_DATA_LENGTH_4 | TD_FUNC_DUAL_LINE | TD_FUNC_FONT_5X8);
#endif
	

	/* Turn display off, cursor off and blinking off */
	td_displayControl(TD_CTRL_DISPLAY_OFF | TD_CTRL_CURSOR_OFF | TD_CTRL_BLINKING_OFF);
	
	/* Clear display */
	td_clearDisplay();
	
	/* Set cursor and shift to home */
	td_cursorHome();

	/* Set mode to increment and no shift */
	td_entryModeSet(TD_MODE_INCREMENT | TD_MODE_SHIFT_OFF);
}

void td_putN(uint8_t data, uint8_t count) {
	uint8_t i;
	for(i=0;i<count;i++) td_put(data);
}

uint8_t td_getRowAddr(uint8_t row) {
	if(row==0) return(0x00);
	if(row==1) return(0x40);
	if(row==2) return(0x14);
	if(row==3) return(0x54); 
	return(0x00);
}

void td_putCh(uint8_t ch, uint8_t x, uint8_t y) {
	td_setDDaddress(td_getRowAddr(y)+x);
	td_put(ch);
}

void td_putStr(const uint8_t *str, uint8_t x, uint8_t y) {
	uint8_t i=0;
	while(str[i]!=0) {
		td_putCh(str[i],x+i,y);
		i++;
	}
}

/* Not yet implemented */
uint8_t td_getCh(uint8_t x, uint8_t y) {
	return(0);
}

uint8_t *td_getStr(uint8_t *buf, uint8_t size, uint8_t x, uint8_t y) {
	uint8_t i=0;
	if(size>1) {
		buf[i]=td_getCh(x+i,y);
		while((buf[i]!=0)&&(i<size)) {
			i++;
			buf[i]=td_getCh(x+i,y);
		}
	}
	buf[size-1]=0; /* Make sure string is null terminated */
	return(buf);
}

inline uint8_t td_getWidth(void) {
	return(_td_display_width);
}

inline uint8_t td_getHeight(void) {
	return(_td_display_height);
}

void _td_wait_while_busy() {
	while(_td_io_read(_td_io_read_nop)&TD_READ_BUSY_FLAG);
}

void _td_io_write(uint8_t data, uint8_t flags) {
	// Wait for busy flag
	if((flags&_td_io_write_no_busy)==0)	_td_wait_while_busy();
	
	// Set rs pin
	if(flags&_td_io_write_rs) {
		_td_set_rs();
	} else {
		_td_clr_rs();
	}
	// Clear read pin
	_td_clr_rw();
	// Set data direction to out
	_td_set_data_out();
	
#if (TEXTDISPLAY_BUS_WIDTH==8)
	// Set high and low nibble
	_td_set_data(0x01);
#else
	// Set high nibble
	_td_set_data((data>>4)&0x0F);
	// Write data
	_td_pulse_e();
	// Set low nibble
	_td_set_data(data&0x0F);
#endif
	// Write data
	_td_pulse_e();
}

uint8_t _td_io_read(uint8_t flags) {
	uint8_t tmp;
	
	// Set rs pin
	if(flags&_td_io_read_rs) {
		_td_set_rs();
	} else {
		_td_clr_rs();
	}
	// Set read pin
	_td_set_rw();
	// Set data direction to in
	_td_set_data_in();
	// Set enable
	_td_set_e();
	// Wait for display
	_td_delay();
	
#if (TEXTDISPLAY_BUS_WIDTH==8)
	// Set high and low nibble
	tmp=_td_get_data();
#else
	// Get high nibble
	tmp=(_td_get_data()<<4)&0xF0;
	// Inverse pulse enable
	_td_inv_pulse_e();
	// Wait for display
	_td_delay();
	// Get low nibble
	tmp|=(_td_get_data()&0x0F);
#endif
	// Clear enable
	_td_clr_e();
	
	return(tmp);
}

void _td_set_data_out(void) {
#if defined(TEXTDISPLAY_IO_PORT)
	#if (TEXTDISPLAY_BUS_WIDTH==8)
		TEXTDISPLAY_DATA_DDR=0xFF;
	#else
		TEXTDISPLAY_DATA_DDR=0xF0|TEXTDISPLAY_DATA_DDR;
	#endif
#elif defined(TEXTDISPLAY_IO_PINS)
	#if (TEXTDISPLAY_BUS_WIDTH==8)
		TEXTDISPLAY_DATA_DDR0|=(1<<TEXTDISPLAY_DATA_P0);
		TEXTDISPLAY_DATA_DDR1|=(1<<TEXTDISPLAY_DATA_P1);
		TEXTDISPLAY_DATA_DDR2|=(1<<TEXTDISPLAY_DATA_P2);
		TEXTDISPLAY_DATA_DDR3|=(1<<TEXTDISPLAY_DATA_P3);
	#endif
	TEXTDISPLAY_DATA_DDR4|=(1<<TEXTDISPLAY_DATA_P4);
	TEXTDISPLAY_DATA_DDR5|=(1<<TEXTDISPLAY_DATA_P5);
	TEXTDISPLAY_DATA_DDR6|=(1<<TEXTDISPLAY_DATA_P6);
	TEXTDISPLAY_DATA_DDR7|=(1<<TEXTDISPLAY_DATA_P7);
#endif
}

void _td_set_data_in(void) {
#if defined(TEXTDISPLAY_IO_PORT)
	#if (TEXTDISPLAY_BUS_WIDTH==8)
		TEXTDISPLAY_DATA_DDR=0x00;
	#else
		TEXTDISPLAY_DATA_DDR=0x0F&TEXTDISPLAY_DATA_DDR;
	#endif
#elif defined(TEXTDISPLAY_IO_PINS)
	#if (TEXTDISPLAY_BUS_WIDTH==8)
		TEXTDISPLAY_DATA_DDR0&=~(1<<TEXTDISPLAY_DATA_P0);
		TEXTDISPLAY_DATA_DDR1&=~(1<<TEXTDISPLAY_DATA_P1);
		TEXTDISPLAY_DATA_DDR2&=~(1<<TEXTDISPLAY_DATA_P2);
		TEXTDISPLAY_DATA_DDR3&=~(1<<TEXTDISPLAY_DATA_P3);
	#endif
	TEXTDISPLAY_DATA_DDR4&=~(1<<TEXTDISPLAY_DATA_P4);
	TEXTDISPLAY_DATA_DDR5&=~(1<<TEXTDISPLAY_DATA_P5);
	TEXTDISPLAY_DATA_DDR6&=~(1<<TEXTDISPLAY_DATA_P6);
	TEXTDISPLAY_DATA_DDR7&=~(1<<TEXTDISPLAY_DATA_P7);
#endif
}

void _td_set_data(uint8_t data) {
#if defined(TEXTDISPLAY_IO_PORT)
	#if (TEXTDISPLAY_BUS_WIDTH==8)
		TEXTDISPLAY_DATA_PORT=data;
	#else
		TEXTDISPLAY_DATA_PORT=((data<<4)&0xF0)|(TEXTDISPLAY_DATA_PORT&0x0F);
	#endif
#elif defined(TEXTDISPLAY_IO_PINS)
	#if (TEXTDISPLAY_BUS_WIDTH==8)
		if(data&0x01) TEXTDISPLAY_DATA_PORT0|=(1<<TEXTDISPLAY_DATA_P0); else TEXTDISPLAY_DATA_PORT0&=~(1<<TEXTDISPLAY_DATA_P0);
		if(data&0x02) TEXTDISPLAY_DATA_PORT1|=(1<<TEXTDISPLAY_DATA_P1); else TEXTDISPLAY_DATA_PORT1&=~(1<<TEXTDISPLAY_DATA_P1);
		if(data&0x04) TEXTDISPLAY_DATA_PORT2|=(1<<TEXTDISPLAY_DATA_P2); else TEXTDISPLAY_DATA_PORT2&=~(1<<TEXTDISPLAY_DATA_P2);
		if(data&0x08) TEXTDISPLAY_DATA_PORT3|=(1<<TEXTDISPLAY_DATA_P3); else TEXTDISPLAY_DATA_PORT3&=~(1<<TEXTDISPLAY_DATA_P3);
		if(data&0x10) TEXTDISPLAY_DATA_PORT4|=(1<<TEXTDISPLAY_DATA_P4); else TEXTDISPLAY_DATA_PORT4&=~(1<<TEXTDISPLAY_DATA_P4);
		if(data&0x20) TEXTDISPLAY_DATA_PORT5|=(1<<TEXTDISPLAY_DATA_P5); else TEXTDISPLAY_DATA_PORT5&=~(1<<TEXTDISPLAY_DATA_P5);
		if(data&0x40) TEXTDISPLAY_DATA_PORT6|=(1<<TEXTDISPLAY_DATA_P6); else TEXTDISPLAY_DATA_PORT6&=~(1<<TEXTDISPLAY_DATA_P6);
		if(data&0x80) TEXTDISPLAY_DATA_PORT7|=(1<<TEXTDISPLAY_DATA_P7); else TEXTDISPLAY_DATA_PORT7&=~(1<<TEXTDISPLAY_DATA_P7);
	#else
		if(data&0x01) TEXTDISPLAY_DATA_PORT4|=(1<<TEXTDISPLAY_DATA_P4); else TEXTDISPLAY_DATA_PORT4&=~(1<<TEXTDISPLAY_DATA_P4);
		if(data&0x02) TEXTDISPLAY_DATA_PORT5|=(1<<TEXTDISPLAY_DATA_P5); else TEXTDISPLAY_DATA_PORT5&=~(1<<TEXTDISPLAY_DATA_P5);
		if(data&0x04) TEXTDISPLAY_DATA_PORT6|=(1<<TEXTDISPLAY_DATA_P6); else TEXTDISPLAY_DATA_PORT6&=~(1<<TEXTDISPLAY_DATA_P6);
		if(data&0x08) TEXTDISPLAY_DATA_PORT7|=(1<<TEXTDISPLAY_DATA_P7); else TEXTDISPLAY_DATA_PORT7&=~(1<<TEXTDISPLAY_DATA_P7);
	#endif
#endif
}

uint8_t _td_get_data(void) {
#if defined(TEXTDISPLAY_IO_PORT)
	#if (TEXTDISPLAY_BUS_WIDTH==8)
		return(TEXTDISPLAY_DATA_PIN);
	#else
		return((TEXTDISPLAY_DATA_PIN>>4)&0x0F);
	#endif
#elif defined(TEXTDISPLAY_IO_PINS)
	uint8_t tmp=0;
	#if (TEXTDISPLAY_BUS_WIDTH==8)
		if(TEXTDISPLAY_DATA_PIN0&(1<<TEXTDISPLAY_DATA_P0)) tmp|=0x01;
		if(TEXTDISPLAY_DATA_PIN1&(1<<TEXTDISPLAY_DATA_P1)) tmp|=0x02;
		if(TEXTDISPLAY_DATA_PIN2&(1<<TEXTDISPLAY_DATA_P2)) tmp|=0x04;
		if(TEXTDISPLAY_DATA_PIN3&(1<<TEXTDISPLAY_DATA_P3)) tmp|=0x08;
		if(TEXTDISPLAY_DATA_PIN4&(1<<TEXTDISPLAY_DATA_P4)) tmp|=0x10;
		if(TEXTDISPLAY_DATA_PIN5&(1<<TEXTDISPLAY_DATA_P5)) tmp|=0x20;
		if(TEXTDISPLAY_DATA_PIN6&(1<<TEXTDISPLAY_DATA_P6)) tmp|=0x40;
		if(TEXTDISPLAY_DATA_PIN7&(1<<TEXTDISPLAY_DATA_P7)) tmp|=0x80;
	#else
		if(TEXTDISPLAY_DATA_PIN4&(1<<TEXTDISPLAY_DATA_P4)) tmp|=0x01;
		if(TEXTDISPLAY_DATA_PIN5&(1<<TEXTDISPLAY_DATA_P5)) tmp|=0x02;
		if(TEXTDISPLAY_DATA_PIN6&(1<<TEXTDISPLAY_DATA_P6)) tmp|=0x04;
		if(TEXTDISPLAY_DATA_PIN7&(1<<TEXTDISPLAY_DATA_P7)) tmp|=0x08;
	#endif
	return(tmp);
#endif
}
