/*
   Copyright 2011 Elijah M. Merkin aka Ellioh

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef AVRPP_PORTS_AVR_GCC_CONFIG_HPP___
#define AVRPP_PORTS_AVR_GCC_CONFIG_HPP___

#include <avr/io.h>
#ifdef AVRPP_AVRLIBC_DELAY_POLICY
#include <util/delay.h>
#endif //AVRPP_AVRLIBC_DELAY_POLICY

#include <avrpp/typelist.hpp>

namespace avrpp {

namespace config {

enum {
	/** @brief According to the C++ standard, sizeof(char)==1,
	 * but char is not guaranteed to be equal to byte
	 */
	num_bits_in_char = 8
};

/** @brief List of types used in ports and buses */
typedef TLIST3(unsigned char, unsigned int, unsigned long) bus_and_port_types;

}

namespace delay
{

/** @brief Type used to specify delays (in ms, us, ns...) */
typedef unsigned long delay_value_type;

#ifdef AVRPP_AVRLIBC_DELAY_POLICY

struct default_delay_policy {
	/** @brief Delay in seconds */
	static void delay_s (delay_value_type s) {
		for (delay_value_type sc = s; sc > 0; sc-- )
			_delay_ms(1000);
	}
	/** @brief Delay in milliseconds */
	static void delay_ms (delay_value_type ms) {
		_delay_ms(ms);
	}
	/** @brief Delay in microseconds */
	static void delay_us (delay_value_type us) {
		_delay_us(us);
	}
	/** @brief Delay in nanoseconds */
	static void delay_ns (delay_value_type ns) {
		if (ns > 1000)
			delay_us(ns/1000);
		else
			delay_us(1);
	}
};

#else //AVRPP_AVRLIBC_DELAY_POLICY

/**
 * @brief Default_delay_policy. Not very accurate, but, unlike the
 * avrlibc version. requires no optimizations to run.
 */
class default_delay_policy {
	/**
	 * @brief Repeat 4-cycle delay given number of times.
	 */
	static inline void delay_4_cycles(unsigned count) {
		if ( count == 0 )
			__asm__ __volatile__( "rjmp 1f\n 1:" );    // 2 cycles
		else
			__asm__ __volatile__ (
					"1: sbiw %0,1" "\n\t"
					"brne 1b"                              // 4 cycles/loop
					: "=w" (count)
					: "0" (count)
			);
	}
public:
	/** @brief Delay in seconds */
	static void delay_s (delay_value_type s) {
		for (delay_value_type sc = s; sc > 0; sc-- )
			delay_ms(1000);
	}
	/** @brief Delay in milliseconds */
	static void delay_ms (delay_value_type ms) {
		for (delay_value_type msc = ms; msc > 0; msc-- )
			delay_us(1000);
	}
	/** @brief Delay in microseconds */
	static void delay_us (delay_value_type us) {
		delay_4_cycles (((1*(F_CPU/4000))*us)/1000 );
	}
	/** @brief Delay in nanoseconds */
	static void delay_ns (delay_value_type ns) {
		if (ns > 1000)
			delay_us(ns/1000);
		else
			delay_us(1);
	}
};

#endif //AVRPP_AVRLIBC_DELAY_POLICY

}

}

#endif //AVRPP_PORTS_AVR_GCC_CONFIG_HPP___
