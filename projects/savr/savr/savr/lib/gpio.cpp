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
#include <stdio.h>

#include <savr/gpio.h>

typedef volatile uint8_t* PortBank;

#define PORTOF(x)   (*(portBanks[(x)]  ))
#define DDROF(x)    (*(portBanks[(x)]-1))
#define PINOF(x)    (*(portBanks[(x)]-2))


static const PortBank portBanks[] = {
#ifdef PORTA
        &PORTA,
#endif
#ifdef PORTB
        &PORTB,
#endif
#ifdef PORTC
        &PORTC,
#endif
#ifdef PORTD
        &PORTD,
#endif
#ifdef PORTE
        &PORTE,
#endif
#ifdef PORTF
        &PORTF,
#endif
#ifdef PORTG
        &PORTG,
#endif
#ifdef PORTH
        &PORTH,
#endif
#ifdef PORTJ
        &PORTJ,
#endif
#ifdef PORTK
        &PORTK,
#endif
#ifdef PORTL
        &PORTL,
#endif
};


namespace GPIO {

typedef enum {
    OP_GET,
    OP_HIGH,
    OP_LOW,
    OP_IN,
    OP_OUT,
} __OP;


static uint8_t __IOP(GPIO::Pin pin, GPIO::__OP op);


/**
 * @par Implementation notes:
 */
void
Set(GPIO::Pin pin, uint8_t set)
{
    if(set) {
        High(pin);
    }else{
        Low(pin);
    }
}


/**
 * @par Implementation notes:
 */
uint8_t
Get(GPIO::Pin pin)
{
    return __IOP(pin, OP_GET);
}


/**
 * @par Implementation notes:
 */
void
High(GPIO::Pin pin)
{
    __IOP(pin, OP_HIGH);
}


/**
 * @par Implementation notes:
 */
void
Low(GPIO::Pin pin)
{
    __IOP(pin, OP_LOW);
}


/**
 * @par Implementation notes:
 */
void
In(GPIO::Pin pin)
{
    __IOP(pin, OP_IN);
}


/**
 * @par Implementation notes:
 */
void
Out(GPIO::Pin pin)
{
    __IOP(pin, OP_OUT);
}


/**
 * @par Implementation notes:
 * Having a common area to do math and stuff saves about 34 program bytes at the expense of speed
 */
uint8_t
__IOP(GPIO::Pin pin, GPIO::__OP op)
{
    uint8_t _port = pin / 8;
    uint8_t _pin  = _BV(pin % 8);

    switch(op) {
    case OP_GET:
        return ((PINOF(_port) & _pin) ? 1 : 0);

    case OP_HIGH:
        PORTOF(_port) |= _pin;
        break;

    case OP_LOW:
        PORTOF(_port) &= ~_pin;
        break;

    case OP_IN:
        DDROF(_port) &= ~_pin;
        break;

    case OP_OUT:
        DDROF(_port) |= _pin;
        break;
    }
    return 0;
}


}
