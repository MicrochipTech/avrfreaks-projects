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

#ifndef AVRPP_BUS_HPP___
#define AVRPP_BUS_HPP___

#include <avrpp/locking.hpp>
#include <avrpp/datatypes.hpp>


namespace avrpp {

/**
 * @brief A namespace containing classes describing logical buses consisting of several logical pins
 *
 */
namespace bus {

/**
 * @brief A template category class used to group logical pins to
 * optimize setting their values.
 *
 * Used as a category in type list categorization.
 */
template <typename bulk_ops, int bit_shift> struct pin_io_grouping_category {
	/** @brief Bulk operations class */
	typedef bulk_ops bulk;
	enum {
		/** @brief Bit shift between source bus and target port */
		shift=bit_shift
	};
};

/** @brief Transformation of a pin to io grouping category */
template <typename pin_class> struct pin_to_io_grouping_transformation {
	/** @brief Category class */
	typedef pin_io_grouping_category <typename pin_class::bulk, pin_class::bit_shift> result;
};

/** @brief Transformation of a pin to update category */
template <typename pin_class> struct pin_to_update_grouping_transformation {
	/** @brief Category class */
	typedef typename pin_class::bulk result;
};

/** @brief Transformation of a pin to config category */
template <typename pin_class> struct pin_to_config_grouping_transformation {
	/** @brief Category class */
	typedef typename pin_class::port result;
};




/**
 * @brief A class describing an output pin inside a bus
 *
 */
template <typename pin_class,int bus_bit_num>
struct bus_pin {
	/** @brief Pin class */
	typedef pin_class pin;
	/** @brief Port class */
	typedef typename pin::port port;
	/** @brief Bulk operations class */
	typedef typename pin::bulk bulk;
	/** @brief Options class */
	typedef typename pin::options options;
	/** @brief Source mask type */
	typedef typename ::avrpp::datatypes::get_enclosing_type_by_bits_num<bus_bit_num+1>::result src_mask_type;
	enum {	/** @brief pin number */
		port_pin = pin::port_pin
	};
	enum {	/** @brief bus bit number */
		bus_bit = bus_bit_num
	};
	enum {	/** @brief source mask */
		src_mask = static_cast<src_mask_type>(1)<<bus_bit
	};
	enum {	/** @brief destination mask */
		dst_mask = static_cast<typename bulk::type>(1)<<port_pin
	};
	enum {	/** @brief bit shift between source and destination bit */
		bit_shift = port_pin-bus_bit
	};
	enum {	/** @brief left bit shift between source and destination bit */
		lshift = (bit_shift>0)?bit_shift:0
	};
	enum {	/** @brief right bit shift between source and destination bit */
		rshift = (bit_shift<0)?-bit_shift:0
	};
};

/**
 * @brief Used as a transformation class when categorizing pins by port
 *
 */
template <typename bus_pin_inst> struct get_port {
	/** @brief The result */
	typedef typename bus_pin_inst::port result;
};

/**
 * @brief Used as a transformation class when categorizing pins by bulk
 *
 */
template <typename bus_pin_inst> struct get_bulk {
	/** @brief The result */
	typedef typename bus_pin_inst::bulk result;
};


/**
 * @brief A wrapper class for a type and number
 *
 */
template <typename cls, int num> struct type_and_number {
	/** @brief Type */
	typedef cls type;
	enum {
		/** @brief number */
		number=num
	};
};

/**
 * @brief Used as a transformation class when categorizing pins by bulk and bit shift
 *
 */
template <typename bus_pin_inst> struct get_bulk_and_shift {
	/** @brief The result */
	typedef type_and_number<typename bus_pin_inst::bulk,
						bus_pin_inst::bit_shift> result;
};


template <typename pin_list> class bus_pin_wrapper;

/**
 * @brief Gets a typelist of pins in a bus (elder ones go first) and
 * converts it into a list of bus_pin <pin,bus_bit> classes.
 *
 * @param  pin_list list of pins (starts with ELDER ones)
 * @return list of bus_pin <pin,bus_bit> classes
 *
 */
template <typename head, typename tail>
class bus_pin_wrapper < ::avrpp::typelist::typelist<head,tail> > {
	typedef ::avrpp::typelist::typelist<head,tail> source_list;
	enum {	/** @brief current bit number */
		bit_num = ::avrpp::typelist::length<source_list>::result - 1
	};
public:
	/** @brief The result */
	typedef ::avrpp::typelist::typelist< bus_pin<head,bit_num>,
				typename bus_pin_wrapper<tail>::result > result;
};

/**
 * @brief Gets a typelist of pins in a bus (elder ones go first) and
 * converts it into a list of bus_pin <pin,bus_bit> classes.
 *
 * @param  pin_list list of pins (starts with ELDER ones)
 * @return list of bus_pin <pin,bus_bit> classes
 *
 */
template <> class bus_pin_wrapper < ::avrpp::typelist::null_list > {
	/** @brief The result */
public:
	typedef ::avrpp::typelist::null_list result;
};

template <typename out_pin_list, typename bus_type>
struct group_base;

/**
 * @brief A class optimizing group operations
 *
 * This class should be instantiated with a list of bus_pin classes, each of which should
 * share the same value of bit_shift.
 *
 */
template <typename head, typename tail, typename bus_data_type>
struct group_base < ::avrpp::typelist::typelist<head,tail>,bus_data_type > {
	/** @brief data type of a port */
	typedef typename head::bulk::type bulk_type;
	/** @brief bus type */
	typedef bus_data_type bus_type;
	/** @brief type for bit math */
	typedef typename ::avrpp::datatypes::get_widest_type< TLIST2(bulk_type,bus_type) >::result math_type;

