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

#ifndef AVRPP_LOCKING_HPP___
#define AVRPP_LOCKING_HPP___

//#include <avr/io.h>

namespace avrpp {

/**
 * @brief A namespace containing locking-related classes
 */
namespace locking {
/*
struct plain_locking {
	static void lock () {cli();}
	static void unlock () {sei();}
};*/


/**
 * @brief Empty locking policy
 */
template <typename locking_policy> struct guard {
	/** @brief Performs locking */
	guard () {
		locking_policy::lock ();
	}
	/** @brief Releases the lock */
	~guard () {
		locking_policy::unlock ();
	}
};
/**
 * @brief Empty locking policy
 */
struct no_locking {
	/** @brief Performs locking */
	static void lock () {}
	/** @brief Releases the lock */
	static void unlock () {}
};

}

}

#endif //AVRPP_LOCKING_HPP___


