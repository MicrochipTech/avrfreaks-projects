//      port.hpp
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

#ifndef port_hpp
#define port_hpp

#include <avr/io.h>
#include <stdint.h>

enum { LOW, HIGH };
enum { INPUT, INPUT_PULLUP, OUTPUT };

class Port {
	public:
		#if defined(PORTA)
		static volatile Port A;
		#endif

		#if defined(PORTB)
		static volatile Port B;
		#endif

		#if defined(PORTC)
		static volatile Port C;
		#endif

		#if defined(PORTD)
		static volatile Port D;
		#endif

		#if defined(PORTE)
		static volatile Port E;
		#endif

		#if defined(PORTF)
		static volatile Port F;
		#endif

		#if defined(PORTG)
		static volatile Port G;
		#endif
		
		Port(volatile uint8_t &portReg, volatile uint8_t &ddrReg, volatile uint8_t &pinReg);
		void mode(uint8_t pinMask, uint8_t mode, uint8_t activeLevel);
		void high(uint8_t pinMask);
		void low(uint8_t pinMask);
		bool active(uint8_t pinMask);
		void toggle(uint8_t pinMask);
		
	private:
		volatile uint8_t *mPORT;
		volatile uint8_t *mDDR;
		volatile uint8_t *mPIN;
		uint8_t mActiveLevels;
};

inline void Port::toggle(uint8_t pinMask) {
	active(pinMask) ? low(pinMask) : high(pinMask);
}

#endif
