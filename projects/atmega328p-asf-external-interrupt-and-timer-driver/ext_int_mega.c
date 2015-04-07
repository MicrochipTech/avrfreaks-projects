/**
 * \file
 *
 * \brief External interrupt driver for ATmega328p
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <compiler.h>
#include "ioport.h"
#include <ext_int_mega.h>

//! \internal Local storage of EXT_INT0 interrupt callback function
static ext_int_callback_t ext_int_int0_callback;
//! \internal Local storage of EXT_INT1 interrupt callback function
static ext_int_callback_t ext_int_int1_callback;
//! \internal Local storage of EXT_INT2 interrupt callback function
//! \internal Local storage of PC_EXT_INT0 to PC_EXT_INT7 interrupt callback function
static ext_int_callback_t ext_int_pcint0_callback;
//! \internal Local storage of PC_EXT_INT8 to PC_EXT_INT14 interrupt callback function
static ext_int_callback_t ext_int_pcint1_callback;
//! \internal Local storage of PC_EXT_INT16 to PC_EXT_INT20 interrupt callback function
static ext_int_callback_t ext_int_pcint2_callback;

/**
 * \internal
 * \brief Interrupt handler for External Interrupt INT0
 *
 * This function will handle interrupt on External Interrupt INT0 and
 * call the callback function.
 */
ISR(INT0_vect)
{
	if (ext_int_int0_callback) {
		ext_int_int0_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for External Interrupt INT1
 *
 * This function will handle interrupt on External Interrupt INT1 and
 * call the callback function.
 */
ISR(INT1_vect)
{
	if (ext_int_int1_callback) {
		ext_int_int1_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for PC External Interrupt INT0 - INT7
 *
 * This function will handle interrupt on PC External Interrupt INT0-INT7 and
 * call the callback function.
 */
ISR(PCINT0_vect)
{
	if (ext_int_pcint0_callback) {
		ext_int_pcint0_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for PC External Interrupt INT8 - INT15
 *
 * This function will handle interrupt on PC External Interrupt INT5-INT15 and
 * call the callback function.
 */
ISR(PCINT1_vect)
{
	if (ext_int_pcint1_callback) {
		ext_int_pcint1_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for PC External Interrupt INT16 - INT23
 *
 * This function will handle interrupt on PC External Interrupt INT16-INT23 and
 * call the callback function.
 */
ISR(PCINT2_vect)
{
	if (ext_int_pcint2_callback) {
		ext_int_pcint2_callback();
	}
}

void ext_int_pcint_clear_flag(ioport_pin_t pin)
{
	if((pin >= PC_INT16_PIN)&&(pin <= PC_INT20_PIN)) {
		CLEAR_PCINT_FLAG2();
	} else if((pin >= PC_INT8_PIN)&&(pin <= PC_INT14_PIN)) {
		CLEAR_PCINT_FLAG1();
	} else if((pin >= PC_INT0_PIN)&&(pin <= PC_INT7_PIN)) {
		CLEAR_PCINT_FLAG0();
	}
}

void ext_int_pcint_enable(ioport_pin_t pin)
{
	if((pin >= PC_INT16_PIN)&&(pin <= PC_INT20_PIN)) {
		 PCICR |= (1 << PCIE2);
		 PCMSK2 |= INT_PIN_MASK(pin);
	} else
	if((pin >= PC_INT8_PIN)&&(pin <= PC_INT14_PIN)) {
		PCICR |= (1 << PCIE1);
		PCMSK1 |= INT_PIN_MASK(pin);
	} else
	if((pin >= PC_INT0_PIN)&&(pin <= PC_INT7_PIN)) {
		PCICR |= (1 << PCIE0);
		PCMSK0 |= INT_PIN_MASK(pin);
	}
}

void ext_int_pcint_disable(ioport_pin_t pin)
{
	if((pin >= PC_INT16_PIN)&&(pin <= PC_INT20_PIN)) {
		PCMSK2 &= ~INT_PIN_MASK(pin);
	} else
	if((pin >= PC_INT8_PIN)&&(pin <= PC_INT14_PIN)) {
		PCMSK1 &= ~INT_PIN_MASK(pin);
	} else
	if((pin >= PC_INT0_PIN)&&(pin <= PC_INT7_PIN)) {
		PCMSK0 &= ~INT_PIN_MASK(pin);
	}
}

void ext_int_clear_flag(ioport_pin_t pin)
{
	CLEAR_INT_FLAG(pin);
}

void ext_int_disable(ioport_pin_t pin)
{
	EIMSK &= ~INT_PIN_MASK(pin);
}

void ext_int_enable(ioport_pin_t pin)
{
	EIMSK |= INT_PIN_MASK(pin);
}

void ext_int_init(ioport_pin_t pin, enum ioport_sense trigmode)
{
	/* Disable the interrupt */
	ext_int_disable(pin);
	
	/* Enable IRQ pin as input */
	/* Enable the pullup for the IRQ pin */
	ioport_configure_pin(pin, IOPORT_DIR_INPUT | IOPORT_PULL_UP);

    /* Setup interrupt sence control */
	ioport_set_pin_sense_mode(pin,trigmode);
	
	/* Clear the INTn interrupt flag */
	ext_int_clear_flag(pin);

	/* Enable the interrupt */
	ext_int_enable(pin);
}

void ext_int_pcint_init(ioport_pin_t pin)
{
	/* Disable the interrupt */
	ext_int_pcint_disable(pin);

	/* Enable IRQ pin as input */
	/* Enable the pullup for the IRQ pin */
	ioport_configure_pin(pin, IOPORT_DIR_INPUT | IOPORT_PULL_UP);

	/* Clear the PCINTn interrupt flag */
	ext_int_pcint_clear_flag(pin);

	/* Enable the interrupt */
	ext_int_pcint_enable(pin);
}

void ext_int_set_interrupt_callback(ioport_pin_t ext_int, ext_int_callback_t callback)
{
	if (ext_int == EXT_INT0_PIN) {
		ext_int_int0_callback = callback;
	} else
	if (ext_int == EXT_INT1_PIN) {
		ext_int_int1_callback = callback;
	} else
	if (ext_int >= PC_INT0_PIN && ext_int <=PC_INT7_PIN) {
		ext_int_pcint0_callback = callback;
	} else
	if (ext_int >= PC_INT8_PIN && ext_int <=PC_INT14_PIN) {
		ext_int_pcint1_callback = callback;
	} else
	if (ext_int >= PC_INT16_PIN && ext_int <=PC_INT20_PIN) {
		ext_int_pcint2_callback = callback;
	}
}