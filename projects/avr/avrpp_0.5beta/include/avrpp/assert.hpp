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

#ifndef AVRPP_ASSERT_HPP___
#define AVRPP_ASSERT_HPP___

/** @brief The AVR++ root namespace
 *
 *  Contains several more nested namespaces.
 */
namespace avrpp {

template <bool val>
struct compile_time_assert;

/** A template class used in compilation time assertion checks.
 * Checks if its parameter is true, otherwise compilation fails.
 */
template <>
struct compile_time_assert <true> {};

template <typename t1, typename t2> struct assert_same_type;

/** A template class used in compilation time assertion checks.
 * Checks if its parameters have the same type, otherwise compilation fails.
 */
template <typename t> struct assert_same_type<t,t>{};

}


/** Does nothing if expr is true, otherwise compilation fails.
 * @param expr An expression to be checked. Should be known at compilation time.
 * @param msg Used to make error messages readable.
 */
#define COMPILE_ASSERT(expr,msg) \
	{avrpp::compile_time_assert<(expr)> ERROR_##msg; (void)ERROR_##msg;} 

/** Does nothing if two types are the same, otherwise compilation fails.
 * @param T1 Type 1
 * @param T2 Type 2
 * @param msg Used to make error messages readable.
 */
#define SAME_TYPE_ASSERT(T1,T2,msg) \
	{avrpp::assert_same_type<T1,T2> ERROR_##msg; (void)ERROR_##msg;} 

#endif //AVRPP_ASSERT_HPP___
