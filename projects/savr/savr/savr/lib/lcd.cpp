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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay_basic.h>
#include <stdio.h>

#include <savr/gpio.h>
#include <savr/lcd.h>


#define LCD_ENTRYMODE_DISPLAYSHIFT_ON   _BV(0)
#define LCD_ENTRYMODE_DISPLAYSHIFT_OFF  0
#define LCD_ENTRYMODE_INC               _BV(1)
#define LCD_ENTRYMODE_DEC               0
#define LCD_ENTRYMODE                   _BV(2)

#define LCD_DISPLAY_BLINK_ON            _BV(0)
#define LCD_DISPLAY_BLINK_OFF           0
#define LCD_DISPLAY_CURSOR_ON           _BV(1)
#define LCD_DISPLAY_CURSOR_OFF          0
#define LCD_DISPLAY_DISPLAY_ON          _BV(2)
#define LCD_DISPLAY_DISPLAY_OFF         0
#define LCD_DISPLAY                     _BV(3)

#define LCD_SHIFT_LEFT                  _BV(2)
#define LCD_SHIFT_RIGHT                 0
#define LCD_SHIFT_SCREEN                _BV(2)
#define LCD_SHIFT_CURSOR                0
#define LCD_SHIFT                       _BV(4)

#define LCD_FUNCTION_5x8                _BV(2)
#define LCD_FUNCTION_5x11               0
#define LCD_FUNCTION_2LINE              _BV(3)
#define LCD_FUNCTION_1LINE              0
#define LCD_FUNCTION_8BIT               _BV(4)
#define LCD_FUNCTION_4BIT               0
#define LCD_FUNCTION                    _BV(5)



#define MIN(x, y) ((x) < (y) ? (x) : (y))


/**
 * @par Implementation notes:
 * Sloppy and slow
 */
static void
MsDelay(uint32_t sysClock, double ms)
{
    static const uint16_t maxDelayLoops = 65535;

    // How many 4-cycle loops to delay?
    uint32_t loops = ms * (0.001 / (4.0 / sysClock));

    while(loops) {
        uint16_t delay = MIN(loops, maxDelayLoops);
        loops -= delay;
        _delay_loop_2(delay);
    }
}


/**
 * @par Implementation notes:
 */
void
LCD::_SetDataOut(void)
{
    GPIO::Low(_pinD4);
    GPIO::Low(_pinD5);
    GPIO::Low(_pinD6);
    GPIO::Low(_pinD7);
    GPIO::Out(_pinD4);
    GPIO::Out(_pinD5);
    GPIO::Out(_pinD6);
    GPIO::Out(_pinD7);
}


/**
 * @par Implementation notes:
 */
void
LCD::_SetDataIn(void)
{
    GPIO::Low(_pinD4);
    GPIO::Low(_pinD5);
    GPIO::Low(_pinD6);
    GPIO::Low(_pinD7);
    GPIO::In(_pinD4);
    GPIO::In(_pinD5);
    GPIO::In(_pinD6);
    GPIO::In(_pinD7);
}


/**
 * @par Implementation notes:
 */
uint8_t
LCD::_ReadDataNibble(void)
{
    uint8_t ret = GPIO::Get(_pinD4);
    ret |= GPIO::Get(_pinD5) << 1;
    ret |= GPIO::Get(_pinD6) << 2;
    ret |= GPIO::Get(_pinD7) << 3;
    return ret;
}


/**
 * @par Implementation notes:
 */
void
LCD::_SetDataNibble(uint8_t nibble)
{
    GPIO::Set(_pinD4, nibble & _BV(0));
    GPIO::Set(_pinD5, nibble & _BV(1));
    GPIO::Set(_pinD6, nibble & _BV(2));
    GPIO::Set(_pinD7, nibble & _BV(3));
}



/**
 * @par Implementation notes:
 */
