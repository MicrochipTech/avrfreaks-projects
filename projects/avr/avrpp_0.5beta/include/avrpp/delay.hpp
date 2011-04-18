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

#ifndef AVRPP_DELAY_HPP___
#define AVRPP_DELAY_HPP___

#include <avrpp/avrppcfg.hpp>

namespace avrpp {

/**
 * @brief A namespace containing all the stuff related to implementing delays.
 * Keywords: delays, delay policies, types tp define timing policies.
 */
namespace delay {


/**
 * @brief Delay in nanoseconds. Used for timing policies.
 */
template <delay_value_type val> struct ns {
	enum {
		/** @brief delay value */
		value = val
	};
	/** @brief Do the delay using the given delay policy */
	template <typename delay_policy> static void delay () {
		delay_policy::delay_ns(value);
	}
};

/**
 * @brief Delay in microseconds. Used for timing policies.
 */
template <delay_value_type val> struct us {
	enum {
		/** @brief delay value */
		value = val
	};
	/** @brief Do the delay using the given delay policy */
	template <typename delay_policy> static void delay () {
		delay_policy::delay_us(value);
	}
};

/**
 * @brief Delay in milliseconds. Used for timing policies.
 */
template <delay_value_type val> struct ms {
	enum {
		/** @brief delay value */
		value = val
	};
	/** @brief Do the delay using the given delay policy */
	template <typename delay_policy> static void delay () {
		delay_policy::delay_ms(value);
	}
};

/**
 * @brief Delay in seconds. Used for timing policies.
 */
template <delay_value_type val> struct s {
	enum {
		/** @brief delay value */
		value = val
	};
	/** @brief Do the delay using the given delay policy */
	template <typename delay_policy> static void delay () {
		delay_policy::delay_s(value);
	}
};


/**
 * @brief A placeholder policy implementing no real delays.
 */
struct no_delay_policy {
	/** @brief Delay in seconds */
	static void delay_s (delay_value_type /*s*/) {}
	/** @brief Delay in milliseconds */
	static void delay_ms (delay_value_type /*ms*/) {}
	/** @brief Delay in microseconds */
	static void delay_us (delay_value_type /*us*/) {}
	/** @brief Delay in nanoseconds */
	static void delay_ns (delay_value_type /*ns*/) {}
};

}

}
#endif
