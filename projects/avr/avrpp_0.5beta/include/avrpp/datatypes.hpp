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

#ifndef AVRPP_DATATYPES_HPP___
#define AVRPP_DATATYPES_HPP___

#include <avrpp/typelist.hpp>

#include <avrpp/avrppcfg.hpp>

namespace avrpp {

/** @brief A namespace containing datatypes to keep integer values */
namespace datatypes {

using namespace ::avrpp::config;

template <typename growing_types_list, int size> class get_enclosing_type_by_sizeof;

/** @brief Selects a smallest-sized type to keep given number of chars */
template <typename head, typename tail, int size>
class get_enclosing_type_by_sizeof < ::avrpp::typelist::typelist<head,tail>, size>
{
	enum {
		/** @brief Size of head type */
		head_size = static_cast<int> (sizeof(head))
	};
	/** @brief The same for the tail */
	typedef typename get_enclosing_type_by_sizeof<tail,size >::result tail_result;
public:
	/** @brief The result */
	typedef typename ::avrpp::typelist::arithmetic_if<(size-head_size),head,head,tail_result>::result result;
};

/** @brief Selects a smallest-sized type to keep given number of chars */
template <int size> class get_enclosing_type_by_sizeof < ::avrpp::typelist::null_list,size > {
public:
	/** @brief The result */
	typedef ::avrpp::typelist::null_list result;
};

/** @brief Gives a smallest-sized type to keep given number of bits */
template <int num_bits> struct get_enclosing_type_by_bits_num {
	/** @brief The result */
	typedef typename get_enclosing_type_by_sizeof<bus_and_port_types,
		(num_bits+num_bits_in_char-1)/num_bits_in_char>::result result;
};

template <typename type_list> class get_widest_type;

/** @brief Gives the widest type in a list */
template <typename head, typename tail>
class get_widest_type < ::avrpp::typelist::typelist<head,tail> > {
	/** @brief The widest of types in tail */
	typedef typename get_widest_type<tail>::result tail_result;
	enum {
		/** @brief Difference in size between head and the widest type in tail */
		diff = (static_cast<int> (sizeof(head)) -
		static_cast<int> (sizeof(typename get_widest_type<tail>::result)))
	};
public:
	/** @brief The result */
	typedef typename ::avrpp::typelist::arithmetic_if<diff, tail_result, head, head>::result result;
};

template <typename head>
class get_widest_type < ::avrpp::typelist::typelist<head,::avrpp::typelist::null_list> > {
public:
	/** @brief The result */
	typedef head result;
};

}

}

#endif
