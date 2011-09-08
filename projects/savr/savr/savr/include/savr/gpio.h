#ifndef _gpio_h_Included_
#define _gpio_h_Included_
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
 * @file gpio.h
 */

#include <stdint.h>

namespace GPIO {

#define MAKEPINS(x) x##0, x##1, x##2, x##3, x##4, x##5, x##6, x##7



/**
 * GPIO Pin Constants for the compiled system.
 *
 * For each port on the target, there will be a corresponding
 * port and pin definition in the form X# where 'X' is the
 * port letter and '#' is the pin number, 0-7.
 *
 * Examples are GPIO::C3 or GPIO::A0
 */
typedef enum {
#if defined(PORTA)
    MAKEPINS(A),
#endif
#if defined(PORTB)
    MAKEPINS(B),
#endif
#if defined(PORTC)
    MAKEPINS(C),
#endif
#if defined(PORTD)
    MAKEPINS(D),
#endif
#if defined(PORTE)
    MAKEPINS(E),
#endif
#if defined(PORTF)
    MAKEPINS(F),
#endif
#if defined(PORTG)
    MAKEPINS(G),
#endif
#if defined(PORTH)
    MAKEPINS(H),
#endif
#if defined(PORTJ)
    MAKEPINS(J),
#endif
#if defined(PORTK)
    MAKEPINS(K),
#endif
#if defined(PORTL)
    MAKEPINS(L),
#endif
    NONE,   ///< Do not pass into GPIO::*() functions(!!), but use for your own boundary checks
} Pin;


/**
 * Set the pin high or low.
 *
 * @param pin   The GPIO::Pin to control
 * @param set   zero = Low, non-zero = High
 */
void    Set(GPIO::Pin pin, uint8_t set);


/**
 * Get the pin's value from the PIN register
 *
 * @param pin   The GPIO::Pin to read
 * @return 0 if the pin is logic low, 1 if logic high
 */
uint8_t Get(GPIO::Pin pin);


/**
 * Set the pin (PORT register) high
 *
 * @param pin   The GPIO::Pin to control
 */
void    High(GPIO::Pin pin);


/**
 * Set the pin (PORT register) low
 *
 * @param pin   The GPIO::Pin to control
 */
void    Low(GPIO::Pin pin);


/**
 * Set the pin direction to be an input
 *
 * @param pin   The GPIO::Pin to control
 */
void    In(GPIO::Pin pin);


/**
 * Set the pin direction to be an output
 *
 * @param pin   The GPIO::Pin to control
 */
void    Out(GPIO::Pin pin);


}

#endif /* GPIO_H_ */