	enum {	/** @brief destination mask */
		dst_mask = static_cast<math_type> (head::dst_mask) | group_base <tail,bus_data_type>::dst_mask
	};
	enum {	/** @brief source mask */
		src_mask = static_cast<math_type> (head::src_mask) | group_base <tail,bus_data_type>::src_mask
	};
	enum {	/** @brief left shift value */
		lshift = head::lshift
	};
	enum {	/** @brief right shift value */
		rshift = head::rshift
	};
};

/**
 * @brief A class optimizing group operations
 *
 * This class should be instantiated with a list of bus_pin classes, each of which should
 * share the same value of bit_shift.
 *
 */
template <typename bus_type>
struct group_base < ::avrpp::typelist::null_list, bus_type> {
	enum {	/** @brief destination mask */
		dst_mask = 0
	};
	enum {	/** @brief source mask */
		src_mask = 0
	};
};

template <typename pin_list, typename bus_type> struct group_options;

/**
 * @brief A class optimizing setting options for similar pins
 *
 * This class should be instantiated with a list of bus_pin classes, each of which should
 * share the same options type.
 *
 */
template <typename head, typename tail, typename bus_type>
struct group_options < ::avrpp::typelist::typelist<head,tail>, bus_type >
	: group_base < ::avrpp::typelist::typelist<head,tail>, bus_type > {
	typedef group_base < ::avrpp::typelist::typelist<head,tail>, bus_type > base;
	using base::dst_mask;
	/** @brief bulk data type */
	typedef typename base::bulk_type bulk_type;
	/** @brief bulk operations class */
	typedef typename head::options options;
	/** @brief Configure options. */
	static void config_options () {
		options::bulk_config (static_cast<bulk_type>(dst_mask));
	}
};

/**
 * @brief A class optimizing setting options for similar pins
 *
 * This class should be instantiated with a list of bus_pin classes, each of which should
 * share the same options type.
 *
 */
template <typename bus_type>
struct group_options < ::avrpp::typelist::null_list, bus_type>
	: group_base < ::avrpp::typelist::null_list,bus_type> {
	/** @brief Configure options. */
	static void config_options () {}
};

/**
 * @brief A transformation to extract options
 *
 */
template <typename cls> struct get_options_transformation {
	/** @brief The result. */
	typedef typename cls::options result;
};

/**
 * @brief To be used with foreach<> to configure pin options
 *
 */
template <typename cls> struct do_config_options {
	/** @brief Called for each type in the list */
	static void call () {
		cls::config_options ();
	}
};

template <typename out_pin_list, typename bus_type>
struct group_iobase;


/**
 * @brief A class optimizing group I/O
 *
 * This class should be instantiated with a list of bus_pin classes, each of which should
 * share the same type of "bulk" nested typedef and the same value of bit_shift.
 *
 */
template <typename head, typename tail, typename bus_type>
struct group_iobase < ::avrpp::typelist::typelist<head,tail>, bus_type >
	: group_base < ::avrpp::typelist::typelist<head,tail>, bus_type > {
	typedef group_base < ::avrpp::typelist::typelist<head,tail>, bus_type > base;
	typedef ::avrpp::typelist::typelist<head,tail> pin_list;
	using base::dst_mask;
	using base::src_mask;
	using base::lshift;
	using base::rshift;

	/**
	 * @brief Transformation to create group_options objects
	 *
	 */
	template <typename cls> struct make_option_group {
		/** @brief The result */
		typedef group_options<cls,bus_type> result;
	};

	/** @brief bulk data type */
	typedef typename base::bulk_type bulk_type;
	/** @brief bulk data type */
	typedef typename base::math_type math_type;
	/** @brief bulk operations class */
	typedef typename head::bulk bulk;

	/** @brief Categorized by options */
	typedef typename ::avrpp::typelist::categorize_by_transformation
			<pin_list, get_options_transformation>::result option_categories;

	/** @brief Grouped by options */
	typedef typename ::avrpp::typelist::transform<option_categories, make_option_group>::result option_groups;
};
/**
 * @brief A class optimizing group I/O
 *
 * This class should be instantiated with a list of bus_pin classes, each of which should
 * share the same type of "bulk" nested typedef and the same value of bit_shift.
 *
 */
template <typename bus_type>
struct group_iobase < ::avrpp::typelist::null_list, bus_type>
	: group_base < ::avrpp::typelist::null_list,bus_type> {
};




template <typename out_pin_list, typename bus_type>
struct group_output;

/**
 * @brief A class optimizing group output and config
 *
 * This class should be instantiated with a list of bus_pin classes, each of which should
 * share the same type of "bulk" nested typedef and the same value of bit_shift.
 *
 */
template <typename head, typename tail, typename bus_type>
struct group_output < ::avrpp::typelist::typelist<head,tail>, bus_type >
	: group_iobase < ::avrpp::typelist::typelist<head,tail>, bus_type > {
	typedef group_iobase < ::avrpp::typelist::typelist<head,tail>, bus_type > base;
	typedef ::avrpp::typelist::typelist<head,tail> pin_list;
	using base::dst_mask;
	using base::src_mask;
	using base::lshift;
	using base::rshift;

	/** @brief bulk data type */
	typedef typename base::bulk_type bulk_type;
	/** @brief bulk data type */
	typedef typename base::math_type math_type;
	/** @brief bulk operations class */
	typedef typename base::bulk bulk;

	/** @brief Grouped by options */
	typedef typename base::option_groups option_groups;

	/** @brief Output. Gets a byte (bus format) and sets the output bits. */
	static void out (bus_type val) {
		math_type out_val = (static_cast<math_type>(val) & src_mask) << lshift >> rshift;
		bulk::bulk_set_data (static_cast<bulk_type>(dst_mask), static_cast<bulk_type> (out_val));
	}
	/** @brief Gets outputs state. */
	static bus_type get () {
		return static_cast<bus_type> (
				static_cast<math_type>(bulk::bulk_get_outputs (dst_mask)) >> lshift << rshift);
	}
	/** @brief Configure pins. */
	static void config () {
		bulk::bulk_config_output (static_cast<bulk_type>(dst_mask));
		config_options ();
	}
	/** @brief Configure options. */
	static void config_options () {
		::avrpp::typelist::foreach<option_groups, do_config_options>::call ();
	}
};
/**
 * @brief A class optimizing group output
 *
 * This class should be instantiated with a list of bus_pin classes, each of which should
 * have the same type of "bulk" nested typedef and the same value of bit_shift.
 *
 */
template <typename bus_type>
struct group_output < ::avrpp::typelist::null_list, bus_type>
	: group_iobase < ::avrpp::typelist::null_list,bus_type> {
	/** @brief Output. Gets a byte (bus format) and sets the output bits. */
	static void out (bus_type val) {}
	/** @brief Gets outputs state. */
	static bus_type get () {
		return 0;
	}
	/** @brief Configure pins. */
	static void config () {}
};

template <typename out_pin_list, typename bus_type>
struct group_update_outputs;

/**
 * @brief A class optimizing group update
 *
 * This class should be instantiated with a list of bus_pin classes, each of which should
 * share the same type of "port" nested typedef and the same value of bit_shift.
 *
 */
template <typename head, typename tail, typename bus_type>
struct group_update_outputs < ::avrpp::typelist::typelist<head,tail>,bus_type >
	: group_base < ::avrpp::typelist::typelist<head,tail>,bus_type > {
	/** @brief A shortcut for base class */
	typedef group_base < ::avrpp::typelist::typelist<head,tail>,bus_type> base;
	using base::dst_mask;
	using base::src_mask;
	using base::lshift;
	using base::rshift;
	/** @brief Port class */
	typedef typename head::port port;
	/** @brief Update. Updates output values. */
	static void update () {
		port::update_outputs ();
	}
};
/**
 * @brief A class optimizing group update and config
 *
 * This class should be instantiated with a list of bus_pin classes, each of which should
 * have the same type of "port" nested typedef.
 *
 */
template <typename bus_type>
struct group_update_outputs < ::avrpp::typelist::null_list,bus_type>
	: group_base < ::avrpp::typelist::null_list,bus_type> {
	static void update () {}
};


template <typename out_pin_list, typename bus_type>
struct group_input;

/**
 * @brief A class optimizing group output and config
 *
 * This class should be instantiated with a list of bus_pin classes, each of which should
 * share the same type of "bulk" nested typedef and the same value of bit_shift.
 *
 */
template <typename head, typename tail, typename bus_type>
struct group_input < ::avrpp::typelist::typelist<head,tail>,bus_type >
	: group_iobase < ::avrpp::typelist::typelist<head,tail>,bus_type > {
	typedef group_iobase < ::avrpp::typelist::typelist<head,tail>,bus_type > base;
	using base::dst_mask;
	using base::src_mask;
	using base::lshift;
	using base::rshift;
	/** @brief bulk data type */
	typedef typename base::bulk_type bulk_type;
	/** @brief bulk data type */
	typedef typename base::math_type math_type;
	/** @brief bulk operations class */
	typedef typename head::bulk bulk;
	/** @brief Grouped by options */
	typedef typename base::option_groups option_groups;

	/** @brief Get inputs state. Does not update data. */
	static bus_type get  () {
		return static_cast<math_type>(bulk::bulk_input (dst_mask)) >> lshift << rshift;
	}
	/** @brief Configure pins. */
	static void config () {
		bulk::bulk_config_input (static_cast<bulk_type>(dst_mask));
		config_options ();
	}
	/** @brief Configure options. */
	static void config_options () {
		::avrpp::typelist::foreach<option_groups, do_config_options>::call ();
	}
};
/**
 * @brief A class optimizing group output
 *
 * This class should be instantiated with a list of bus_pin classes, each of which should
 * have the same type of "bulk" nested typedef and the same value of bit_shift.
 *
 */
template <typename bus_type>
struct group_input < ::avrpp::typelist::null_list,bus_type>
	: group_iobase < ::avrpp::typelist::null_list,bus_type> {
	/** @brief Get inputs state. Does not update data. */
	static unsigned char get  () {
		return 0;
	}
	/** @brief Configure pins. */
	static void config () {}
};

template <typename out_pin_list, typename bus_type>
struct group_update_inputs;

/**
 * @brief A class optimizing group update
 *
 * This class should be instantiated with a list of bus_pin classes, each of which should
 * share the same type of "port" nested typedef and the same value of bit_shift.
 *
 */
template <typename head, typename tail, typename bus_type>
struct group_update_inputs < ::avrpp::typelist::typelist<head,tail>,bus_type >
	: group_base < ::avrpp::typelist::typelist<head,tail>,bus_type > {
	/** @brief A shortcut for base class */
	typedef group_base < ::avrpp::typelist::typelist<head,tail>,bus_type > base;
	using base::dst_mask;
	using base::src_mask;
	using base::lshift;
	using base::rshift;
	/** @brief Port class */
	typedef typename head::port port;
	/** @brief Update. Updates input values. Call before reading. */
	static void update () {
		port::update_inputs ();
	}
};
/**
 * @brief A class optimizing group update and config
 *
 * This class should be instantiated with a list of bus_pin classes, each of which should
 * have the same type of "port" nested typedef.
 *
 */
template <typename bus_type>
struct group_update_inputs < ::avrpp::typelist::null_list,bus_type>
	: group_base < ::avrpp::typelist::null_list,bus_type> {
	/** @brief Update. Updates input values. Call before reading. */
	static void update () {}
};


/** @brief Performs configuration. To be run by foreach */
template <typename cls> struct do_config {
	/** @brief Call method */
	static void call () {
		cls::config ();
	};
};

/** @brief Performs update. To be run by foreach */
template <typename cls> struct do_update {
	/** @brief Call method */
	static void call () {
		cls::update ();
	};
};

/** @brief Reads out bits. */
template <typename cls> struct do_read_outs {
	/** @brief Call method */
	static typename cls::bus_type call () {
		return cls::get ();
	};
};

/** @brief Performs output. To be run by foreach */
template <typename cls> struct do_out {
	/** @brief Call method */
	static void call (typename cls::bus_type p) {
		cls::out (p);
	};
};

/** @brief Performs output. To be run by foreach */
template <typename cls> struct do_in {
	/** @brief Call method */
	static typename cls::bus_type call () {
		return cls::get ();
	};
};

template <typename class_list, template <typename cls> class processor> struct accumulate_bits;

/** @brief Joins bit masks returned by call () using bitwise OR */
template <typename head, typename tail, template <typename cls> class processor>
struct accumulate_bits < ::avrpp::typelist::typelist<head,tail>,processor > {
	/** @brief Data type for bulk operations */
	typedef typename head::bus_type bus_type;
	/** @brief Joins bit masks returned by call () using bitwise OR */
	static bus_type call () {
		return accumulate_bits <tail,processor>::call () | processor<head>::call ();
	}
};

/** @brief Joins bit masks returned by call () using bitwise OR */
template <template <typename cls> class processor>
struct accumulate_bits < ::avrpp::typelist::null_list,processor > {
	/** @brief Joins bit masks returned by call () using bitwise OR */
	static unsigned char call () {
		return 0;
	}
};

/**
 * @brief Get port type for bulk ops.
 */
template <typename group_cls> struct get_bulk_type {
	/** @brief The result */
	typedef typename group_cls::bulk::type result;
};

/**
 * @brief Joins several physical or virtual output pins into a single logical output bus
 *
 * @param pin_list is a type list containing pin classes; bit 0 goes last
 */
template <typename pin_list, typename locking_policy=::avrpp::locking::no_locking>
struct output_bus {
	/** @brief Bus data type */
	typedef typename ::avrpp::datatypes::get_enclosing_type_by_bits_num<
			::avrpp::typelist::length<pin_list>::result >::result bus_type;

