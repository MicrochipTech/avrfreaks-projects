/**
 * \file
 *
 * \brief I/O Ports software driver interface.
 *
 * Copyright (c) 2010 Atmel Corporation. All rights reserved.
 *
 * \page License
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
#ifndef IOPORT_H
#define IOPORT_H

#include <compiler.h>


/**
 * \defgroup port_driver_group IO PORT Driver
 *
 * This is a driver implementation for the I/O ports peripheral. The I/O ports
 * peripheral can be found on XMEGA devices and it controls and configures the
 * I/O pins.
 * @{
 */

/**
 * \brief A bitmask representing a set of pins on a port
 *
 * Starting from pin 0 as the LSB, each bit in the mask corresponds to
 * a pin on some port. Each '1' bit includes the corresponding pin in
 * the set.
 */
typedef uint8_t pin_mask_t;

/**
 * \brief A PORT pin
 *
 * This type is used to describe the PORT pins on the part.
 */
typedef uint8_t port_pin_t;

/**
 * \brief Pin configuration flags
 *
 * This is a bitmask containing configuration flags for the pins that shall be
 * configured.
 */
typedef uint16_t port_pin_flags_t;

/**
 * \brief A port id
 *
 * This type is used to describe the port id on the part (0 is PORTA).
 */
typedef uint8_t	port_id_t;

//! \name Input/Output Configuration Flags
//@{
#define IOPORT_DIR_INPUT          (0 << 8) //!< Pin is Input
#define IOPORT_DIR_OUTPUT         (1 << 8) //!< Pin is Output
//@}

//! \name Initial Output State Flags
//@{
#define IOPORT_INIT_LOW           (0 << 9) //!< Initial Ouptput State is Low
#define IOPORT_INIT_HIGH          (1 << 9) //!< Initial Ouptput State is High
//@}

//! \name Input/Sense Configuration Flags
//@{
#define IOPORT_BOTHEDGES          (0 << 0) //!< Sense Both Edges
#define IOPORT_RISING             (1 << 0) //!< Sense Risign Edge
#define IOPORT_FALLING            (2 << 0) //!< Sense Falling Edge
#define IOPORT_LEVEL              (3 << 0) //!< Sense Low Level
#define IOPORT_INPUT_DISABLE      (7 << 0) //!< Input Buffer Disabled
//@}

//! \name Output and Pull Configuration Flags
//@{
#define IOPORT_TOTEM              (0 << 3) //!< Normal push/pull output
#define IOPORT_BUSKEEPER          (1 << 3) //!< Bus Keeper
#define IOPORT_PULL_DOWN          (2 << 3) //!< Pull-Down (when input)
#define IOPORT_PULL_UP            (3 << 3) //!< Pull-Up (when input)
#define IOPORT_WIRED_OR           (4 << 3) //!< Wired OR
#define IOPORT_WIRED_AND          (5 << 3) //!< Wired AND
#define IOPORT_WIRED_OR_PULL_DOWN (6 << 3) //!< Wired OR and Pull-Down
#define IOPORT_WIRED_AND_PULL_UP  (7 << 3) //!< Wired AND and Pull-Up
//@}

//! \name Inverted I/O Configuration Flags
//@{
#define IOPORT_INV_ENABLED        (1 << 6) //!< I/O is Inverted
#define IOPORT_INV_DISABLE        (0 << 6) //!< I/O is Not Inverted
//@}

//! \name Slew Rate Limit Configuration Flags
//@{
#define IOPORT_SRL_ENABLED        (1 << 7) //!< Slew Rate Limit Enabled
#define IOPORT_SRL_DISABLED       (0 << 7) //!< Slew Rate Limit Disabled
//@}


/**
 * \brief Create a PORT pin number
 *
 * This macro creates a PORT pin number from a pin on a port. The PORT pin
 * number can be used with the functions provided from this driver.
 *
 * \param port Port name e.g. PORTA.
 * \param pin Pin number on the port, valid values are 0 to 7.
 */
#define IOPORT_CREATE_PIN(port, pin) ((PORT_##port) + (pin))

/**
 * \brief Create a pointer to a port from a PORT pin number
 *
 * It is assumed that all ports are lined up after PORTA in the memory map like
 * it is described in the XMEGA A manual.
 * Otherwise we have to do a switch case here.
 *
 * \param pin PORT pin number. This number can be generated by the macro
 * CREATE_PORT_PIN.
 * \return Pointer to the port on which the pin is located.
 */
static inline void *ioport_pin_to_port(port_pin_t pin)
{
	// Each port has an offset of 0x20
	return (void *)((uintptr_t)&PORTA + (pin >> 3) * 0x20);
}

