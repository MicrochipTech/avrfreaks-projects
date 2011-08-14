/**
 * \file
 *
 * \brief Programmable Multilevel Interrupt Controller driver
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
#ifndef PMIC_H
#define PMIC_H

#include <compiler.h>
#include <ccp.h>

/**
 * \defgroup pmic_group Programmable Multilevel Interrupt Controller
 *
 * This is a low-level driver implementation for the AVR XMEGA Programmable
 * Multilevel Interrupt Controller.
 *
 * \note If these functions are used in interrupt service routines (ISRs), any
 * non-ISR code or ISR code for lower level interrupts must ensure that the
 * operations are atomic, i.e., by disabling interrupts during the function
 * calls.
 * @{
 */

/**
 * \brief Interrupt level bitmasks
 *
 * \note These may be OR'ed, e.g., if multiple levels are to be enabled or
 * disabled.
 */
enum pmic_level {
	PMIC_LVL_LOW    = PMIC_LOLVLEN_bm,    //!< Low-level interrupts
	PMIC_LVL_MEDIUM = PMIC_MEDLVLEN_bm,   //!< Medium-level interrupts
	PMIC_LVL_HIGH   = PMIC_HILVLEN_bm,    //!< High-level interrupts
	/**
	 * \brief Non-maskable interrupts
	 * \note These cannot be enabled nor disabled.
	 */
	PMIC_LVL_NMI    = PMIC_NMIEX_bp,
};

//! Interrupt vector locations
enum pmic_vector {
	PMIC_VEC_APPLICATION,    //!< Application section
	PMIC_VEC_BOOT,           //!< Boot section
	PMIC_NR_OF_VECTORS,      //!< Number of interrupt vector locations
};

//! Interrupt scheduling schemes
enum pmic_schedule {
	PMIC_SCH_FIXED_PRIORITY, //!< Default, fixed priority scheduling
	PMIC_SCH_ROUND_ROBIN,    //!< Round-robin scheduling
	PMIC_NR_OF_SCHEDULES,    //!< Number of interrupt scheduling schemes
};

/**
 * \brief Initialize the PMIC
 *
 * Enables all interrupt levels, with vectors located in the application section
 * and fixed priority scheduling.
 */
static inline void pmic_init(void)
{
	PMIC.CTRL = PMIC_LVL_LOW | PMIC_LVL_MEDIUM |
			PMIC_LVL_HIGH;
}

/**
 * \brief Enable interrupts with specified \a level(s).
 *
 * \param level Interrupt level(s) to enable.
 */
static inline void pmic_enable_level(enum pmic_level level)
{
	Assert(!(level & PMIC_LVL_NMI));

	PMIC.CTRL |= level;
}

/**
 * \brief Disable interrupts with specified \a level(s).
 *
 * \param level Interrupt level(s) to disable.
 */
static inline void pmic_disable_level(enum pmic_level level)
{
	Assert(!(level & PMIC_LVL_NMI));

	PMIC.CTRL &= ~level;
}

/**
 * \brief Check if specified interrupt \a level(s) is enabled.
 *
 * \param level Interrupt level(s) to check.
 *
 * \return True if interrupt level(s) is enabled.
 */
static inline bool pmic_level_is_enabled(enum pmic_level level)
{
	Assert(!(level & PMIC_LVL_NMI));

	return PMIC.CTRL & level;
}

/**
 * \brief Get currently enabled level(s)
 *
 * \return Bitmask with currently enabled levels.
 */
static inline enum pmic_level pmic_get_enabled_levels(void)
{
	return (enum pmic_level)(PMIC.CTRL & (PMIC_LVL_LOW | PMIC_LVL_MEDIUM
				| PMIC_LVL_HIGH));
}

/**
 * \brief Check if an interrupt level(s) is currently executing.
 *
 * \param level Interrupt level(s) to check.
 *
 * \return True if interrupt level(s) is currently executing.
 */
static inline bool pmic_level_is_executing(enum pmic_level level)
{
	return PMIC.STATUS & level;
}

/**
 * \brief Set interrupt scheduling for low-level interrupts.
 *
 * \param schedule Interrupt scheduling method to set.
 *
 * \note The low-priority vector, INTPRI, must be set to 0 when round-robin
 * scheduling is disabled to return to default interrupt priority order.
 */
static inline void pmic_set_scheduling(enum pmic_schedule schedule)
{
	Assert(schedule < PMIC_NR_OF_SCHEDULES);

	switch (schedule) {
	case PMIC_SCH_FIXED_PRIORITY:
		PMIC.CTRL &= ~PMIC_RREN_bm;
		PMIC.INTPRI = 0;
		break;

	case PMIC_SCH_ROUND_ROBIN:
		PMIC.CTRL |= PMIC_RREN_bm;
		break;

	default:
		break;
	};
}

/**
 * \brief Set location of interrupt vectors.
 *
 * \param vector Location to use for interrupt vectors.
 */
static inline void pmic_set_vector_location(enum pmic_vector vector)
{
	uint8_t ctrl = PMIC.CTRL;

	Assert(vector < PMIC_NR_OF_VECTORS);

	switch (vector) {
	case PMIC_VEC_APPLICATION:
		ctrl &= ~PMIC_IVSEL_bm;
		break;

	case PMIC_VEC_BOOT:
		ctrl |= PMIC_IVSEL_bm;
		break;

	default:
		break;
	}

	ccp_write_io((uint8_t*)&PMIC.CTRL, ctrl);
}

//! @}

#endif /* PMIC_H */
