//      pin.hpp
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

#ifndef pin_hpp
#define pin_hpp

#include <avr/io.h>
#include <stdint.h>
#include <port/port.hpp>

class Pin {
	public:
		Pin(volatile Port &port, uint8_t pin, uint8_t mode, uint8_t activeLevel = HIGH);
		void mode(uint8_t mode, uint8_t activeLevel = HIGH);
		void high();
		void low();
		bool active() const;
		void toggle();
		
	private:
		volatile Port *mPort;
		uint8_t mPinMask;
};

inline void Pin::mode(uint8_t mode, uint8_t activeLevel) {
	((Port *)mPort)->mode(mPinMask, mode, activeLevel);
}

inline void Pin::high() {
	((Port *)mPort)->high(mPinMask);
}

inline void Pin::low() {
	((Port *)mPort)->low(mPinMask);
}

inline bool Pin::active() const {
	return ((Port *)mPort)->active(mPinMask);
}

inline void Pin::toggle() {
	((Port *)mPort)->toggle(mPinMask);
}

#endif
