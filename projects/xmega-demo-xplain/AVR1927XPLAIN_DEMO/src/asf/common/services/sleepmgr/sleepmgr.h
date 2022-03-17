/**
 * \file
 *
 * \brief Sleep manager
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
#ifndef SLEEPMGR_H
#define SLEEPMGR_H

#include <compiler.h>
#include <sleep.h>
#include <parts.h>

#if defined(XMEGA)
# include "xmega/sleepmgr.h"
#elif (defined(__GNUC__) && defined(__AVR32__)) || (defined(__ICCAVR32__) || defined(__AAVR32__))
# include "uc3/sleepmgr.h"
#else
# error Unsupported device.
#endif

/**
 * \defgroup sleepmgr_group Sleep manager
 *
 * The sleep manager is a service for ensuring that the device is not put to
 * sleep in deeper sleep modes than the system (e.g., peripheral drivers,
 * services or the application) allows at any given time.
 *
 * It is based on the use of lock counting for the individual sleep modes, and
 * will put the device to sleep in the shallowest sleep mode that has a non-zero
 * lock count. The drivers/services/application can change these counts by use
 * of \ref sleepmgr_lock_mode and \ref sleepmgr_unlock_mode.
 * Refer to \ref sleepmgr_mode for a list of the sleep modes available for
 * locking, and the device datasheet for information on their effect.
 *
 * The application must supply the file \ref conf_sleepmgr.h.
 *
 * For the sleep manager to be enabled, the symbol \ref CONFIG_SLEEPMGR_ENABLE
 * must be defined, e.g., in \ref conf_sleepmgr.h. If this symbol is not
 * defined, the functions are replaced with dummy functions and no RAM is used.
 *
 * @{
 */

/**
 * \def CONFIG_SLEEPMGR_ENABLE
 * \brief Configuration symbol for enabling the sleep manager
 *
 * If this symbol is not defined, the functions of this service are replaced
 * with dummy functions. This is useful for reducing code size and execution
 * time if the sleep manager is not needed in the application.
 *
 * This symbol may be defined in \ref conf_sleepmgr.h.
 */
#if defined(__DOXYGEN__) && !defined(CONFIG_SLEEPMGR_ENABLE)
#  define CONFIG_SLEEPMGR_ENABLE
#endif

/**
 * \enum sleepmgr_mode
 * \brief Sleep mode locks
 *
 * Identifiers for the different sleep mode locks.
 */

/**
 * \brief Initialize the lock counts
 *
 * Sets all lock counts to 0, except the very last one, which is set to 1. This
 * is done to simplify the algorithm for finding the deepest allowable sleep
 * mode in \ref sleepmgr_enter_sleep.
 */
static inline void sleepmgr_init(void)
{
#ifdef CONFIG_SLEEPMGR_ENABLE
	uint8_t i;

	for (i = 0; i < SLEEPMGR_NR_OF_MODES - 1; i++) {
		sleepmgr_locks[i] = 0;
	}
	sleepmgr_locks[SLEEPMGR_NR_OF_MODES - 1] = 1;
#endif /* CONFIG_SLEEPMGR_ENABLE */
}

/**
 * \brief Increase lock count for a sleep mode
 *
 * Increases the lock count for \a mode to ensure that the sleep manager does
 * not put the device to sleep in the deeper sleep modes.
 *
 * \param mode Sleep mode to lock.
 */
static inline void sleepmgr_lock_mode(enum sleepmgr_mode mode)
{
#ifdef CONFIG_SLEEPMGR_ENABLE
	irqflags_t flags;

	Assert(sleepmgr_locks[mode] < 0xff);

	// Enter a critical section
	flags = cpu_irq_save();

	++sleepmgr_locks[mode];

	// Leave the critical section
	cpu_irq_restore(flags);
#endif /* CONFIG_SLEEPMGR_ENABLE */
}

/**
 * \brief Decrease lock count for a sleep mode
 *
 * Decreases the lock count for \a mode. If the lock count reaches 0, the sleep
 * manager can put the device to sleep in the deeper sleep modes.
 *
 * \param mode Sleep mode to unlock.
 */
static inline void sleepmgr_unlock_mode(enum sleepmgr_mode mode)
{
#ifdef CONFIG_SLEEPMGR_ENABLE
	irqflags_t flags;

	Assert(sleepmgr_locks[mode]);

	// Enter a critical section
	flags = cpu_irq_save();

	--sleepmgr_locks[mode];

	// Leave the critical section
	cpu_irq_restore(flags);
#endif /* CONFIG_SLEEPMGR_ENABLE */
}

/**
 * \fn sleepmgr_enter_sleep
 * \brief Go to sleep in the deepest allowed mode
 *
 * Searches through the sleep mode lock counts, starting at the shallowest sleep
 * mode, until the first non-zero lock count is found. The device is then put to
 * sleep in the sleep mode that corresponds to the lock.
 *
 * \note This function enables interrupts before going to sleep, and will leave
 * them enabled upon return. This also applies if sleep is skipped due to ACTIVE
 * mode being locked.
 */

//! @}

#endif /* SLEEPMGR_H */
