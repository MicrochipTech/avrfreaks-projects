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

#ifndef AVRPP_PORTS_KTEST_CONFIG_HPP___
#define AVRPP_PORTS_KTEST_CONFIG_HPP___

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

struct no_delay_policy;
/** @brief Select no_delay_policy as the default policy */
typedef no_delay_policy default_delay_policy;

}

}

#endif //AVRPP_PORTS_KTEST_CONFIG_HPP___
