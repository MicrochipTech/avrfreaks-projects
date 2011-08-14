/**
 * \file
 *
 * \brief AVR XMEGA Sleep manager implementation
 *
 * Copyright (C) 2010 Atmel Corporation. All rights reserved.
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
#ifndef XMEGA_SLEEPMGR_H
#define XMEGA_SLEEPMGR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <compiler.h>
#include <conf_sleepmgr.h>
#include <sleep.h>

/**
 * \weakgroup sleepmgr_group
 * @{
 */

enum sleepmgr_mode {
	//! Active mode.
	SLEEPMGR_ACTIVE,
	//! Idle mode.
	SLEEPMGR_IDLE,
	//! Extended Standby mode.
	SLEEPMGR_ESTDBY,
	//! Power Save mode.
	SLEEPMGR_PSAVE,
	//! Standby mode.
	SLEEPMGR_STDBY,
	//! Power Down mode.
	SLEEPMGR_PDOWN,
	SLEEPMGR_NR_OF_MODES,
};

/**
 * \internal
 * \name Internal arrays
 * @{
 */
#if defined(CONFIG_SLEEPMGR_ENABLE) || defined(__DOXYGEN__)
//! Sleep mode lock counters
extern uint8_t sleepmgr_locks[];
#  ifndef __DOXYGEN__
PROGMEM_DECLARE(extern enum SLEEP_SMODE_enum, sleepmgr_configs[]);
#  else
/**
 * \brief Look-up table with sleep mode configurations
 * \note This is located in program memory (Flash) as it is constant.
 */
extern enum SLEEP_SMODE_enum sleepmgr_configs[];
#  endif /* __DOXYGEN__ */
#endif /* CONFIG_SLEEPMGR_ENABLE */
//! @}

static inline void sleepmgr_enter_sleep(void)
{
#ifdef CONFIG_SLEEPMGR_ENABLE
	enum SLEEP_SMODE_enum PROGMEM_PTR_T config_ptr = sleepmgr_configs;
	enum SLEEP_SMODE_enum               config;
	uint8_t                             *lock_ptr = sleepmgr_locks;

	cpu_irq_disable();

	// Return right away if first mode (ACTIVE) is locked.
	if (*lock_ptr) {
		cpu_irq_enable();
		return;
	}
	lock_ptr++;

	// Find first non-zero lock count, starting with the shallowest modes.
	while (!(*lock_ptr)) {
		lock_ptr++;
		config_ptr++;
	}

	Assert((uintptr_t)(lock_ptr - sleepmgr_locks) < SLEEPMGR_NR_OF_MODES);
	config = PROGMEM_READ_BYTE(config_ptr);
	sleep_set_mode(config);
	sleep_enable();

	cpu_irq_enable();
	sleep_enter();

	sleep_disable();
#else
	cpu_irq_enable();
#endif /* CONFIG_SLEEPMGR_ENABLE */
}

//! @}

#ifdef __cplusplus
}
#endif

#endif /* XMEGA_SLEEPMGR_H */
