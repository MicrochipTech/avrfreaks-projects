//      port.cpp
//      
//      Copyright 2016 Javier Valencia <javiervalencia80@gmail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.
//      
//      

#include "port.hpp"

#if defined(PORTA)
volatile Port Port::A(PORTA, DDRA, PINA);
#endif

#if defined(PORTB)
volatile Port Port::B(PORTB, DDRB, PINB);
#endif

#if defined(PORTC)
volatile Port Port::C(PORTC, DDRC, PINC);
#endif

#if defined(PORTD)
volatile Port Port::D(PORTD, DDRD, PIND);
#endif

#if defined(PORTE)
volatile Port Port::E(PORTE, DDRE, PINE);
#endif

#if defined(PORTF)
volatile Port Port::F(PORTF, DDRF, PINF);
#endif

#if defined(PORTG)
volatile Port Port::G(PORTG, DDRG, PING);
#endif

Port::Port(volatile uint8_t &portReg, volatile uint8_t &ddrReg, volatile uint8_t &pinReg) :
	mPORT(&portReg),
	mDDR(&ddrReg),
	mPIN(&pinReg),
	mActiveLevels(255)
{}

void Port::mode(uint8_t pinMask, uint8_t mode, uint8_t activeLevel) {
	switch(mode) {
		case INPUT:
			*mDDR &= ~pinMask;
			*mPORT &= ~pinMask;
			break;
		case INPUT_PULLUP:
			*mDDR &= ~pinMask;
			*mPORT |= pinMask;
			break;
		case OUTPUT:
			*mDDR |= pinMask;
			break;
	}
	
	switch(activeLevel) {
		case HIGH:
			mActiveLevels |= pinMask;
			break;
		case LOW:
			mActiveLevels &= ~pinMask;
			break;
	}
}

void Port::high(uint8_t pinMask) {
	if ((mActiveLevels & pinMask) == 0) {
		*mPORT &= ~pinMask;
	} else {
		*mPORT |= pinMask;
	}
}

void Port::low(uint8_t pinMask) {
	if ((mActiveLevels & pinMask) == 0) {
		*mPORT |= pinMask;
	} else {
		*mPORT &= ~pinMask;		
	}
}

bool Port::active(uint8_t pinMask) {
	if ((mActiveLevels & pinMask) == 0) {
		return (*mPIN & pinMask) == 0;
	} else {
		return (*mPIN & pinMask) != 0;
	}
}
