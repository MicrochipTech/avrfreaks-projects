/**
 * \file
 *
 * \brief AVR XMEGA Real Time Counter driver definitions
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
#ifndef DRIVERS_RTC_RTC_H
#define DRIVERS_RTC_RTC_H

#include <compiler.h>
#include <conf_rtc.h>

/**
 * \defgroup rtc_group Real Time Counter (RTC)
 *
 * This is a driver implementation for the XMEGA RTC.
 *
 * \section rtc_min_alarm_time Minimum allowed alarm time
 *
 * If current time is close to a time unit roll over, there is a risk to miss
 * this when using a value of 0.
 *
 * A safe use of this can be in an alarm callback.
 *
 * @{
 */

/**
 * \def CONFIG_RTC_COMPARE_INT_LEVEL
 * \brief Configuration symbol for interrupt level to use on alarm
 *
 * Possible values:
 * - RTC_COMPINTLVL_LO_gc
 * - RTC_COMPINTLVL_MED_gc
 * - RTC_COMPINTLVL_HI_gc
 */
#ifdef __DOXYGEN__
# define CONFIG_RTC_COMPARE_INT_LEVEL
#endif

/**
 * \def CONFIG_RTC_OVERFLOW_INT_LEVEL
 * \brief Configuration symbol for interrupt level to use on overflow
 *
 * Possible values:
 * - RTC_OVFINTLVL_LO_gc
 * - RTC_OVFINTLVL_MED_gc
 * - RTC_OVFINTLVL_HI_gc
 */
#ifdef __DOXYGEN__
# define CONFIG_RTC_OVERFLOW_INT_LEVEL
#endif

/**
 * \def CONFIG_RTC_PRESCALER
 * \brief Configuration symbol for prescaler to use
 *
 * Possible values:
 * - RTC_PRESCALER_DIV1_gc
 * - RTC_PRESCALER_DIV2_gc
 * - RTC_PRESCALER_DIV8_gc
 * - RTC_PRESCALER_DIV16_gc
 * - RTC_PRESCALER_DIV64_gc
 * - RTC_PRESCALER_DIV256_gc
 * - RTC_PRESCALER_DIV1024_gc
 */
#ifdef __DOXYGEN__
# define CONFIG_RTC_PRESCALER
#endif

/**
 * \def CONFIG_RTC_CLOCK_SOURCE
 * \brief Configuration symbol for which clock source to use
 *
 * Possible values:
 * - CLK_RTCSRC_ULP_gc
 * - CLK_RTCSRC_TOSC_gc
 * - CLK_RTCSRC_RCOSC_gc
 * - CLK_RTCSRC_TOSC32_gc
 */
#ifdef __DOXYGEN__
# define CONFIG_RTC_CLOCK_SOURCE
#endif

/**
 * \brief Callback definition for alarm callback
 *
 * \param time The time of the alarm
 */
typedef void (*rtc_callback_t)(uint32_t time);

void rtc_set_callback(rtc_callback_t callback);
void rtc_set_time(uint32_t time);
uint32_t rtc_get_time(void);
void rtc_set_alarm(uint32_t time);
bool rtc_alarm_has_triggered(void);

/**
 * \brief Set alarm relative to current time
 *
 * \param offset Offset to current time. This is minimum value, so the alarm
 * might happen at up to one time unit later. See also \ref
 * rtc_min_alarm_time
 *
 * \note Due to errata, this can be unsafe to do shortly after waking up from
 * sleep.
 */
static inline void rtc_set_alarm_relative(uint32_t offset)
{
        rtc_set_alarm(rtc_get_time() + offset);
}

extern void rtc_init(void);

//! @}

#endif /* RTC_H */
