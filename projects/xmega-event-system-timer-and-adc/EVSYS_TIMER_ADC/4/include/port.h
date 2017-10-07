/**
 * \file
 *
 * \brief Port
 *
 *
 * Copyright (C) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 *
 */

#ifndef _PORT_INCLUDED_
#define _PORT_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

enum port_output_pull_mode {
	PORT_CONFIGURATION_TOTEM,
	PORT_CONFIGURATION_BUSKEEPER,
	PORT_CONFIGURATION_PULLDOWN,
	PORT_CONFIGURATION_PULLUP,
	PORT_CONFIGURATION_WIREDOR,
	PORT_CONFIGURATION_WIREDAND,
	PORT_CONFIGURATION_WIREDORPULL,
	PORT_CONFIGURATION_WIREDANDPULL
};

enum port_dir { PORT_DIR_IN, PORT_DIR_OUT, PORT_DIR_OFF };

/**
 * \brief Set port pin Output & Pull configuration
 *
 * Configure pin output mode and pull configuration
 *
 * \param[in] pin       		The pin number for device
 * \param[in] output_pull_mode	Pin output and pull mode
 */
static inline void PORTA_set_pin_output_pull_mode(const uint8_t pin, const enum port_output_pull_mode output_pull_mode)
{
	volatile uint8_t *port_pin_ctrl = (volatile uint8_t *)(&PORTA.PIN0CTRL) + pin;

	*port_pin_ctrl &= PORT_OPC_TOTEM_gc;
	if (output_pull_mode == PORT_CONFIGURATION_BUSKEEPER) {
		*port_pin_ctrl |= PORT_OPC_BUSKEEPER_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLDOWN) {
		*port_pin_ctrl |= PORT_OPC_PULLDOWN_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLUP) {
		*port_pin_ctrl |= PORT_OPC_PULLUP_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDOR) {
		*port_pin_ctrl |= PORT_OPC_WIREDOR_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDAND) {
		*port_pin_ctrl |= PORT_OPC_WIREDAND_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDORPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDORPULL_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDANDPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDANDPULL_gc;
	}
}

/**
 * \brief Set port pin inverted mode
 *
 * Configure pin invert I/O or not
 *
 * \param[in] pin       The pin number within port
 * \param[in] inverted  Pin inverted mode
 */
static inline void PORTA_pin_set_inverted(const uint8_t pin, const bool inverted)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTA + 0x10 + pin);

	if (inverted) {
		*port_pin_ctrl |= PORT_INVEN_bm;
	} else {
		*port_pin_ctrl &= ~PORT_INVEN_bm;
	}
}

/**
 * \brief Set port pin input/sense configuration
 *
 * Enable/disable digital input buffer and pin change interrupt,
 * select pin interrupt edge/level sensing mode
 *
 * \param[in] pin pin number within port
 * \param[in] isc PORT_ISC_BOTHEDGES_gc     = Sense Both Edges
 *                PORT_ISC_RISING_gc        = Sense Rising Edge
 *                PORT_ISC_FALLING_gc       = Sense Falling Edge
 *                PORT_ISC_INPUT_DISABLE_gc = Digital Input Buffer disabled
 *                PORT_ISC_LEVEL_gc         = Sense low Level
 *
 */
static inline void PORTA_pin_set_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTA + 0x10 + pin);

	*port_pin_ctrl = (*port_pin_ctrl & ~PORT_ISC_gm) | isc;
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] mask      Bit mask where 1 means apply direction setting to the
 *                      corresponding pin
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTA_set_port_dir(const uint8_t mask, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTA.DIRCLR = mask;
		break;
	case PORT_DIR_OUT:
		PORTA.DIRSET = mask;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		PORTCFG.MPCMASK = mask;
		PORTA.PIN0CTRL  = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] pin       The pin number for device
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTA_set_pin_dir(const uint8_t pin, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTA.DIRCLR = 1 << pin;
		break;
	case PORT_DIR_OUT:
		PORTA.DIRSET = 1 << pin;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		*((uint8_t *)&PORTA + 0x10 + pin) = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on the pins defined by the bit mask
 *
 * \param[in] mask  Bit mask where 1 means apply port level to the corresponding
 *                  pin
 * \param[in] level true  = Pin levels set to "high" state
 *                  false = Pin levels set to "low" state
 */
static inline void PORTA_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		PORTA.OUTSET = mask;
	} else {
		PORTA.OUTCLR = mask;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on a pin
 *
 * \param[in] pin       The pin number for device
 * \param[in] level true  = Pin level set to "high" state
 *                  false = Pin level set to "low" state
 */
static inline void PORTA_set_pin_level(const uint8_t pin, const bool level)
{
	if (level == true) {
		PORTA.OUTSET = 1 << pin;
	} else {
		PORTA.OUTCLR = 1 << pin;
	}
}

/**
 * \brief Toggle out level on pins
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] mask  Bit mask where 1 means toggle pin level to the corresponding
 *                  pin
 */
static inline void PORTA_toggle_port_level(const uint8_t mask)
{
	PORTA.OUTTGL = mask;
}

/**
 * \brief Toggle output level on pin
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] pin       The pin number for device
 */
static inline void PORTA_toggle_pin_level(const uint8_t pin)
{
	PORTA.OUTTGL = 1 << pin;
}

/**
 * \brief Get input level on pins
 *
 * Read the input level on pins connected to a port
 *
 */
static inline uint8_t PORTA_get_port_level()
{
	return PORTA.IN;
}

/**
 * \brief Get level on pin
 *
 * Reads the level on pins connected to a port
 *
 * \param[in] pin       The pin number for device
 */
static inline bool PORTA_get_pin_level(const uint8_t pin)
{
	return PORTA.IN & (1 << pin);
}

/**
 * \brief Write value to Port
 *
 * Write directly to the port OUT register
 *
 * \param[in] value Value to write to the port register
 */
static inline void PORTA_write_port(const uint8_t value)
{
	PORTA.OUT = value;
}

/**
 * \brief Set port pin Output & Pull configuration
 *
 * Configure pin output mode and pull configuration
 *
 * \param[in] pin       		The pin number for device
 * \param[in] output_pull_mode	Pin output and pull mode
 */
static inline void PORTB_set_pin_output_pull_mode(const uint8_t pin, const enum port_output_pull_mode output_pull_mode)
{
	volatile uint8_t *port_pin_ctrl = (volatile uint8_t *)(&PORTB.PIN0CTRL) + pin;

	*port_pin_ctrl &= PORT_OPC_TOTEM_gc;
	if (output_pull_mode == PORT_CONFIGURATION_BUSKEEPER) {
		*port_pin_ctrl |= PORT_OPC_BUSKEEPER_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLDOWN) {
		*port_pin_ctrl |= PORT_OPC_PULLDOWN_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLUP) {
		*port_pin_ctrl |= PORT_OPC_PULLUP_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDOR) {
		*port_pin_ctrl |= PORT_OPC_WIREDOR_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDAND) {
		*port_pin_ctrl |= PORT_OPC_WIREDAND_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDORPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDORPULL_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDANDPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDANDPULL_gc;
	}
}

/**
 * \brief Set port pin inverted mode
 *
 * Configure pin invert I/O or not
 *
 * \param[in] pin       The pin number within port
 * \param[in] inverted  Pin inverted mode
 */
static inline void PORTB_pin_set_inverted(const uint8_t pin, const bool inverted)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTB + 0x10 + pin);

	if (inverted) {
		*port_pin_ctrl |= PORT_INVEN_bm;
	} else {
		*port_pin_ctrl &= ~PORT_INVEN_bm;
	}
}

