/**
 * \file
 *
 * \brief AVR XMEGA Real Time Counter driver
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
#include <compiler.h>
#include <sysclk.h>
#include <sleepmgr.h>
#include <rtc.h>

#ifdef CONFIG_RTC_OVERFLOW_INT_LEVEL
# define RTC_OVERFLOW_INT_LEVEL CONFIG_RTC_OVERFLOW_INT_LEVEL
#else
# define RTC_OVERFLOW_INT_LEVEL RTC_OVFINTLVL_LO_gc
#endif

#ifdef CONFIG_RTC_COMPARE_INT_LEVEL
# define RTC_COMPARE_INT_LEVEL CONFIG_RTC_COMPARE_INT_LEVEL
#else
# define RTC_COMPARE_INT_LEVEL RTC_COMPINTLVL_LO_gc
#endif

/**
 * \internal
 * \brief Driver private struct
 */
struct rtc_data_struct {
	//! High value of counter
	uint16_t counter_high;
	//! High value of alarm time
	uint16_t alarm_high;
	//! Low value of alarm time
	uint16_t alarm_low;
	//! Callback function to use on alarm
	rtc_callback_t callback;
};

/**
 * \internal
 * \brief Driver private data
 */
struct rtc_data_struct rtc_data;

/**
 * \internal
 * \brief Check if RTC is busy synchronizing
 */
static bool rtc_is_busy(void)
{
	return RTC.STATUS & RTC_SYNCBUSY_bm;
}

/**
 * \brief Set current time
 *
 * \param time Time value to set
 */
void rtc_set_time(uint32_t time)
{
	RTC.CTRL = RTC_PRESCALER_OFF_gc;
	while (rtc_is_busy());
	RTC.CNT = time;
	rtc_data.counter_high = time >> 16;
	RTC.CTRL = CONFIG_RTC_PRESCALER;
}

/**
 * \brief Get current time
 *
 * \return Current time value
 *
 * \note Due to errate, this can return old values shortly after waking up from
 * sleep.
 */
uint32_t rtc_get_time(void)
{
	irqflags_t flags;
	uint16_t   count_high;
	uint16_t   count_low;

	flags = cpu_irq_save();
	count_high = rtc_data.counter_high;
	count_low = RTC.CNT;
	// Test for possible pending increase of high count value
	if ((count_low == 0) && (RTC.INTFLAGS & RTC_OVFIF_bm))
		count_high++;
	cpu_irq_restore(flags);
	return ((uint32_t)count_high << 16) | count_low;
}

/**
 * \brief Set alarm time
 *
 * Will set absolute alarm time that will call the callback specifed by \ref
 * rtc_set_callback on completion. Or possibly use \ref
 * rtc_alarm_has_triggered to check for it.
 *
 * Any pending alarm will be overwritten with this function.
 *
 * \param time Absolute time value. See also \ref rtc_min_alarm_time
 * \pre Needs interrupts disabled if used from several contexts
 */
void rtc_set_alarm(uint32_t time)
{
	RTC.INTCTRL = RTC_OVERFLOW_INT_LEVEL;
	while (rtc_is_busy());
	RTC.COMP = time;
	rtc_data.alarm_low = time;
	rtc_data.alarm_high = time >> 16;
	while (rtc_is_busy());
	RTC.INTCTRL = (uint8_t)RTC_COMPARE_INT_LEVEL
		| (uint8_t)RTC_OVERFLOW_INT_LEVEL;
}

/**
 * \brief Check if pending alarm have triggered
 *
 * \retval true  Alarm have triggered
 * \retval false Alarm is pending
 */
bool rtc_alarm_has_triggered(void)
{
	// Interrupt enable is used on pending alarm
	return !(RTC.INTCTRL & RTC_COMPARE_INT_LEVEL);
}

/**
 * \brief Set callback to call on alarm
 *
 * \param callback Callback function pointer
 */
void rtc_set_callback(rtc_callback_t callback)
{
	rtc_data.callback = callback;
}

/**
 * \brief Initialize the RTC
 *
 * Start up the RTC and start counting from 0
 */
void rtc_init(void)
{
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_RTC);
	CLK.RTCCTRL = CONFIG_RTC_CLOCK_SOURCE | CLK_RTCEN_bm;
	RTC.PER = 0xffff;
	RTC.CNT = 0;
	/* Since overflow interrupt is needed all the time we limit sleep to
	 * power-save.
	 */
	sleepmgr_lock_mode(SLEEPMGR_PSAVE);
	RTC.INTCTRL = RTC_OVERFLOW_INT_LEVEL;
	RTC.CTRL = CONFIG_RTC_PRESCALER;
}

/**
 * \internal
 * \brief Overflow interrupt handling high counter
 */
ISR(RTC_OVF_vect)
{
	rtc_data.counter_high++;
}

/**
 * \internal
 * \brief Compare interrupt used for alarm
 */
ISR(RTC_COMP_vect)
{
	if (rtc_data.counter_high >= rtc_data.alarm_high) {
		RTC.INTCTRL = RTC_OVERFLOW_INT_LEVEL;
		if (rtc_data.callback) {
			uint32_t count = ((uint32_t)rtc_data.counter_high << 16)
					| RTC.CNT;
			uint32_t alarm = ((uint32_t)rtc_data.alarm_high << 16)
					| rtc_data.alarm_low;
			/* Workaround for errata. Count might not be updated
			 * when waking up from sleep, so in this case use alarm
			 * time pluss one.
			 */
			if (alarm >= count)
				count = alarm + 1;
			rtc_data.callback(count);
		}
	}
}