	/** @brief Wrapped pins */
	typedef typename bus_pin_wrapper <pin_list>::result
						wrapped_pins;
	/** @brief Pins categorized by port */
	typedef typename ::avrpp::typelist::categorize_by_transformation <wrapped_pins,
				get_port>::result pins_by_port;
	/** @brief Pins categorized by bulk */
	typedef typename ::avrpp::typelist::categorize_by_transformation <wrapped_pins,
				get_bulk>::result pins_by_bulk;
	/** @brief Pins categorized by bulk and bit shift */
	typedef typename ::avrpp::typelist::categorize_by_transformation <wrapped_pins,
				get_bulk_and_shift>::result pins_by_bulk_and_shift;

	template <typename p_list> struct make_group_update_outputs {
		typedef group_update_outputs<p_list,bus_type> result;
	};

	template <typename p_list> struct make_group_output {
		typedef group_output<p_list,bus_type> result;
	};


	/** @brief Grouped by port for update */
	typedef typename ::avrpp::typelist::transform<pins_by_port, make_group_update_outputs>::result update_groups;
	/** @brief Grouped by bulk ops for config */
	typedef typename ::avrpp::typelist::transform<pins_by_bulk_and_shift, make_group_output>::result config_groups;
	/** @brief Grouped by bulk ops for output */
	typedef typename ::avrpp::typelist::transform<pins_by_bulk_and_shift, make_group_output>::result out_groups;
	/** @brief Data types */
	typedef typename ::avrpp::typelist::transform<out_groups, get_bulk_type>::result data_types;
	/** @brief Widest bulk data type */
	typedef typename ::avrpp::datatypes::get_widest_type<data_types>::result max_type;
	/** @brief Type for bit math */
	typedef typename ::avrpp::datatypes::get_widest_type<TLIST2(max_type,bus_type)>::result math_type;