/**
 * \brief Set port pin input/sense configuration
 *
 * Enable/disable digital input buffer and pin change interrupt,
 * select pin interrupt edge/level sensing mode
 *
 * \param[in] pin pin number within port
 * \param[in] isc PORT_ISC_BOTHEDGES_gc     = Sense Both Edges
 *                PORT_ISC_RISING_gc        = Sense Rising Edge
 *                PORT_ISC_FALLING_gc       = Sense Falling Edge
 *                PORT_ISC_INPUT_DISABLE_gc = Digital Input Buffer disabled
 *                PORT_ISC_LEVEL_gc         = Sense low Level
 *
 */
static inline void PORTB_pin_set_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTB + 0x10 + pin);

	*port_pin_ctrl = (*port_pin_ctrl & ~PORT_ISC_gm) | isc;
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] mask      Bit mask where 1 means apply direction setting to the
 *                      corresponding pin
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTB_set_port_dir(const uint8_t mask, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTB.DIRCLR = mask;
		break;
	case PORT_DIR_OUT:
		PORTB.DIRSET = mask;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		PORTCFG.MPCMASK = mask;
		PORTB.PIN0CTRL  = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] pin       The pin number for device
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTB_set_pin_dir(const uint8_t pin, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTB.DIRCLR = 1 << pin;
		break;
	case PORT_DIR_OUT:
		PORTB.DIRSET = 1 << pin;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		*((uint8_t *)&PORTB + 0x10 + pin) = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on the pins defined by the bit mask
 *
 * \param[in] mask  Bit mask where 1 means apply port level to the corresponding
 *                  pin
 * \param[in] level true  = Pin levels set to "high" state
 *                  false = Pin levels set to "low" state
 */
static inline void PORTB_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		PORTB.OUTSET = mask;
	} else {
		PORTB.OUTCLR = mask;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on a pin
 *
 * \param[in] pin       The pin number for device
 * \param[in] level true  = Pin level set to "high" state
 *                  false = Pin level set to "low" state
 */
static inline void PORTB_set_pin_level(const uint8_t pin, const bool level)
{
	if (level == true) {
		PORTB.OUTSET = 1 << pin;
	} else {
		PORTB.OUTCLR = 1 << pin;
	}
}

/**
 * \brief Toggle out level on pins
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] mask  Bit mask where 1 means toggle pin level to the corresponding
 *                  pin
 */
static inline void PORTB_toggle_port_level(const uint8_t mask)
{
	PORTB.OUTTGL = mask;
}

/**
 * \brief Toggle output level on pin
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] pin       The pin number for device
 */
static inline void PORTB_toggle_pin_level(const uint8_t pin)
{
	PORTB.OUTTGL = 1 << pin;
}

/**
 * \brief Get input level on pins
 *
 * Read the input level on pins connected to a port
 *
 */
static inline uint8_t PORTB_get_port_level()
{
	return PORTB.IN;
}

/**
 * \brief Get level on pin
 *
 * Reads the level on pins connected to a port
 *
 * \param[in] pin       The pin number for device
 */
static inline bool PORTB_get_pin_level(const uint8_t pin)
{
	return PORTB.IN & (1 << pin);
}

/**
 * \brief Write value to Port
 *
 * Write directly to the port OUT register
 *
 * \param[in] value Value to write to the port register
 */
static inline void PORTB_write_port(const uint8_t value)
{
	PORTB.OUT = value;
}

/**
 * \brief Set port pin Output & Pull configuration
 *
 * Configure pin output mode and pull configuration
 *
 * \param[in] pin       		The pin number for device
 * \param[in] output_pull_mode	Pin output and pull mode
 */
static inline void PORTC_set_pin_output_pull_mode(const uint8_t pin, const enum port_output_pull_mode output_pull_mode)
{
	volatile uint8_t *port_pin_ctrl = (volatile uint8_t *)(&PORTC.PIN0CTRL) + pin;

	*port_pin_ctrl &= PORT_OPC_TOTEM_gc;
	if (output_pull_mode == PORT_CONFIGURATION_BUSKEEPER) {
		*port_pin_ctrl |= PORT_OPC_BUSKEEPER_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLDOWN) {
		*port_pin_ctrl |= PORT_OPC_PULLDOWN_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLUP) {
		*port_pin_ctrl |= PORT_OPC_PULLUP_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDOR) {
		*port_pin_ctrl |= PORT_OPC_WIREDOR_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDAND) {
		*port_pin_ctrl |= PORT_OPC_WIREDAND_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDORPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDORPULL_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDANDPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDANDPULL_gc;
	}
}

/**
 * \brief Set port pin inverted mode
 *
 * Configure pin invert I/O or not
 *
 * \param[in] pin       The pin number within port
 * \param[in] inverted  Pin inverted mode
 */
static inline void PORTC_pin_set_inverted(const uint8_t pin, const bool inverted)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTC + 0x10 + pin);

	if (inverted) {
		*port_pin_ctrl |= PORT_INVEN_bm;
	} else {
		*port_pin_ctrl &= ~PORT_INVEN_bm;
	}
}

/**
 * \brief Set port pin input/sense configuration
 *
 * Enable/disable digital input buffer and pin change interrupt,
 * select pin interrupt edge/level sensing mode
 *
 * \param[in] pin pin number within port
 * \param[in] isc PORT_ISC_BOTHEDGES_gc     = Sense Both Edges
 *                PORT_ISC_RISING_gc        = Sense Rising Edge
 *                PORT_ISC_FALLING_gc       = Sense Falling Edge
 *                PORT_ISC_INPUT_DISABLE_gc = Digital Input Buffer disabled
 *                PORT_ISC_LEVEL_gc         = Sense low Level
 *
 */
