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

#ifndef AVRPP_PORTS_HPP___
#define AVRPP_PORTS_HPP___

#ifdef AVRPP_AVR_GCC
#include <avrpp/ports/avr-gcc/ports_impl.hpp>
#else
#ifdef AVRPP_KTEST
// ktest port is built for host system. Testing and debugging use ONLY.
#include <avrpp/ports/ktest/ports_impl.hpp>
#else
#error "AVR++ port is not selected. Please define an appropriate macro for your platform..."
#endif
#endif

#endif //AVRPP_PORTS_HPP___
