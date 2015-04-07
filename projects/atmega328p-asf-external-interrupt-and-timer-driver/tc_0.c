/**
 * \file
 *
 * \brief AVR MEGA TC Driver
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <stdint.h>

#include "interrupt.h"
#include "compiler.h"
#include "parts.h"

#include "tc.h"
#include "sysclk.h"
#include "status_codes.h"

//! \internal Local storage of Timer Counter TCC0 interrupt callback function
static tc_callback_t tc_tcc0_oca_callback;
static tc_callback_t tc_tcc0_ocb_callback;
static tc_callback_t tc_tcc0_ovf_callback;

static tc_callback_t tc_tcc1_oca_callback;
static tc_callback_t tc_tcc1_ocb_callback;
static tc_callback_t tc_tcc1_ovf_callback;
static tc_callback_t tc_tcc1_ic_callback;

static tc_callback_t tc_tcc2_oca_callback;
static tc_callback_t tc_tcc2_ocb_callback;
static tc_callback_t tc_tcc2_ovf_callback;

/**
 * \internal
 * \brief Interrupt handler for Timer Counter 0 Output Compare A
 *
 * This function will handle interrupt on Timer Counter O Output Compare A and
 * call the callback function.
 */
ISR(TIMER0_COMPA_vect)
{
	if (tc_tcc0_oca_callback) {
		tc_tcc0_oca_callback();
	}
}

ISR(TIMER1_COMPA_vect)
{
	if (tc_tcc1_oca_callback) {
		tc_tcc1_oca_callback();
	}
}