static inline void PORTC_pin_set_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTC + 0x10 + pin);

	*port_pin_ctrl = (*port_pin_ctrl & ~PORT_ISC_gm) | isc;
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] mask      Bit mask where 1 means apply direction setting to the
 *                      corresponding pin
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTC_set_port_dir(const uint8_t mask, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTC.DIRCLR = mask;
		break;
	case PORT_DIR_OUT:
		PORTC.DIRSET = mask;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		PORTCFG.MPCMASK = mask;
		PORTC.PIN0CTRL  = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] pin       The pin number for device
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTC_set_pin_dir(const uint8_t pin, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTC.DIRCLR = 1 << pin;
		break;
	case PORT_DIR_OUT:
		PORTC.DIRSET = 1 << pin;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		*((uint8_t *)&PORTC + 0x10 + pin) = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on the pins defined by the bit mask
 *
 * \param[in] mask  Bit mask where 1 means apply port level to the corresponding
 *                  pin
 * \param[in] level true  = Pin levels set to "high" state
 *                  false = Pin levels set to "low" state
 */
static inline void PORTC_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		PORTC.OUTSET = mask;
	} else {
		PORTC.OUTCLR = mask;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on a pin
 *
 * \param[in] pin       The pin number for device
 * \param[in] level true  = Pin level set to "high" state
 *                  false = Pin level set to "low" state
 */
static inline void PORTC_set_pin_level(const uint8_t pin, const bool level)
{
	if (level == true) {
		PORTC.OUTSET = 1 << pin;
	} else {
		PORTC.OUTCLR = 1 << pin;
	}
}

/**
 * \brief Toggle out level on pins
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] mask  Bit mask where 1 means toggle pin level to the corresponding
 *                  pin
 */
static inline void PORTC_toggle_port_level(const uint8_t mask)
{
	PORTC.OUTTGL = mask;
}

/**
 * \brief Toggle output level on pin
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] pin       The pin number for device
 */
static inline void PORTC_toggle_pin_level(const uint8_t pin)
{
	PORTC.OUTTGL = 1 << pin;
}

/**
 * \brief Get input level on pins
 *
 * Read the input level on pins connected to a port
 *
 */
static inline uint8_t PORTC_get_port_level()
{
	return PORTC.IN;
}

/**
 * \brief Get level on pin
 *
 * Reads the level on pins connected to a port
 *
 * \param[in] pin       The pin number for device
 */
static inline bool PORTC_get_pin_level(const uint8_t pin)
{
	return PORTC.IN & (1 << pin);
}

/**
 * \brief Write value to Port
 *
 * Write directly to the port OUT register
 *
 * \param[in] value Value to write to the port register
 */
static inline void PORTC_write_port(const uint8_t value)
{
	PORTC.OUT = value;
}

/**
 * \brief Set port pin Output & Pull configuration
 *
 * Configure pin output mode and pull configuration
 *
 * \param[in] pin       		The pin number for device
 * \param[in] output_pull_mode	Pin output and pull mode
 */
static inline void PORTD_set_pin_output_pull_mode(const uint8_t pin, const enum port_output_pull_mode output_pull_mode)
{
	volatile uint8_t *port_pin_ctrl = (volatile uint8_t *)(&PORTD.PIN0CTRL) + pin;

	*port_pin_ctrl &= PORT_OPC_TOTEM_gc;
	if (output_pull_mode == PORT_CONFIGURATION_BUSKEEPER) {
		*port_pin_ctrl |= PORT_OPC_BUSKEEPER_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLDOWN) {
		*port_pin_ctrl |= PORT_OPC_PULLDOWN_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLUP) {
		*port_pin_ctrl |= PORT_OPC_PULLUP_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDOR) {
		*port_pin_ctrl |= PORT_OPC_WIREDOR_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDAND) {
		*port_pin_ctrl |= PORT_OPC_WIREDAND_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDORPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDORPULL_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDANDPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDANDPULL_gc;
	}
}

/**
 * \brief Set port pin inverted mode
 *
 * Configure pin invert I/O or not
 *
 * \param[in] pin       The pin number within port
 * \param[in] inverted  Pin inverted mode
 */
static inline void PORTD_pin_set_inverted(const uint8_t pin, const bool inverted)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTD + 0x10 + pin);

	if (inverted) {
		*port_pin_ctrl |= PORT_INVEN_bm;
	} else {
		*port_pin_ctrl &= ~PORT_INVEN_bm;
	}
}

/**
 * \brief Set port pin input/sense configuration
 *
 * Enable/disable digital input buffer and pin change interrupt,
 * select pin interrupt edge/level sensing mode
 *
 * \param[in] pin pin number within port
 * \param[in] isc PORT_ISC_BOTHEDGES_gc     = Sense Both Edges
 *                PORT_ISC_RISING_gc        = Sense Rising Edge
 *                PORT_ISC_FALLING_gc       = Sense Falling Edge
 *                PORT_ISC_INPUT_DISABLE_gc = Digital Input Buffer disabled
 *                PORT_ISC_LEVEL_gc         = Sense low Level
 *
 */
static inline void PORTD_pin_set_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTD + 0x10 + pin);

	*port_pin_ctrl = (*port_pin_ctrl & ~PORT_ISC_gm) | isc;
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] mask      Bit mask where 1 means apply direction setting to the
 *                      corresponding pin
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTD_set_port_dir(const uint8_t mask, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTD.DIRCLR = mask;
		break;
	case PORT_DIR_OUT:
		PORTD.DIRSET = mask;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		PORTCFG.MPCMASK = mask;
		PORTD.PIN0CTRL  = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] pin       The pin number for device
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTD_set_pin_dir(const uint8_t pin, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTD.DIRCLR = 1 << pin;
		break;
	case PORT_DIR_OUT:
		PORTD.DIRSET = 1 << pin;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		*((uint8_t *)&PORTD + 0x10 + pin) = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on the pins defined by the bit mask
 *
 * \param[in] mask  Bit mask where 1 means apply port level to the corresponding
 *                  pin
 * \param[in] level true  = Pin levels set to "high" state
 *                  false = Pin levels set to "low" state
 */
static inline void PORTD_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		PORTD.OUTSET = mask;
	} else {
		PORTD.OUTCLR = mask;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on a pin
 *
 * \param[in] pin       The pin number for device
 * \param[in] level true  = Pin level set to "high" state
 *                  false = Pin level set to "low" state
 */
static inline void PORTD_set_pin_level(const uint8_t pin, const bool level)
{
	if (level == true) {
		PORTD.OUTSET = 1 << pin;
	} else {
		PORTD.OUTCLR = 1 << pin;
	}
}

/**
 * \brief Toggle out level on pins
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] mask  Bit mask where 1 means toggle pin level to the corresponding
 *                  pin
 */
static inline void PORTD_toggle_port_level(const uint8_t mask)
{
	PORTD.OUTTGL = mask;
}

/**
 * \brief Toggle output level on pin
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] pin       The pin number for device
 */
static inline void PORTD_toggle_pin_level(const uint8_t pin)
{
	PORTD.OUTTGL = 1 << pin;
}

/**
 * \brief Get input level on pins
 *
 * Read the input level on pins connected to a port
 *
 */
static inline uint8_t PORTD_get_port_level()
{
	return PORTD.IN;
}

/**
 * \brief Get level on pin
 *
 * Reads the level on pins connected to a port
 *
 * \param[in] pin       The pin number for device
 */
static inline bool PORTD_get_pin_level(const uint8_t pin)
{
	return PORTD.IN & (1 << pin);
}

/**
 * \brief Write value to Port
 *
 * Write directly to the port OUT register
 *
 * \param[in] value Value to write to the port register
 */
static inline void PORTD_write_port(const uint8_t value)
{
	PORTD.OUT = value;
}

/**
 * \brief Set port pin Output & Pull configuration
 *
 * Configure pin output mode and pull configuration
 *
 * \param[in] pin       		The pin number for device
 * \param[in] output_pull_mode	Pin output and pull mode
 */
static inline void PORTE_set_pin_output_pull_mode(const uint8_t pin, const enum port_output_pull_mode output_pull_mode)
{
	volatile uint8_t *port_pin_ctrl = (volatile uint8_t *)(&PORTE.PIN0CTRL) + pin;

	*port_pin_ctrl &= PORT_OPC_TOTEM_gc;
	if (output_pull_mode == PORT_CONFIGURATION_BUSKEEPER) {
		*port_pin_ctrl |= PORT_OPC_BUSKEEPER_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLDOWN) {
		*port_pin_ctrl |= PORT_OPC_PULLDOWN_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLUP) {
		*port_pin_ctrl |= PORT_OPC_PULLUP_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDOR) {
		*port_pin_ctrl |= PORT_OPC_WIREDOR_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDAND) {
		*port_pin_ctrl |= PORT_OPC_WIREDAND_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDORPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDORPULL_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDANDPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDANDPULL_gc;
	}
}

/**
 * \brief Set port pin inverted mode
 *
 * Configure pin invert I/O or not
 *
 * \param[in] pin       The pin number within port
 * \param[in] inverted  Pin inverted mode
 */
static inline void PORTE_pin_set_inverted(const uint8_t pin, const bool inverted)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTE + 0x10 + pin);

	if (inverted) {
		*port_pin_ctrl |= PORT_INVEN_bm;
	} else {
		*port_pin_ctrl &= ~PORT_INVEN_bm;
	}
}