void
LCD::__LCD( uint32_t sysClock,
            GPIO::Pin d4, GPIO::Pin d5, GPIO::Pin d6, GPIO::Pin d7,
            GPIO::Pin rs, GPIO::Pin rw, GPIO::Pin e)
{

    _pinD4 = d4;
    _pinD5 = d5;
    _pinD6 = d6;
    _pinD7 = d7;
    _pinRS = rs;
    _pinRW = rw;
    _pinE  = e;

    GPIO::Out(_pinE);
    GPIO::Out(_pinRS);
    GPIO::Out(_pinRW);
    GPIO::Low(_pinE);
    GPIO::Low(_pinRS);
    GPIO::Low(_pinRW);

    _SetDataOut();

    // Init for 4 data lines...
    MsDelay(sysClock, 50); // Power-on delay
    _OutNib(0x03);
    MsDelay(sysClock, 5);
    _OutNib(0x03);
    MsDelay(sysClock, 5);
    _OutNib(0x03);

    _OutNib(0x02);
    _Wait();

    _functionSet    = LCD_FUNCTION  | LCD_FUNCTION_2LINE | LCD_FUNCTION_5x8 | LCD_FUNCTION_4BIT;
    _entryMode      = LCD_ENTRYMODE | LCD_ENTRYMODE_INC | LCD_ENTRYMODE_DISPLAYSHIFT_OFF;
    _displayCtrl    = LCD_DISPLAY   | LCD_DISPLAY_DISPLAY_ON | LCD_DISPLAY_BLINK_OFF | LCD_DISPLAY_CURSOR_OFF;

    OutCmd(_functionSet);
    OutCmd(_entryMode);
    OutCmd(_displayCtrl);
    OutCmd(_displayShift);
    Clear();

}


/**
 * @par Implementation notes:
 */
void
LCD::SetCursor(bool cursor)
{
    if(cursor) {
        _displayCtrl &= ~LCD_DISPLAY_CURSOR_OFF;
        _displayCtrl |=  LCD_DISPLAY_CURSOR_ON;
    }else{
        _displayCtrl &= ~LCD_DISPLAY_CURSOR_ON;
        _displayCtrl |=  LCD_DISPLAY_CURSOR_OFF;
    }
    OutCmd(_displayCtrl);
}


/**
 * @par Implementation notes:
 */
void
LCD::SetBlink(bool blink)
{
    if(blink) {
        _displayCtrl &= ~LCD_DISPLAY_BLINK_OFF;
        _displayCtrl |=  LCD_DISPLAY_BLINK_ON;
    }else{
        _displayCtrl &= ~LCD_DISPLAY_BLINK_ON;
        _displayCtrl |=  LCD_DISPLAY_BLINK_OFF;
    }
    OutCmd(_displayCtrl);
}


/**
 * @par Implementation notes:
 */
void
LCD::SetDisplay(bool on)
{
    if(on) {
        _displayCtrl &= ~LCD_DISPLAY_DISPLAY_OFF;
        _displayCtrl |=  LCD_DISPLAY_DISPLAY_ON;
    }else{
        _displayCtrl &= ~LCD_DISPLAY_DISPLAY_ON;
        _displayCtrl |=  LCD_DISPLAY_DISPLAY_OFF;
    }
    OutCmd(_displayCtrl);
}


/**
 * @par Implementation notes:
 */
uint8_t
LCD::_GetByte(uint8_t mode)
{
    uint8_t x = 0;

    _SetDataIn();

    GPIO::High(_pinRW);
    if(mode) GPIO::High(_pinRS);


    GPIO::High(_pinE);
    x = _ReadDataNibble() << 4;
    GPIO::Low(_pinE);

    // GPIO:X function call is enough delay...

    GPIO::High(_pinE);
    x |= _ReadDataNibble();
    GPIO::Low(_pinE);

    GPIO::Low(_pinRW);
    GPIO::Low(_pinRS);

    _SetDataOut();

    return x;
}


/**
 * @par Implementation notes:
 */
void
LCD::OutByte(uint8_t byte, uint8_t mode)
{
    _Wait();
    _OutNib(byte >> 4, mode);
    _OutNib(byte,      mode);
}


/**
 * @par Implementation notes:
 */
void
LCD::OutString(const char * string)
{
    while(*string)
        OutChar(*string++);
}


/**
 * @par Implementation notes:
 */
void
LCD::_Wait(void)
{
    while(_GetByte() & LCD_READ_BUSYFLAG) ;
}


/**
 * @par Implementation notes:
 */
void
LCD::_OutNib(uint8_t nib, uint8_t mode)
{
    GPIO::High(_pinE);

    _SetDataNibble(nib);
    if (mode) GPIO::High(_pinRS);

    GPIO::Low(_pinE);
    GPIO::Low(_pinRS);
}