ISR(TIMER2_COMPA_vect)
{
	if (tc_tcc2_oca_callback) {
		tc_tcc2_oca_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter 0 Output Compare B
 *
 * This function will handle interrupt on Timer Counter O Output Compare B and
 * call the callback function.
 */
ISR(TIMER0_COMPB_vect)
{
	if (tc_tcc0_ocb_callback) {
		tc_tcc0_ocb_callback();
	}
}

ISR(TIMER1_COMPB_vect)
{
	if (tc_tcc1_ocb_callback) {
		tc_tcc1_ocb_callback();
	}
}

ISR(TIMER2_COMPB_vect)
{
	if (tc_tcc2_ocb_callback) {
		tc_tcc2_ocb_callback();
	}
}

ISR(TIMER1_CAPT_vect)
{
	if (tc_tcc1_ic_callback) {
		tc_tcc1_ic_callback();
	}
}

/**
 * \internal
 * \brief Interrupt handler for Timer Counter 0 Overflow
 *
 * This function will handle interrupt on Timer Counter O Overflow and
 * call the callback function.
 */
ISR(TIMER0_OVF_vect)
{
	if (tc_tcc0_ovf_callback) {
		tc_tcc0_ovf_callback();
	}
}

ISR(TIMER1_OVF_vect)
{
	if (tc_tcc1_ovf_callback) {
		tc_tcc1_ovf_callback();
	}
}

ISR(TIMER2_OVF_vect)
{
	if (tc_tcc2_ovf_callback) {
		tc_tcc2_ovf_callback();
	}
}

/**
 * \brief Enable TC
 *
 * Enables the TC.
 *
 * \note
 * unmask TC clock (sysclk), but does not configure the TC clock source.
 */
void tc0_enable(void)
{
	PRR = PRR & ~(1 << PRTIM0);
}

void tc1_enable(void)
{
	PRR = PRR & ~(1 << PRTIM1);
}

void tc2_enable(void)
{
	PRR = PRR & ~(1 << PRTIM2);
}

/**
 * \brief Disable TC
 *
 * Disables the TC.
 *
 * \note
 * mask TC clock (sysclk).
 */
void tc0_disable(void)
{
	PRR = PRR | (1 << PRTIM0);
}

void tc1_disable(void)
{
	PRR = PRR | (1 << PRTIM1);
}

void tc2_disable(void)
{
	PRR = PRR | (1 << PRTIM2);
}

/**
 * \brief Set the interrupt callback
 * \param  callback : interrupt callback
 */
void tc0_set_ovf_interrupt_callback(tc_callback_t callback)
{
	irqflags_t iflags = cpu_irq_save();
	
	tc_tcc0_ovf_callback = callback;
	TIFR0 |= (1<<TOV0);
	TIMSK0 |= (1<<TOIE0);
	cpu_irq_restore(iflags);
}

void tc1_set_ovf_interrupt_callback(tc_callback_t callback)
{
	irqflags_t iflags = cpu_irq_save();
	
	tc_tcc1_ovf_callback = callback;
	TIFR1 |= (1<<TOV1);
	TIMSK1 |= (1<<TOIE1);
	cpu_irq_restore(iflags);
}

void tc2_set_ovf_interrupt_callback(tc_callback_t callback)
{
	irqflags_t iflags = cpu_irq_save();
	
	tc_tcc2_ovf_callback = callback;
	TIFR2 |= (1<<TOV2);
	TIMSK2 |= (1<<TOIE2);
	cpu_irq_restore(iflags);
}

/**
 * \brief Unset the interrupt callback
 */
void tc0_unset_ovf_interrupt_callback(void)
{
	irqflags_t iflags = cpu_irq_save();
	
	tc_tcc0_ovf_callback = NULL;
	TIMSK0 = TIMSK0 & ~(1<<TOIE0);
	cpu_irq_restore(iflags);
}

void tc1_unset_ovf_interrupt_callback(void)
{
	irqflags_t iflags = cpu_irq_save();
	
	tc_tcc1_ovf_callback = NULL;
	TIMSK1 = TIMSK1 & ~(1<<TOIE1);
	cpu_irq_restore(iflags);
}

void tc2_unset_ovf_interrupt_callback(void)
{
	irqflags_t iflags = cpu_irq_save();
	
	tc_tcc2_ovf_callback = NULL;
	TIMSK2 = TIMSK2 & ~(1<<TOIE2);
	cpu_irq_restore(iflags);
}

/**
 * \brief Set the interrupt callback
 * \param  callback : interrupt callback
 */
void tc0_set_oca_interrupt_callback(tc_callback_t callback)
{
	irqflags_t iflags = cpu_irq_save();
		
	tc_tcc0_oca_callback = callback;
	TIFR0 |= (1<<OCF0A);
	TIMSK0 |= (1<<OCIE0A);
	cpu_irq_restore(iflags);
}

void tc1_set_oca_interrupt_callback(tc_callback_t callback)
{
	irqflags_t iflags = cpu_irq_save();
		
	tc_tcc1_oca_callback = callback;
	TIFR1 |= (1<<OCF1A);
	TIMSK1 |= (1<<OCIE1A);
	cpu_irq_restore(iflags);
}

void tc2_set_oca_interrupt_callback(tc_callback_t callback)
{
	irqflags_t iflags = cpu_irq_save();
	
	tc_tcc2_oca_callback = callback;
	TIFR2 |= (1<<OCF2A);
	TIMSK2 |= (1<<OCIE2A);
	cpu_irq_restore(iflags);
}

/**
 * \brief Unset the interrupt callback
 */
void tc0_unset_oca_interrupt_callback(void)
{
	irqflags_t iflags = cpu_irq_save();
	
	tc_tcc0_oca_callback = NULL;
	TIMSK0 = TIMSK0 & ~(1<<OCIE0A);
	cpu_irq_restore(iflags);
}

void tc1_unset_oca_interrupt_callback(void)
{
	irqflags_t iflags = cpu_irq_save();
	
	tc_tcc1_oca_callback = NULL;
	TIMSK1 = TIMSK1 & ~(1<<OCIE1A);
	cpu_irq_restore(iflags);
}

void tc2_unset_oca_interrupt_callback(void)
{
	irqflags_t iflags = cpu_irq_save();
	
	tc_tcc2_oca_callback = NULL;
	TIMSK2 = TIMSK2 & ~(1<<OCIE2A);
	cpu_irq_restore(iflags);
}

/**
 * \brief Set the interrupt callback
 * \param  callback : interrupt callback
 */
void tc0_set_ocb_interrupt_callback(tc_callback_t callback)
{
	irqflags_t iflags = cpu_irq_save();
		
	tc_tcc0_ocb_callback = callback;
	TIFR0 |= (1<<OCF0B);
	TIMSK0 |= (1<<OCIE0B);
	cpu_irq_restore(iflags);	
}

void tc1_set_ocb_interrupt_callback(tc_callback_t callback)
{
	irqflags_t iflags = cpu_irq_save();
		
	tc_tcc1_ocb_callback = callback;
	TIFR1 |= (1<<OCF1B);
	TIMSK1 |= (1<<OCIE1B);
	cpu_irq_restore(iflags);	
}

void tc2_set_ocb_interrupt_callback(tc_callback_t callback)
{
	irqflags_t iflags = cpu_irq_save();
	
	tc_tcc2_ocb_callback = callback;
	TIFR2 |= (1<<OCF2B);
	TIMSK2 |= (1<<OCIE2B);
	cpu_irq_restore(iflags);
}

/**
 * \brief Unset the interrupt callback
 */
void tc0_unset_ocb_interrupt_callback(void)
{
	irqflags_t iflags = cpu_irq_save();
	
	tc_tcc0_ocb_callback = NULL;
	TIMSK0 = TIMSK0 & ~(1<<OCIE0B);
	cpu_irq_restore(iflags);
}

void tc1_unset_ocb_interrupt_callback(void)
{
	irqflags_t iflags = cpu_irq_save();
	
	tc_tcc1_ocb_callback = NULL;
	TIMSK1 = TIMSK1 & ~(1<<OCIE1B);
	cpu_irq_restore(iflags);
}

void tc2_unset_ocb_interrupt_callback(void)
{
	irqflags_t iflags = cpu_irq_save();
	
	tc_tcc2_ocb_callback = NULL;
	TIMSK2 = TIMSK2 & ~(1<<OCIE2B);
	cpu_irq_restore(iflags);
}

void tc1_set_ic_interrupt_callback(tc_callback_t callback)
{
	irqflags_t iflags = cpu_irq_save();
	
	tc_tcc1_ic_callback = callback;
	TIFR1 |= (1<<ICF1);
	TIMSK1 |= (1<<ICIE1);
	cpu_irq_restore(iflags);
}

void tc1_unset_ic_interrupt_callback(void)
{
	irqflags_t iflags = cpu_irq_save();
	
	tc_tcc1_ovf_callback = NULL;
	TIMSK1 = TIMSK1 & ~(1<<ICIE1);
	cpu_irq_restore(iflags);
}