/**
 * \brief Set port pin input/sense configuration
 *
 * Enable/disable digital input buffer and pin change interrupt,
 * select pin interrupt edge/level sensing mode
 *
 * \param[in] pin pin number within port
 * \param[in] isc PORT_ISC_BOTHEDGES_gc     = Sense Both Edges
 *                PORT_ISC_RISING_gc        = Sense Rising Edge
 *                PORT_ISC_FALLING_gc       = Sense Falling Edge
 *                PORT_ISC_INPUT_DISABLE_gc = Digital Input Buffer disabled
 *                PORT_ISC_LEVEL_gc         = Sense low Level
 *
 */
static inline void PORTE_pin_set_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTE + 0x10 + pin);

	*port_pin_ctrl = (*port_pin_ctrl & ~PORT_ISC_gm) | isc;
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] mask      Bit mask where 1 means apply direction setting to the
 *                      corresponding pin
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTE_set_port_dir(const uint8_t mask, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTE.DIRCLR = mask;
		break;
	case PORT_DIR_OUT:
		PORTE.DIRSET = mask;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		PORTCFG.MPCMASK = mask;
		PORTE.PIN0CTRL  = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] pin       The pin number for device
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTE_set_pin_dir(const uint8_t pin, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTE.DIRCLR = 1 << pin;
		break;
	case PORT_DIR_OUT:
		PORTE.DIRSET = 1 << pin;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		*((uint8_t *)&PORTE + 0x10 + pin) = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on the pins defined by the bit mask
 *
 * \param[in] mask  Bit mask where 1 means apply port level to the corresponding
 *                  pin
 * \param[in] level true  = Pin levels set to "high" state
 *                  false = Pin levels set to "low" state
 */
static inline void PORTE_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		PORTE.OUTSET = mask;
	} else {
		PORTE.OUTCLR = mask;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on a pin
 *
 * \param[in] pin       The pin number for device
 * \param[in] level true  = Pin level set to "high" state
 *                  false = Pin level set to "low" state
 */
static inline void PORTE_set_pin_level(const uint8_t pin, const bool level)
{
	if (level == true) {
		PORTE.OUTSET = 1 << pin;
	} else {
		PORTE.OUTCLR = 1 << pin;
	}
}

/**
 * \brief Toggle out level on pins
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] mask  Bit mask where 1 means toggle pin level to the corresponding
 *                  pin
 */
static inline void PORTE_toggle_port_level(const uint8_t mask)
{
	PORTE.OUTTGL = mask;
}

/**
 * \brief Toggle output level on pin
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] pin       The pin number for device
 */
static inline void PORTE_toggle_pin_level(const uint8_t pin)
{
	PORTE.OUTTGL = 1 << pin;
}

/**
 * \brief Get input level on pins
 *
 * Read the input level on pins connected to a port
 *
 */
static inline uint8_t PORTE_get_port_level()
{
	return PORTE.IN;
}

/**
 * \brief Get level on pin
 *
 * Reads the level on pins connected to a port
 *
 * \param[in] pin       The pin number for device
 */
static inline bool PORTE_get_pin_level(const uint8_t pin)
{
	return PORTE.IN & (1 << pin);
}

/**
 * \brief Write value to Port
 *
 * Write directly to the port OUT register
 *
 * \param[in] value Value to write to the port register
 */
static inline void PORTE_write_port(const uint8_t value)
{
	PORTE.OUT = value;
}

/**
 * \brief Set port pin Output & Pull configuration
 *
 * Configure pin output mode and pull configuration
 *
 * \param[in] pin       		The pin number for device
 * \param[in] output_pull_mode	Pin output and pull mode
 */
static inline void PORTF_set_pin_output_pull_mode(const uint8_t pin, const enum port_output_pull_mode output_pull_mode)
{
	volatile uint8_t *port_pin_ctrl = (volatile uint8_t *)(&PORTF.PIN0CTRL) + pin;

	*port_pin_ctrl &= PORT_OPC_TOTEM_gc;
	if (output_pull_mode == PORT_CONFIGURATION_BUSKEEPER) {
		*port_pin_ctrl |= PORT_OPC_BUSKEEPER_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLDOWN) {
		*port_pin_ctrl |= PORT_OPC_PULLDOWN_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLUP) {
		*port_pin_ctrl |= PORT_OPC_PULLUP_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDOR) {
		*port_pin_ctrl |= PORT_OPC_WIREDOR_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDAND) {
		*port_pin_ctrl |= PORT_OPC_WIREDAND_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDORPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDORPULL_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDANDPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDANDPULL_gc;
	}
}

/**
 * \brief Set port pin inverted mode
 *
 * Configure pin invert I/O or not
 *
 * \param[in] pin       The pin number within port
 * \param[in] inverted  Pin inverted mode
 */
static inline void PORTF_pin_set_inverted(const uint8_t pin, const bool inverted)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTF + 0x10 + pin);

	if (inverted) {
		*port_pin_ctrl |= PORT_INVEN_bm;
	} else {
		*port_pin_ctrl &= ~PORT_INVEN_bm;
	}
}

/**
 * \brief Set port pin input/sense configuration
 *
 * Enable/disable digital input buffer and pin change interrupt,
 * select pin interrupt edge/level sensing mode
 *
 * \param[in] pin pin number within port
 * \param[in] isc PORT_ISC_BOTHEDGES_gc     = Sense Both Edges
 *                PORT_ISC_RISING_gc        = Sense Rising Edge
 *                PORT_ISC_FALLING_gc       = Sense Falling Edge
 *                PORT_ISC_INPUT_DISABLE_gc = Digital Input Buffer disabled
 *                PORT_ISC_LEVEL_gc         = Sense low Level
 *
 */