	/** @brief Configure all the pins */
	static void config () {
		::avrpp::locking::guard<locking_policy> g;
		::avrpp::typelist::foreach<config_groups,do_config>::call ();
	}
	/** @brief Sets all the bits in one operation and updates ports */
	static void out (bus_type data) {
		::avrpp::locking::guard<locking_policy> g;
		::avrpp::typelist::foreach<out_groups,do_out>::call (data);
		::avrpp::typelist::foreach<update_groups,do_update>::call ();
	}
	/** @brief Sets all the bits in one operation, no update is performed */
	static void just_out (bus_type data) {
		::avrpp::locking::guard<locking_policy> g;
		::avrpp::typelist::foreach<out_groups,do_out>::call (data);
	}
	/** @brief Sets all the bits in one operation */
	static bus_type get_outputs () {
		::avrpp::locking::guard<locking_policy> g;
		return accumulate_bits<out_groups,do_read_outs>::call ();
	}
	/** @brief Updates all relevant ports */
	static void update_outputs () {
		::avrpp::locking::guard<locking_policy> g;
		::avrpp::typelist::foreach<update_groups,do_update>::call ();
	}
};

/**
 * @brief Joins several physical or virtual input pins into a single logical input bus
 *
 * @param pin_list is a type list containing pin classes; bit 0 goes last
 */
template <typename pin_list, typename locking_policy=::avrpp::locking::no_locking>
struct input_bus {
	/** @brief Bus data type */
	typedef typename ::avrpp::datatypes::get_enclosing_type_by_bits_num<
			::avrpp::typelist::length<pin_list>::result >::result bus_type;