/**
 * \brief Create a pointer to a port from a PORT id number
 *
 * It is assumed that all ports are lined up after PORTA in the memory map like
 * it is described in the XMEGA A manual.
 * Otherwise we have to do a switch case here.
 *
 * \param port PORT id number. (PORTA is 0, PORTB is 1 ...)
 * \return Pointer to the port on which the pin is located.
 */
static inline void *ioport_id_pin_to_port(port_id_t port)
{
	// Each port has an offset of 0x20
	return (void *)((uintptr_t)&PORTA + port * 0x20);
}

/**
 * \brief Generate port pin mask form PORT pin number
 *
 * \param pin PORT pin number.
 * \return Pin mask.
 */
static inline pin_mask_t ioport_pin_to_mask(port_pin_t pin)
{
	return 1U << (pin & 0x7);
}

/**
 * \brief Configure the IO PORT pin function for a set of pins on a port
 *
 * \param port Pointer to the port
 * \param pin_mask Mask containing the pins that should be configured
 * \param flags Bitmask of flags specifying additional configuration
 * parameters.
 */
extern void ioport_configure_port_pin(void *port, pin_mask_t pin_mask,
		port_pin_flags_t flags);

/**
 * \brief Select the port function for a single pin
 *
 * \param pin The pin to configure
 * \param flags Bitmask of flags specifying additional configuration
 * parameters.
 */
static inline void ioport_configure_pin(port_pin_t pin, port_pin_flags_t flags)
{
	ioport_configure_port_pin(ioport_pin_to_port(pin), ioport_pin_to_mask(pin), flags);
}


/**
 * \brief Configure a group of I/O pins on a specified port number
 *
 * \param port The port number
 * \param pin_mask The pin mask to configure
 * \param flags Bitmask of flags specifying additional configuration
 * parameters.
 */
static inline void ioport_configure_group(port_id_t port, pin_mask_t pin_mask, port_pin_flags_t flags)
{
	ioport_configure_port_pin(ioport_id_pin_to_port(port), pin_mask, flags);
}


/**
 * \internal
 * \name PORT Pin Numbering
 *
 * These macros are used to generate PORT pin numbers for each port with the
 * CREATE_PORT_PIN macro. Each port has 8 pins so e.g. the first pin on PORTB
 * gets number 8, first pin on PORTC gets 16 ...
 */
//@{
#define PORT_PORTA        (0 * 8)
#define PORT_PORTB        (1 * 8)
#define PORT_PORTC        (2 * 8)
#define PORT_PORTD        (3 * 8)
#define PORT_PORTE        (4 * 8)
#define PORT_PORTF        (5 * 8)
#define PORT_PORTG        (6 * 8)
#define PORT_PORTH        (7 * 8)
#define PORT_PORTJ        (8 * 8)
#define PORT_PORTK        (9 * 8)
#define PORT_PORTL        (10 * 8)
#define PORT_PORTM        (11 * 8)
#define PORT_PORTN        (12 * 8)
#define PORT_PORTP        (13 * 8)
#define PORT_PORTQ        (14 * 8)
#define PORT_PORTR        (15 * 8)
//@}

/**
 * \internal
 * \name PORT fields struture offset
 *
 * These macros are used to compute the field offset nummber with the PORT_t struture.
 */
//@{
#define PORT_DIR                0x00  //!< Data Direction
#define PORT_DIRSET             0x01  //!< Data Direction Set
#define PORT_DIRCLR             0x02  //!< Data Direction Clear
#define PORT_DIRTGL             0x03  //!< Data Direction Toggle
#define PORT_OUT                0x04  //!< Data Output Value
#define PORT_OUTSET             0x05  //!< Data Output Value Set
#define PORT_OUTCLR             0x06  //!< Data Output Value Clear
#define PORT_OUTTGL             0x07  //!< Data Output Value Toggle
#define PORT_IN                 0x08  //!< Data Input Value
#define PORT_INTCTRL            0x09  //!< Interrupt Control
#define PORT_INT0MASK           0x0A  //!< Interrupt 0 Mask
#define PORT_INT1MASK           0x0B  //!< Interrupt 1 Mask
#define PORT_INTFLAGS           0x0C  //!< Interrupt Flags
#define PORT_PIN0CTRL           0x10  //!< Pin 0 Configuration
#define PORT_PIN1CTRL           0x11  //!< Pin 1 Configuration
#define PORT_PIN2CTRL           0x12  //!< Pin 2 Configuration
#define PORT_PIN3CTRL           0x13  //!< Pin 3 Configuration
#define PORT_PIN4CTRL           0x14  //!< Pin 4 Configuration
#define PORT_PIN5CTRL           0x15  //!< Pin 5 Configuration
#define PORT_PIN6CTRL           0x16  //!< Pin 6 Configuration
#define PORT_PIN7CTRL           0x17  //!< Pin 7 Configuration
//@}