static inline void PORTF_pin_set_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTF + 0x10 + pin);

	*port_pin_ctrl = (*port_pin_ctrl & ~PORT_ISC_gm) | isc;
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] mask      Bit mask where 1 means apply direction setting to the
 *                      corresponding pin
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTF_set_port_dir(const uint8_t mask, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTF.DIRCLR = mask;
		break;
	case PORT_DIR_OUT:
		PORTF.DIRSET = mask;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		PORTCFG.MPCMASK = mask;
		PORTF.PIN0CTRL  = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] pin       The pin number for device
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTF_set_pin_dir(const uint8_t pin, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTF.DIRCLR = 1 << pin;
		break;
	case PORT_DIR_OUT:
		PORTF.DIRSET = 1 << pin;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		*((uint8_t *)&PORTF + 0x10 + pin) = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on the pins defined by the bit mask
 *
 * \param[in] mask  Bit mask where 1 means apply port level to the corresponding
 *                  pin
 * \param[in] level true  = Pin levels set to "high" state
 *                  false = Pin levels set to "low" state
 */
static inline void PORTF_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		PORTF.OUTSET = mask;
	} else {
		PORTF.OUTCLR = mask;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on a pin
 *
 * \param[in] pin       The pin number for device
 * \param[in] level true  = Pin level set to "high" state
 *                  false = Pin level set to "low" state
 */
static inline void PORTF_set_pin_level(const uint8_t pin, const bool level)
{
	if (level == true) {
		PORTF.OUTSET = 1 << pin;
	} else {
		PORTF.OUTCLR = 1 << pin;
	}
}

/**
 * \brief Toggle out level on pins
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] mask  Bit mask where 1 means toggle pin level to the corresponding
 *                  pin
 */
static inline void PORTF_toggle_port_level(const uint8_t mask)
{
	PORTF.OUTTGL = mask;
}

/**
 * \brief Toggle output level on pin
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] pin       The pin number for device
 */
static inline void PORTF_toggle_pin_level(const uint8_t pin)
{
	PORTF.OUTTGL = 1 << pin;
}

/**
 * \brief Get input level on pins
 *
 * Read the input level on pins connected to a port
 *
 */
static inline uint8_t PORTF_get_port_level()
{
	return PORTF.IN;
}

/**
 * \brief Get level on pin
 *
 * Reads the level on pins connected to a port
 *
 * \param[in] pin       The pin number for device
 */
static inline bool PORTF_get_pin_level(const uint8_t pin)
{
	return PORTF.IN & (1 << pin);
}

/**
 * \brief Write value to Port
 *
 * Write directly to the port OUT register
 *
 * \param[in] value Value to write to the port register
 */
static inline void PORTF_write_port(const uint8_t value)
{
	PORTF.OUT = value;
}

/**
 * \brief Set port pin Output & Pull configuration
 *
 * Configure pin output mode and pull configuration
 *
 * \param[in] pin       		The pin number for device
 * \param[in] output_pull_mode	Pin output and pull mode
 */
static inline void PORTH_set_pin_output_pull_mode(const uint8_t pin, const enum port_output_pull_mode output_pull_mode)
{
	volatile uint8_t *port_pin_ctrl = (volatile uint8_t *)(&PORTH.PIN0CTRL) + pin;

	*port_pin_ctrl &= PORT_OPC_TOTEM_gc;
	if (output_pull_mode == PORT_CONFIGURATION_BUSKEEPER) {
		*port_pin_ctrl |= PORT_OPC_BUSKEEPER_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLDOWN) {
		*port_pin_ctrl |= PORT_OPC_PULLDOWN_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLUP) {
		*port_pin_ctrl |= PORT_OPC_PULLUP_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDOR) {
		*port_pin_ctrl |= PORT_OPC_WIREDOR_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDAND) {
		*port_pin_ctrl |= PORT_OPC_WIREDAND_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDORPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDORPULL_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDANDPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDANDPULL_gc;
	}
}

/**
 * \brief Set port pin inverted mode
 *
 * Configure pin invert I/O or not
 *
 * \param[in] pin       The pin number within port
 * \param[in] inverted  Pin inverted mode
 */
static inline void PORTH_pin_set_inverted(const uint8_t pin, const bool inverted)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTH + 0x10 + pin);

	if (inverted) {
		*port_pin_ctrl |= PORT_INVEN_bm;
	} else {
		*port_pin_ctrl &= ~PORT_INVEN_bm;
	}
}

/**
 * \brief Set port pin input/sense configuration
 *
 * Enable/disable digital input buffer and pin change interrupt,
 * select pin interrupt edge/level sensing mode
 *
 * \param[in] pin pin number within port
 * \param[in] isc PORT_ISC_BOTHEDGES_gc     = Sense Both Edges
 *                PORT_ISC_RISING_gc        = Sense Rising Edge
 *                PORT_ISC_FALLING_gc       = Sense Falling Edge
 *                PORT_ISC_INPUT_DISABLE_gc = Digital Input Buffer disabled
 *                PORT_ISC_LEVEL_gc         = Sense low Level
 *
 */
