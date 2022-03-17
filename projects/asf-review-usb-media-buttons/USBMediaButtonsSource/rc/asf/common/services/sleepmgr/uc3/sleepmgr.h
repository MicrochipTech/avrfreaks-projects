/**
 * \file
 *
 * \brief AVR UC3 Sleep manager implementation
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
#ifndef UC3_SLEEPMGR_H
#define UC3_SLEEPMGR_H

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
	//! Frozen mode.
	SLEEPMGR_FROZEN,
	//! Standby mode.
	SLEEPMGR_STDBY,
	//! Stop mode.
	SLEEPMGR_STOP,
	//! Deep Stop mode.
	SLEEPMGR_DEEPSTOP,
	//! Static mode.
	SLEEPMGR_STATIC,
#if UC3L
	//! Shutdown mode.
	SLEEPMGR_SHUTDOWN,
#endif
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
#endif /* CONFIG_SLEEPMGR_ENABLE */
//! @}

static inline void sleepmgr_enter_sleep(void)
{
#ifdef CONFIG_SLEEPMGR_ENABLE
	int                   sleep_mode = -1; // -1 representing the ACTIVE mode
	uint8_t               *lock_ptr = sleepmgr_locks;

	cpu_irq_disable();

	// Find first non-zero lock count, starting with the shallowest modes.
	while(!(*lock_ptr++)) {
		sleep_mode++;
	}
	// Catch the case where one too many sleepmgr_unlock_mode() call has been
	// performed on the deepest sleep mode.
	Assert((uintptr_t)(lock_ptr - sleepmgr_locks) < SLEEPMGR_NR_OF_MODES);

	if(sleep_mode >= AVR32_PM_SMODE_IDLE) {
		// Atomically enable the global interrupts and enter the sleep mode.
		pm_sleep(AVR32_PM_SMODE_GMCLEAR_MASK | sleep_mode);
	}
	else {
		// Case where the ACTIVE mode is locked (i.e. no sleep mode allowed).
		// Enable the interrupts to have the same behavior in all cases.
		cpu_irq_enable();
	}
#else
	cpu_irq_enable();
#endif /* CONFIG_SLEEPMGR_ENABLE */
}

//! @}

#ifdef __cplusplus
}
#endif

#endif /* UC3_SLEEPMGR_H */
