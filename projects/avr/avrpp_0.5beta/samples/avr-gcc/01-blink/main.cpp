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

/**
 * The following demo is an example of working with MCU GPIO ports.
 * AVR++ contains a step-by-step tutorial containing of such demos.
 * Every demo usually is an extension of previous ones. All the features
 * introduced in a demo are commented. This is the very first demo, so
 * all the features are new, and the code is fully commented.
 *
 * NOTE: A comment explaining a line of code usually goes BEFORE the line.
 *
 * HARDWARE REQUIREMENTS: an AVR (e.g. ATmega16) with a LED connected to PA0.
 * Do not forget a resistor to limit output current. Value of about 200-500
 * ohms probably is ok.
 *
 * EMULATED RUN: open the VMLAB project. VMLAB is free, and runs even on
 * Linux using WINE. All the necessary outputs are added to scope.
 */

/**
 * First of all AVR++ requires some configuration. You may define the
 * necessary macros either here or from makefile.
 */

/**
 * The following #define tells AVR++ to use config files specific to avr-gcc
 * version. It is necessary.
 */
#define AVRPP_AVR_GCC

/**
 * Uncomment to use avr-libc delay implementation. Personally I prefer AVR++
 * default implementation. At least it is tolerant to -O0 and never generates
 * a lot of unneeded code.
 */
//#define AVRPP_AVRLIBC_DELAY_POLICY

/** A header file containing I/O ports code */
#include <avrpp/ports.hpp>
/** A header file containing delay-related stuff */
#include <avrpp/delay.hpp>

/**
 *  The line is optional. You may use "::avrpp::ports" directly. However,
 *  pin declarations would look much more verbose:
 *    typedef ::avrpp::ports::output< ::avrpp::ports::port_a,0 > led_pin;
 */
using namespace ::avrpp::ports;

/**
 * PORTA, bit 0, i.e. PA0. Should be configured as output (a little further).
 * Important: we do not create (and we are not going to create!) any objects,
 * we just declare a class. All operations on pin are performed using static
 * member functions of the class.
 *
 * NOTE:
 * C++ is usually associated with object-oriented programming, but AVR++ is
 * NOT an object-oriented library. In most cases it just uses advanced C++
 * features to generate configurable procedure-style code. Configuration is
 * performed using template parameters.
 *
 * So, PA0 is declared as output and named "led_pin"...
 */
typedef output<port_a,0> led_pin;

int main () {
	/**
	 * Configuring. We have to set DDRA bit 0 to 1.
	 * Fortunately, the led_pin class knows how to do that.
	*/
	led_pin::config ();

	while(1) {
		/** Set output to high */
		led_pin::set (true);
		/** Using a namespace implementing delay functions */
		using namespace ::avrpp::delay;
		/**
		 * The easiest way to make a delay. However, not the only one.
		 */
		default_delay_policy::delay_ms(300);
		/** Set output to low */
		led_pin::set (false);
		/**
		 * Another way to make a delay. Used widely in timing policies
		 * to create library code that can be easily configured.
		 * It allows separate definition of time interval (ms<500> means
		 * 500ms) and so called "delay policy". Delay policy defines method
		 * of implementing delays independently from delay value (e.g. it may
		 * call avrlibc functions _delay_ms and _delay_us or implement the
		 * delay code from scratch).
		 *
		 * Usually this way is used in conjunction with "timing policies",
		 * though it is not what we are going to discuss right now.
		 *
		 */
		ms<300>::delay<default_delay_policy> ();
	}
	return 0; //Just to avoid a warning in some configurations
}