static inline void PORTH_pin_set_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTH + 0x10 + pin);

	*port_pin_ctrl = (*port_pin_ctrl & ~PORT_ISC_gm) | isc;
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] mask      Bit mask where 1 means apply direction setting to the
 *                      corresponding pin
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTH_set_port_dir(const uint8_t mask, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTH.DIRCLR = mask;
		break;
	case PORT_DIR_OUT:
		PORTH.DIRSET = mask;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		PORTCFG.MPCMASK = mask;
		PORTH.PIN0CTRL  = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] pin       The pin number for device
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTH_set_pin_dir(const uint8_t pin, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTH.DIRCLR = 1 << pin;
		break;
	case PORT_DIR_OUT:
		PORTH.DIRSET = 1 << pin;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		*((uint8_t *)&PORTH + 0x10 + pin) = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on the pins defined by the bit mask
 *
 * \param[in] mask  Bit mask where 1 means apply port level to the corresponding
 *                  pin
 * \param[in] level true  = Pin levels set to "high" state
 *                  false = Pin levels set to "low" state
 */
static inline void PORTH_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		PORTH.OUTSET = mask;
	} else {
		PORTH.OUTCLR = mask;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on a pin
 *
 * \param[in] pin       The pin number for device
 * \param[in] level true  = Pin level set to "high" state
 *                  false = Pin level set to "low" state
 */
static inline void PORTH_set_pin_level(const uint8_t pin, const bool level)
{
	if (level == true) {
		PORTH.OUTSET = 1 << pin;
	} else {
		PORTH.OUTCLR = 1 << pin;
	}
}

/**
 * \brief Toggle out level on pins
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] mask  Bit mask where 1 means toggle pin level to the corresponding
 *                  pin
 */
static inline void PORTH_toggle_port_level(const uint8_t mask)
{
	PORTH.OUTTGL = mask;
}

/**
 * \brief Toggle output level on pin
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] pin       The pin number for device
 */
static inline void PORTH_toggle_pin_level(const uint8_t pin)
{
	PORTH.OUTTGL = 1 << pin;
}

/**
 * \brief Get input level on pins
 *
 * Read the input level on pins connected to a port
 *
 */
static inline uint8_t PORTH_get_port_level()
{
	return PORTH.IN;
}

/**
 * \brief Get level on pin
 *
 * Reads the level on pins connected to a port
 *
 * \param[in] pin       The pin number for device
 */
static inline bool PORTH_get_pin_level(const uint8_t pin)
{
	return PORTH.IN & (1 << pin);
}

/**
 * \brief Write value to Port
 *
 * Write directly to the port OUT register
 *
 * \param[in] value Value to write to the port register
 */
static inline void PORTH_write_port(const uint8_t value)
{
	PORTH.OUT = value;
}

/**
 * \brief Set port pin Output & Pull configuration
 *
 * Configure pin output mode and pull configuration
 *
 * \param[in] pin       		The pin number for device
 * \param[in] output_pull_mode	Pin output and pull mode
 */
static inline void PORTJ_set_pin_output_pull_mode(const uint8_t pin, const enum port_output_pull_mode output_pull_mode)
{
	volatile uint8_t *port_pin_ctrl = (volatile uint8_t *)(&PORTJ.PIN0CTRL) + pin;

	*port_pin_ctrl &= PORT_OPC_TOTEM_gc;
	if (output_pull_mode == PORT_CONFIGURATION_BUSKEEPER) {
		*port_pin_ctrl |= PORT_OPC_BUSKEEPER_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLDOWN) {
		*port_pin_ctrl |= PORT_OPC_PULLDOWN_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLUP) {
		*port_pin_ctrl |= PORT_OPC_PULLUP_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDOR) {
		*port_pin_ctrl |= PORT_OPC_WIREDOR_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDAND) {
		*port_pin_ctrl |= PORT_OPC_WIREDAND_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDORPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDORPULL_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDANDPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDANDPULL_gc;
	}
}

/**
 * \brief Set port pin inverted mode
 *
 * Configure pin invert I/O or not
 *
 * \param[in] pin       The pin number within port
 * \param[in] inverted  Pin inverted mode
 */
static inline void PORTJ_pin_set_inverted(const uint8_t pin, const bool inverted)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTJ + 0x10 + pin);

	if (inverted) {
		*port_pin_ctrl |= PORT_INVEN_bm;
	} else {
		*port_pin_ctrl &= ~PORT_INVEN_bm;
	}
}

/**
 * \brief Set port pin input/sense configuration
 *
 * Enable/disable digital input buffer and pin change interrupt,
 * select pin interrupt edge/level sensing mode
 *
 * \param[in] pin pin number within port
 * \param[in] isc PORT_ISC_BOTHEDGES_gc     = Sense Both Edges
 *                PORT_ISC_RISING_gc        = Sense Rising Edge
 *                PORT_ISC_FALLING_gc       = Sense Falling Edge
 *                PORT_ISC_INPUT_DISABLE_gc = Digital Input Buffer disabled
 *                PORT_ISC_LEVEL_gc         = Sense low Level
 *
 */
static inline void PORTJ_pin_set_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTJ + 0x10 + pin);

	*port_pin_ctrl = (*port_pin_ctrl & ~PORT_ISC_gm) | isc;
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] mask      Bit mask where 1 means apply direction setting to the
 *                      corresponding pin
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTJ_set_port_dir(const uint8_t mask, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTJ.DIRCLR = mask;
		break;
	case PORT_DIR_OUT:
		PORTJ.DIRSET = mask;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		PORTCFG.MPCMASK = mask;
		PORTJ.PIN0CTRL  = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] pin       The pin number for device
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTJ_set_pin_dir(const uint8_t pin, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTJ.DIRCLR = 1 << pin;
		break;
	case PORT_DIR_OUT:
		PORTJ.DIRSET = 1 << pin;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		*((uint8_t *)&PORTJ + 0x10 + pin) = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on the pins defined by the bit mask
 *
 * \param[in] mask  Bit mask where 1 means apply port level to the corresponding
 *                  pin
 * \param[in] level true  = Pin levels set to "high" state
 *                  false = Pin levels set to "low" state
 */
static inline void PORTJ_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		PORTJ.OUTSET = mask;
	} else {
		PORTJ.OUTCLR = mask;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on a pin
 *
 * \param[in] pin       The pin number for device
 * \param[in] level true  = Pin level set to "high" state
 *                  false = Pin level set to "low" state
 */
static inline void PORTJ_set_pin_level(const uint8_t pin, const bool level)
{
	if (level == true) {
		PORTJ.OUTSET = 1 << pin;
	} else {
		PORTJ.OUTCLR = 1 << pin;
	}
}

/**
 * \brief Toggle out level on pins
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] mask  Bit mask where 1 means toggle pin level to the corresponding
 *                  pin
 */
static inline void PORTJ_toggle_port_level(const uint8_t mask)
{
	PORTJ.OUTTGL = mask;
}

/**
 * \brief Toggle output level on pin
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] pin       The pin number for device
 */
static inline void PORTJ_toggle_pin_level(const uint8_t pin)
{
	PORTJ.OUTTGL = 1 << pin;
}

/**
 * \brief Get input level on pins
 *
 * Read the input level on pins connected to a port
 *
 */
static inline uint8_t PORTJ_get_port_level()
{
	return PORTJ.IN;
}

/**
 * \brief Get level on pin
 *
 * Reads the level on pins connected to a port
 *
 * \param[in] pin       The pin number for device
 */
static inline bool PORTJ_get_pin_level(const uint8_t pin)
{
	return PORTJ.IN & (1 << pin);
}

/**
 * \brief Write value to Port
 *
 * Write directly to the port OUT register
 *
 * \param[in] value Value to write to the port register
 */
static inline void PORTJ_write_port(const uint8_t value)
{
	PORTJ.OUT = value;
}

/**
 * \brief Set port pin Output & Pull configuration
 *
 * Configure pin output mode and pull configuration
 *
 * \param[in] pin       		The pin number for device
 * \param[in] output_pull_mode	Pin output and pull mode
 */
static inline void PORTK_set_pin_output_pull_mode(const uint8_t pin, const enum port_output_pull_mode output_pull_mode)
{
	volatile uint8_t *port_pin_ctrl = (volatile uint8_t *)(&PORTK.PIN0CTRL) + pin;

	*port_pin_ctrl &= PORT_OPC_TOTEM_gc;
	if (output_pull_mode == PORT_CONFIGURATION_BUSKEEPER) {
		*port_pin_ctrl |= PORT_OPC_BUSKEEPER_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLDOWN) {
		*port_pin_ctrl |= PORT_OPC_PULLDOWN_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLUP) {
		*port_pin_ctrl |= PORT_OPC_PULLUP_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDOR) {
		*port_pin_ctrl |= PORT_OPC_WIREDOR_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDAND) {
		*port_pin_ctrl |= PORT_OPC_WIREDAND_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDORPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDORPULL_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDANDPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDANDPULL_gc;
	}
}

/**
 * \brief Set port pin inverted mode
 *
 * Configure pin invert I/O or not
 *
 * \param[in] pin       The pin number within port
 * \param[in] inverted  Pin inverted mode
 */
static inline void PORTK_pin_set_inverted(const uint8_t pin, const bool inverted)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTK + 0x10 + pin);

	if (inverted) {
		*port_pin_ctrl |= PORT_INVEN_bm;
	} else {
		*port_pin_ctrl &= ~PORT_INVEN_bm;
	}
}

