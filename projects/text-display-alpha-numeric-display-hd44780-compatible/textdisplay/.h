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

#ifndef _TEXTDISPLAY_H_
#define _TEXTDISPLAY_H_ 1

/* ********************************************************************************
 *  FLAGS TO FUNCTIONS                                                            *
 ******************************************************************************** */

/* td_entryModeSet(uint8_t mode) */
#define TD_MODE_INCREMENT			0x02
#define TD_MODE_DECREMENT			0x00
#define TD_MODE_SHIFT_ON			0x01
#define TD_MODE_SHIFT_OFF			0x00

/* td_displayControl(uint8_t ctrl) */
#define TD_CTRL_DISPLAY_ON		0x04
#define TD_CTRL_DISPLAY_OFF		0x00
#define TD_CTRL_CURSOR_ON			0x02
#define TD_CTRL_CURSOR_OFF		0x00
#define TD_CTRL_BLINKING_ON		0x01
#define TD_CTRL_BLINKING_OFF	0x00

/* td_cursorDisplayShift(uint8_t options) */
#define TD_DISPLAY_MOVE_CURSOR	0x00
#define TD_DISPLAY_SHIFT_RIGHT	0x08
#define TD_DISPLAY_SHIFT_LEFT		0x09

/* td_functionSet(uint8_t options) */
#define TD_FUNC_DATA_LENGTH_8	0x10
#define TD_FUNC_DATA_LENGTH_4	0x00
#define TD_FUNC_DUAL_LINE			0x08
#define TD_FUNC_SINGLE_LINE		0x00
#define TD_FUNC_FONT_5X10			0x04
#define TD_FUNC_FONT_5X8			0x00

/* td_readBusyFlagAndAddress(void) */
#define TD_READ_BUSY_FLAG			0x80
#define TD_READ_ADDRESS				0x7F

/* ********************************************************************************
 *  FUNCTIONS                                                                     *
 ******************************************************************************** */

/* Defined wrappers for _td_io_write() and _td_io_read() */

#define td_clearDisplay()              _td_io_write(0x01,_td_io_write_nop)
#define td_cursorHome()                _td_io_write(0x02,_td_io_write_nop)
#define td_entryModeSet(mode)          _td_io_write(0x04|((mode)&0x03),_td_io_write_nop)
#define td_displayControl(ctrl)        _td_io_write(0x08|((ctrl)&0x07),_td_io_write_nop)
#define td_cursorDisplayShift(options) _td_io_write(0x10|((options)&0x0F),_td_io_write_nop)
#define td_functionSet(options)        _td_io_write(0x20|((options)&0x1F),_td_io_write_no_busy)
#define td_setCGaddress(address)       _td_io_write(0x40|((address)&0x3F),_td_io_write_nop)
#define td_setDDaddress(address)       _td_io_write(0x80|((address)&0x7F),_td_io_write_nop)
#define td_readBusyFlagAndAddress()    _td_io_read(_td_io_read_nop)
#define td_put(data)                   _td_io_write(data,_td_io_write_rs)
#define td_get()                       _td_io_read(_td_io_read_rs)
#define td_moveCursor(x,y)             _td_io_write(0x80|((td_getRowAddr(y)+x)&0x7F), _td_io_write_nop)

#define td_readBusyFlag()             (_td_io_read(_td_io_read_nop)&TD_READ_BUSY_FLAG)
#define td_waitWhileBusy()            while(_td_io_read(_td_io_read_nop)&TD_READ_BUSY_FLAG)
#define td_readAddress()              (_td_io_read(_td_io_read_nop)&TD_READ_ADDRESS)

#define td_clearRow(row)							{ td_moveCursor(0,row);	td_putN(' ',td_getWidth()); }

/* Old init function */
#define initTextDisplay(width,height) td_init(width,height)

/* Functions */
void							td_init(uint8_t width,uint8_t height);
inline uint8_t 	td_getWidth(void);
inline uint8_t 	td_getHeight(void);
void							td_putN(uint8_t data, uint8_t count);
uint8_t						td_getRowAddr(uint8_t row);
void							td_putCh(uint8_t ch, uint8_t x, uint8_t y);
uint8_t						td_getCh(uint8_t x, uint8_t y);
void							td_putStr(const uint8_t *str, uint8_t x, uint8_t y);
uint8_t						*td_getStr(uint8_t *buf, uint8_t size, uint8_t x, uint8_t y);


/* Low level functions, do not use unless */
/* you know what you are doing            */
#define _td_io_write_nop 0x00
#define _td_io_write_rs 0x01
#define _td_io_write_no_busy 0x02
void _td_io_write(uint8_t data, uint8_t flags);

#define _td_io_read_nop 0x00
#define _td_io_read_rs 0x01
uint8_t _td_io_read(uint8_t flags);

#endif
