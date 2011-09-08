#ifndef _lcd_h_Included_
#define _lcd_h_Included_
/*********************************************************************************
 Copyright (C) 2011 by Stefan Filipek

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*********************************************************************************/

/**
 * @file lcd.h
 *
 * LCD Interface
 */


#include <stdint.h>
#include <savr/gpio.h>


#define LCD_READ_BUSYFLAG           0x80

class LCD {

public:
    /**
     * Initialize the LCD
     *
     * @param sysClock  System clock frequency in Hz, used for initial delays
     * @param d4        GPIO::Pin for DB4
     * @param d5        GPIO::Pin for DB5
     * @param d6        GPIO::Pin for DB6
     * @param d7        GPIO::Pin for DB7
     * @param rs        GPIO::Pin for the RS line
     * @param rw        GPIO::Pin for the R/W line
     * @param e         GPIO::Pin for the Enable line
     */
    LCD(uint32_t sysClock,
        GPIO::Pin d4, GPIO::Pin d5, GPIO::Pin d6, GPIO::Pin d7,
        GPIO::Pin rs, GPIO::Pin rw, GPIO::Pin e) {
        __LCD(sysClock, d4, d5, d6, d7, rs, rw, e);
    }


    /**
     * Turn on/off the cursor
     *
     * @param cursor Boolean true/false (true = show cursor)
     */
    void SetCursor(bool cursor);


    /**
     * Turn on/off the cursor blink
     *
     * @param blink Boolean true/false (true = blink)
     */
    void SetBlink(bool blink);


    /**
     * Turn on/off the entire display
     *
     * @param on Boolean true/false (true = on)
     */
    void SetDisplay(bool on);


    /**
     * Write a full string to the display
     *
     * @param string The null-terminated string to display
     */
    void OutString(const char * string);


    /**
     * Send a raw byte to the display
     *
     * @param byte  The byte to send
     * @param mode  RS line control
     */
    void OutByte(uint8_t byte, uint8_t mode=0);


    /**
     * Send a command to the display
     *
     * @param cmd  The command byte to send
     */
    inline void OutCmd(uint8_t cmd) {
        OutByte(cmd);
    }


    /**
     * Write a single character to the display
     *
     * @param c  The character to write
     */
    inline void OutChar(char c) {
        OutByte(c, 1);
    }


    /**
     * Clear the display, cursor to home position
     */
    inline void Clear(void) {
        OutCmd(0x01);
    }


    /**
     * Set the cursor to the home position
     */
    inline void Home(void) {
        OutCmd(0x02);
    }


    /**
     * Manually set the cursor position
     *
     * @param pos The DDRAM address
     */
    inline void SetPos(uint8_t pos) {
        OutCmd(0x80 | pos);
    }


    /**
     * Manually set the cursor position
     *
     * @return The current cursor position
     */
    inline uint8_t GetPos(void) {
        return _GetByte() & ~LCD_READ_BUSYFLAG;
    }


private:
    void __LCD( uint32_t sysClock,
                GPIO::Pin d4, GPIO::Pin d5, GPIO::Pin d6, GPIO::Pin d7,
                GPIO::Pin rs, GPIO::Pin rw, GPIO::Pin e);


    /**
     * Send a single nibble down with the given mode (RS)
     * @param nib  Nibble (least significant 4 bits)
     * @param mode RS control
     */
    void _OutNib(uint8_t nib, uint8_t mode=0);


    /**
     * Wait for the busy flag to not be set
     */
    void _Wait(void);


    /**
     * Change data direction
     */
    void _SetDataOut(void);


    /**
     * Change data direction
     */
    void _SetDataIn(void);


    /**
     * Read a nibble from the data lines
     */
    uint8_t _ReadDataNibble(void);


    /**
     * Write a nibble to the data lines
     */
    void _SetDataNibble(uint8_t nibble);


    /**
     * Read a byte from the data lines
     *
     * @param mode  RS control. 0 for address/busy flag. 1 for data.
     */
    uint8_t _GetByte(uint8_t mode = 0);


    uint8_t         _entryMode;
    uint8_t         _displayCtrl;
    uint8_t         _displayShift;
    uint8_t         _functionSet;

    GPIO::Pin       _pinD4;
    GPIO::Pin       _pinD5;
    GPIO::Pin       _pinD6;
    GPIO::Pin       _pinD7;

    GPIO::Pin       _pinRW;
    GPIO::Pin       _pinE;
    GPIO::Pin       _pinRS;

};

#endif /* _lcd_h_Included_ */