/**
 * \brief Set port pin input/sense configuration
 *
 * Enable/disable digital input buffer and pin change interrupt,
 * select pin interrupt edge/level sensing mode
 *
 * \param[in] pin pin number within port
 * \param[in] isc PORT_ISC_BOTHEDGES_gc     = Sense Both Edges
 *                PORT_ISC_RISING_gc        = Sense Rising Edge
 *                PORT_ISC_FALLING_gc       = Sense Falling Edge
 *                PORT_ISC_INPUT_DISABLE_gc = Digital Input Buffer disabled
 *                PORT_ISC_LEVEL_gc         = Sense low Level
 *
 */
static inline void PORTK_pin_set_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTK + 0x10 + pin);

	*port_pin_ctrl = (*port_pin_ctrl & ~PORT_ISC_gm) | isc;
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] mask      Bit mask where 1 means apply direction setting to the
 *                      corresponding pin
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTK_set_port_dir(const uint8_t mask, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTK.DIRCLR = mask;
		break;
	case PORT_DIR_OUT:
		PORTK.DIRSET = mask;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		PORTCFG.MPCMASK = mask;
		PORTK.PIN0CTRL  = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] pin       The pin number for device
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTK_set_pin_dir(const uint8_t pin, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTK.DIRCLR = 1 << pin;
		break;
	case PORT_DIR_OUT:
		PORTK.DIRSET = 1 << pin;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		*((uint8_t *)&PORTK + 0x10 + pin) = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on the pins defined by the bit mask
 *
 * \param[in] mask  Bit mask where 1 means apply port level to the corresponding
 *                  pin
 * \param[in] level true  = Pin levels set to "high" state
 *                  false = Pin levels set to "low" state
 */
static inline void PORTK_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		PORTK.OUTSET = mask;
	} else {
		PORTK.OUTCLR = mask;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on a pin
 *
 * \param[in] pin       The pin number for device
 * \param[in] level true  = Pin level set to "high" state
 *                  false = Pin level set to "low" state
 */
static inline void PORTK_set_pin_level(const uint8_t pin, const bool level)
{
	if (level == true) {
		PORTK.OUTSET = 1 << pin;
	} else {
		PORTK.OUTCLR = 1 << pin;
	}
}

/**
 * \brief Toggle out level on pins
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] mask  Bit mask where 1 means toggle pin level to the corresponding
 *                  pin
 */
static inline void PORTK_toggle_port_level(const uint8_t mask)
{
	PORTK.OUTTGL = mask;
}

/**
 * \brief Toggle output level on pin
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] pin       The pin number for device
 */
static inline void PORTK_toggle_pin_level(const uint8_t pin)
{
	PORTK.OUTTGL = 1 << pin;
}

/**
 * \brief Get input level on pins
 *
 * Read the input level on pins connected to a port
 *
 */
static inline uint8_t PORTK_get_port_level()
{
	return PORTK.IN;
}

/**
 * \brief Get level on pin
 *
 * Reads the level on pins connected to a port
 *
 * \param[in] pin       The pin number for device
 */
static inline bool PORTK_get_pin_level(const uint8_t pin)
{
	return PORTK.IN & (1 << pin);
}

/**
 * \brief Write value to Port
 *
 * Write directly to the port OUT register
 *
 * \param[in] value Value to write to the port register
 */
static inline void PORTK_write_port(const uint8_t value)
{
	PORTK.OUT = value;
}

/**
 * \brief Set port pin Output & Pull configuration
 *
 * Configure pin output mode and pull configuration
 *
 * \param[in] pin       		The pin number for device
 * \param[in] output_pull_mode	Pin output and pull mode
 */
static inline void PORTQ_set_pin_output_pull_mode(const uint8_t pin, const enum port_output_pull_mode output_pull_mode)
{
	volatile uint8_t *port_pin_ctrl = (volatile uint8_t *)(&PORTQ.PIN0CTRL) + pin;

	*port_pin_ctrl &= PORT_OPC_TOTEM_gc;
	if (output_pull_mode == PORT_CONFIGURATION_BUSKEEPER) {
		*port_pin_ctrl |= PORT_OPC_BUSKEEPER_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLDOWN) {
		*port_pin_ctrl |= PORT_OPC_PULLDOWN_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLUP) {
		*port_pin_ctrl |= PORT_OPC_PULLUP_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDOR) {
		*port_pin_ctrl |= PORT_OPC_WIREDOR_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDAND) {
		*port_pin_ctrl |= PORT_OPC_WIREDAND_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDORPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDORPULL_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDANDPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDANDPULL_gc;
	}
}

/**
 * \brief Set port pin inverted mode
 *
 * Configure pin invert I/O or not
 *
 * \param[in] pin       The pin number within port
 * \param[in] inverted  Pin inverted mode
 */
static inline void PORTQ_pin_set_inverted(const uint8_t pin, const bool inverted)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTQ + 0x10 + pin);

	if (inverted) {
		*port_pin_ctrl |= PORT_INVEN_bm;
	} else {
		*port_pin_ctrl &= ~PORT_INVEN_bm;
	}
}

/**
 * \brief Set port pin input/sense configuration
 *
 * Enable/disable digital input buffer and pin change interrupt,
 * select pin interrupt edge/level sensing mode
 *
 * \param[in] pin pin number within port
 * \param[in] isc PORT_ISC_BOTHEDGES_gc     = Sense Both Edges
 *                PORT_ISC_RISING_gc        = Sense Rising Edge
 *                PORT_ISC_FALLING_gc       = Sense Falling Edge
 *                PORT_ISC_INPUT_DISABLE_gc = Digital Input Buffer disabled
 *                PORT_ISC_LEVEL_gc         = Sense low Level
 *
 */