	/** @brief Wrapped pins */
	typedef typename bus_pin_wrapper <pin_list>::result
						wrapped_pins;
	/** @brief Pins categorized by port */
	typedef typename ::avrpp::typelist::categorize_by_transformation <wrapped_pins,
				get_port>::result pins_by_port;
	/** @brief Pins categorized by bulk */
	typedef typename ::avrpp::typelist::categorize_by_transformation <wrapped_pins,
				get_bulk>::result pins_by_bulk;
	/** @brief Pins categorized by bulk and bit shift */
	typedef typename ::avrpp::typelist::categorize_by_transformation <wrapped_pins,
				get_bulk_and_shift>::result pins_by_bulk_and_shift;

	template <typename p_list> struct make_group_update_inputs {
		typedef group_update_inputs<p_list,bus_type> result;
	};

	template <typename p_list> struct make_group_input {
		typedef group_input<p_list,bus_type> result;
	};


	/** @brief Grouped by port for update */
	typedef typename ::avrpp::typelist::transform<pins_by_port, make_group_update_inputs>::result update_groups;
	/** @brief Grouped by bulk ops for config */
	typedef typename ::avrpp::typelist::transform<pins_by_bulk_and_shift, make_group_input>::result config_groups;
	/** @brief Grouped by bulk ops for input */
	typedef typename ::avrpp::typelist::transform<pins_by_bulk_and_shift, make_group_input>::result in_groups;
	/** @brief Data types */
	typedef typename ::avrpp::typelist::transform<in_groups, get_bulk_type>::result data_types;
	/** @brief Widest bulk data type */
	typedef typename ::avrpp::datatypes::get_widest_type<data_types>::result max_type;
	/** @brief Type for bit math */
	typedef typename ::avrpp::datatypes::get_widest_type<TLIST2(max_type,bus_type)>::result math_type;

	/** @brief Configure all the pins */
	static void config () {
		::avrpp::locking::guard<locking_policy> g;
		::avrpp::typelist::foreach<config_groups,do_config>::call ();
	}
	/** @brief Updates ports and gets all the bits in one operation */
	static bus_type in () {
		::avrpp::locking::guard<locking_policy> g;
		::avrpp::typelist::foreach<update_groups,do_update>::call ();
		return accumulate_bits<in_groups,do_in>::call ();
	}
	/** @brief Gets all the bits in one operation */
	static bus_type just_in () {
		::avrpp::locking::guard<locking_policy> g;
		return accumulate_bits<in_groups,do_in>::call ();
	}
	/** @brief Updates all relevant ports */
	static void update_inputs () {
		::avrpp::locking::guard<locking_policy> g;
		::avrpp::typelist::foreach<update_groups,do_update>::call ();
	}
};


}

}

#endif //AVRPP_BUS_HPP___