/**
 * \brief Drive a PORT pin to a given state
 *
 * This function will only have an effect if \a pin is configured as
 * an output.
 *
 * \param pin A number identifying the pin to act on.
 * \param value The desired state of the pin. \a true means drive the
 * pin high (towards Vdd), while \a false means drive the pin low
 * (towards Vss).
 */
static inline void ioport_set_value(port_pin_t pin, bool value)
{
	PORT_t *port = ioport_pin_to_port(pin);
	if (value)
		port->OUTSET=ioport_pin_to_mask(pin);
	else
		port->OUTCLR=ioport_pin_to_mask(pin);
}

/**
 * \brief Drive a PORT pin to a low level
 *
 * This function will only have an effect if \a pin is configured as
 * an output.
 *
 * \param pin A number identifying the pin to act on.
 */
static inline void ioport_set_pin_low(port_pin_t pin)
{
	PORT_t *port = ioport_pin_to_port(pin);
	port->OUTCLR=ioport_pin_to_mask(pin);
}

/**
 * \brief Drive a PORT pin to a high level
 *
 * This function will only have an effect if \a pin is configured as
 * an output.
 *
 * \param pin A number identifying the pin to act on.
 */
static inline void ioport_set_pin_high(port_pin_t pin)
{
	PORT_t *port = ioport_pin_to_port(pin);
	port->OUTSET=ioport_pin_to_mask(pin);
}

/**
 * \brief Read the current state of a PORT pin
 *
 * \param pin A number identifying the pin to read.
 * \retval true The pin is currently high (close to Vdd)
 * \retval false The pin is currently low (close to Vss)
 */
static inline bool ioport_get_value(port_pin_t pin)
{
	PORT_t *port = ioport_pin_to_port(pin);
	return port->IN&ioport_pin_to_mask(pin);
}

/**
 * \brief Read the current state of a PORT pin and test high level
 *
 * \param pin A number identifying the pin to read.
 * \retval true The pin is currently high (close to Vdd)
 * \retval false The pin is currently low (close to Vss)
 */
static inline bool ioport_pin_is_high(port_pin_t pin)
{
	PORT_t *port = ioport_pin_to_port(pin);
	return port->IN&ioport_pin_to_mask(pin);
}

/**
 * \brief Read the current state of a PORT pin and test high level
 *
 * \param pin A number identifying the pin to read.
 * \retval true The pin is currently high (close to Vdd)
 * \retval false The pin is currently low (close to Vss)
 */
static inline bool ioport_pin_is_low(port_pin_t pin)
{
	PORT_t *port = ioport_pin_to_port(pin);
	return (~port->IN&ioport_pin_to_mask(pin));
}
/**
 * \brief Toggle the current state of a PORT pin
 *
 * \param pin A number identifying the pin to act on.
 */
static inline void ioport_toggle_pin(port_pin_t pin)
{
	pin_mask_t pin_mask = ioport_pin_to_mask(pin);
	PORT_t *port = ioport_pin_to_port(pin);
	port->OUTTGL = pin_mask;
}

/*! \brief Drives a group of I/O pin of a port to high level.
 *
 * \param port_id The port number.
 * \param port_mask The mask.
 */
static inline void ioport_set_group_high(port_id_t port_id,pin_mask_t port_mask)
{
	PORT_t *port  = ioport_id_pin_to_port(port_id);
	port->OUTSET = port_mask;
}

/*! \brief Drives a group of I/O pin of a port to low level.
 *
 * \param port_id The port number.
 * \param port_mask The mask.
 */
static inline void ioport_set_group_low(port_id_t port_id,pin_mask_t port_mask)
{
	PORT_t *port  = ioport_id_pin_to_port(port_id);
	port->OUTCLR = port_mask;
}

/*! \brief Toggles a group of I/O pin of a port.
 *
 * \param port_id The port number.
 * \param port_mask The mask.
 */
static inline void ioport_tgl_group(port_id_t port_id,pin_mask_t port_mask)
{
	PORT_t *port  = ioport_id_pin_to_port(port_id);
	port->OUTTGL = port_mask;
}

/*@}*/

#endif /* IOPORT_H */