static inline void PORTQ_pin_set_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTQ + 0x10 + pin);

	*port_pin_ctrl = (*port_pin_ctrl & ~PORT_ISC_gm) | isc;
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] mask      Bit mask where 1 means apply direction setting to the
 *                      corresponding pin
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTQ_set_port_dir(const uint8_t mask, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTQ.DIRCLR = mask;
		break;
	case PORT_DIR_OUT:
		PORTQ.DIRSET = mask;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		PORTCFG.MPCMASK = mask;
		PORTQ.PIN0CTRL  = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] pin       The pin number for device
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTQ_set_pin_dir(const uint8_t pin, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTQ.DIRCLR = 1 << pin;
		break;
	case PORT_DIR_OUT:
		PORTQ.DIRSET = 1 << pin;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		*((uint8_t *)&PORTQ + 0x10 + pin) = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on the pins defined by the bit mask
 *
 * \param[in] mask  Bit mask where 1 means apply port level to the corresponding
 *                  pin
 * \param[in] level true  = Pin levels set to "high" state
 *                  false = Pin levels set to "low" state
 */
static inline void PORTQ_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		PORTQ.OUTSET = mask;
	} else {
		PORTQ.OUTCLR = mask;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on a pin
 *
 * \param[in] pin       The pin number for device
 * \param[in] level true  = Pin level set to "high" state
 *                  false = Pin level set to "low" state
 */
static inline void PORTQ_set_pin_level(const uint8_t pin, const bool level)
{
	if (level == true) {
		PORTQ.OUTSET = 1 << pin;
	} else {
		PORTQ.OUTCLR = 1 << pin;
	}
}

/**
 * \brief Toggle out level on pins
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] mask  Bit mask where 1 means toggle pin level to the corresponding
 *                  pin
 */
static inline void PORTQ_toggle_port_level(const uint8_t mask)
{
	PORTQ.OUTTGL = mask;
}

/**
 * \brief Toggle output level on pin
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] pin       The pin number for device
 */
static inline void PORTQ_toggle_pin_level(const uint8_t pin)
{
	PORTQ.OUTTGL = 1 << pin;
}

/**
 * \brief Get input level on pins
 *
 * Read the input level on pins connected to a port
 *
 */
static inline uint8_t PORTQ_get_port_level()
{
	return PORTQ.IN;
}

/**
 * \brief Get level on pin
 *
 * Reads the level on pins connected to a port
 *
 * \param[in] pin       The pin number for device
 */
static inline bool PORTQ_get_pin_level(const uint8_t pin)
{
	return PORTQ.IN & (1 << pin);
}

/**
 * \brief Write value to Port
 *
 * Write directly to the port OUT register
 *
 * \param[in] value Value to write to the port register
 */
static inline void PORTQ_write_port(const uint8_t value)
{
	PORTQ.OUT = value;
}

/**
 * \brief Set port pin Output & Pull configuration
 *
 * Configure pin output mode and pull configuration
 *
 * \param[in] pin       		The pin number for device
 * \param[in] output_pull_mode	Pin output and pull mode
 */
static inline void PORTR_set_pin_output_pull_mode(const uint8_t pin, const enum port_output_pull_mode output_pull_mode)
{
	volatile uint8_t *port_pin_ctrl = (volatile uint8_t *)(&PORTR.PIN0CTRL) + pin;

	*port_pin_ctrl &= PORT_OPC_TOTEM_gc;
	if (output_pull_mode == PORT_CONFIGURATION_BUSKEEPER) {
		*port_pin_ctrl |= PORT_OPC_BUSKEEPER_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLDOWN) {
		*port_pin_ctrl |= PORT_OPC_PULLDOWN_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_PULLUP) {
		*port_pin_ctrl |= PORT_OPC_PULLUP_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDOR) {
		*port_pin_ctrl |= PORT_OPC_WIREDOR_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDAND) {
		*port_pin_ctrl |= PORT_OPC_WIREDAND_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDORPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDORPULL_gc;
	} else if (output_pull_mode == PORT_CONFIGURATION_WIREDANDPULL) {
		*port_pin_ctrl |= PORT_OPC_WIREDANDPULL_gc;
	}
}

/**
 * \brief Set port pin inverted mode
 *
 * Configure pin invert I/O or not
 *
 * \param[in] pin       The pin number within port
 * \param[in] inverted  Pin inverted mode
 */
static inline void PORTR_pin_set_inverted(const uint8_t pin, const bool inverted)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTR + 0x10 + pin);

	if (inverted) {
		*port_pin_ctrl |= PORT_INVEN_bm;
	} else {
		*port_pin_ctrl &= ~PORT_INVEN_bm;
	}
}

/**
 * \brief Set port pin input/sense configuration
 *
 * Enable/disable digital input buffer and pin change interrupt,
 * select pin interrupt edge/level sensing mode
 *
 * \param[in] pin pin number within port
 * \param[in] isc PORT_ISC_BOTHEDGES_gc     = Sense Both Edges
 *                PORT_ISC_RISING_gc        = Sense Rising Edge
 *                PORT_ISC_FALLING_gc       = Sense Falling Edge
 *                PORT_ISC_INPUT_DISABLE_gc = Digital Input Buffer disabled
 *                PORT_ISC_LEVEL_gc         = Sense low Level
 *
 */
static inline void PORTR_pin_set_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTR + 0x10 + pin);

	*port_pin_ctrl = (*port_pin_ctrl & ~PORT_ISC_gm) | isc;
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] mask      Bit mask where 1 means apply direction setting to the
 *                      corresponding pin
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTR_set_port_dir(const uint8_t mask, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTR.DIRCLR = mask;
		break;
	case PORT_DIR_OUT:
		PORTR.DIRSET = mask;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		PORTCFG.MPCMASK = mask;
		PORTR.PIN0CTRL  = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] pin       The pin number for device
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTR_set_pin_dir(const uint8_t pin, const enum port_dir dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		PORTR.DIRCLR = 1 << pin;
		break;
	case PORT_DIR_OUT:
		PORTR.DIRSET = 1 << pin;
		break;
	case PORT_DIR_OFF:
		/* Activate pullup for power saving */
		*((uint8_t *)&PORTR + 0x10 + pin) = PORT_OPC_PULLUP_gc;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on the pins defined by the bit mask
 *
 * \param[in] mask  Bit mask where 1 means apply port level to the corresponding
 *                  pin
 * \param[in] level true  = Pin levels set to "high" state
 *                  false = Pin levels set to "low" state
 */
static inline void PORTR_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		PORTR.OUTSET = mask;
	} else {
		PORTR.OUTCLR = mask;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on a pin
 *
 * \param[in] pin       The pin number for device
 * \param[in] level true  = Pin level set to "high" state
 *                  false = Pin level set to "low" state
 */
static inline void PORTR_set_pin_level(const uint8_t pin, const bool level)
{
	if (level == true) {
		PORTR.OUTSET = 1 << pin;
	} else {
		PORTR.OUTCLR = 1 << pin;
	}
}

/**
 * \brief Toggle out level on pins
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] mask  Bit mask where 1 means toggle pin level to the corresponding
 *                  pin
 */
static inline void PORTR_toggle_port_level(const uint8_t mask)
{
	PORTR.OUTTGL = mask;
}

/**
 * \brief Toggle output level on pin
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] pin       The pin number for device
 */
static inline void PORTR_toggle_pin_level(const uint8_t pin)
{
	PORTR.OUTTGL = 1 << pin;
}

/**
 * \brief Get input level on pins
 *
 * Read the input level on pins connected to a port
 *
 */
static inline uint8_t PORTR_get_port_level()
{
	return PORTR.IN;
}

/**
 * \brief Get level on pin
 *
 * Reads the level on pins connected to a port
 *
 * \param[in] pin       The pin number for device
 */
static inline bool PORTR_get_pin_level(const uint8_t pin)
{
	return PORTR.IN & (1 << pin);
}

/**
 * \brief Write value to Port
 *
 * Write directly to the port OUT register
 *
 * \param[in] value Value to write to the port register
 */
static inline void PORTR_write_port(const uint8_t value)
{
	PORTR.OUT = value;
}

#ifdef __cplusplus
}
#endif

#endif
