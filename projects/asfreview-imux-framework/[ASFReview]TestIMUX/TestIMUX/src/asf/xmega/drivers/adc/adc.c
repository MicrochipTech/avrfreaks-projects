/**
 * \file
 *
 * \brief AVR XMEGA Analog to Digital Converter driver
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
#include <adc.h>

/**
 * \ingroup adc_module_group
 * @{
 */

//! \name ADC interrupt callback function
//@{

#ifdef ADCA

/**
 * \internal
 * \brief ADC A enable counter
 *
 * This is used to ensure that ADC A is not inadvertently disabled when its
 * module or channel configurations are updated.
 */
static uint8_t adca_enable_count;

#  ifdef CONFIG_ADC_CALLBACK_ENABLE

/**
 * \internal
 * \brief ADC A interrupt callback function pointer
 */
static adc_callback_t adca_callback;

/**
 * \internal
 * \brief ISR for channel 0 on ADC A
 *
 * Calls the callback function that has been set for the ADC when the channel's
 * interrupt flag is set, if its interrupt has been enabled.
 */
ISR(ADCA_CH0_vect)
{
	adca_callback(&ADCA, 0, adcch_get_result(&ADCA, 0));
}

#    if XMEGA_A

/**
 * \internal
 * \brief ISR for channel 1 on ADC A
 *
 * Calls the callback function that has been set for the ADC when the channel's
 * interrupt flag is set, if its interrupt has been enabled.
 */
ISR(ADCA_CH1_vect)
{
	adca_callback(&ADCA, 1, adcch_get_result(&ADCA, 1));
}

/**
 * \internal
 * \brief ISR for channel 2 on ADC A
 *
 * Calls the callback function that has been set for the ADC when the channel's
 * interrupt flag is set, if its interrupt has been enabled.
 */
ISR(ADCA_CH2_vect)
{
	adca_callback(&ADCA, 2, adcch_get_result(&ADCA, 2));
}

/**
 * \internal
 * \brief ISR for channel 3 on ADC A
 *
 * Calls the callback function that has been set for the ADC when the channel's
 * interrupt flag is set, if its interrupt has been enabled.
 */
ISR(ADCA_CH3_vect)
{
	adca_callback(&ADCA, 3, adcch_get_result(&ADCA, 3));
}

#    endif /* XMEGA_A */
#  endif /* ADCA */
#endif /* CONFIG_ADC_CALLBACK_ENABLE */

#ifdef ADCB

/**
 * \internal
 * \brief ADC A enable counter
 *
 * This is used to ensure that ADC B is not inadvertently disabled when its
 * module or channel configurations are updated.
 */
static uint8_t adcb_enable_count;

#  ifdef CONFIG_ADC_CALLBACK_ENABLE

/**
 * \internal
 * \brief ADC A interrupt callback function pointer
 */
static adc_callback_t adcb_callback;

/**
 * \internal
 * \brief ISR for channel 0 on ADC B
 *
 * Calls the callback function that has been set for the ADC when the channel's
 * interrupt flag is set, if its interrupt has been enabled.
 */
ISR(ADCB_CH0_vect)
{
	adcb_callback(&ADCB, 0, adcch_get_result(&ADCB, 0));
}

#    if XMEGA_A

/**
 * \internal
 * \brief ISR for channel 1 on ADC B
 *
 * Calls the callback function that has been set for the ADC when the channel's
 * interrupt flag is set, if its interrupt has been enabled.
 */
ISR(ADCB_CH1_vect)
{
	adcb_callback(&ADCB, 1, adcch_get_result(&ADCB, 1));
}

/**
 * \internal
 * \brief ISR for channel 2 on ADC B
 *
 * Calls the callback function that has been set for the ADC when the channel's
 * interrupt flag is set, if its interrupt has been enabled.
 */
ISR(ADCB_CH2_vect)
{
	adcb_callback(&ADCB, 2, adcch_get_result(&ADCB, 2));
}

/**
 * \internal
 * \brief ISR for channel 3 on ADC B
 *
 * Calls the callback function that has been set for the ADC when the channel's
 * interrupt flag is set, if its interrupt has been enabled.
 */
ISR(ADCB_CH3_vect)
{
	adcb_callback(&ADCB, 3, adcch_get_result(&ADCB, 3));
}

#    endif /* XMEGA_A */
#  endif /* ADCB */
#endif /* CONFIG_ADC_CALLBACK_ENABLE */

//@}

/** @} */

/**
 * \internal
 * \brief Enable peripheral clock for ADC
 *
 * Checks if the enable count for the ADC is zero, then increments it. If the
 * count was zero, the peripheral clock is enabled. Otherwise, it is already
 * enabled.
 *
 * \param adc Pointer to ADC module.
 */
static void adc_enable_clock(ADC_t *adc)
{
#ifdef ADCA
	if ((uintptr_t)adc == (uintptr_t)(&ADCA)) {
		Assert(adca_enable_count < 0xff);
		if (!adca_enable_count++) {
			sysclk_enable_module(SYSCLK_PORT_A, SYSCLK_ADC);
		}
	} else
#endif

#ifdef ADCB
	if ((uintptr_t)adc == (uintptr_t)(&ADCB)) {
		Assert(adcb_enable_count < 0xff);
		if (!adcb_enable_count++) {
			sysclk_enable_module(SYSCLK_PORT_B, SYSCLK_ADC);
		}
	} else
#endif

	{
		Assert(0);
	}
}

/**
 * \internal
 * \brief Disable peripheral clock for ADC
 *
 * Decrements the enable count for the ADC, then disables its peripheral clock
 * if the count hit zero. If the count did not hit zero, it indicates the ADC is
 * enabled.
 *
 * \param adc Pointer to ADC module
 */
static void adc_disable_clock(ADC_t *adc)
{
#ifdef ADCA
	if ((uintptr_t)adc == (uintptr_t)(&ADCA)) {
		Assert(adca_enable_count);
		if (!--adca_enable_count) {
			sysclk_disable_module(SYSCLK_PORT_A, SYSCLK_ADC);
		}
	} else
#endif

#ifdef ADCB
	if ((uintptr_t)adc == (uintptr_t)(&ADCB)) {
		Assert(adcb_enable_count);
		if (!--adcb_enable_count) {
			sysclk_disable_module(SYSCLK_PORT_B, SYSCLK_ADC);
		}
	} else
#endif

	{
		Assert(0);
	}
}

/**
 * \brief Enable ADC
 *
 * Enables the ADC and locks IDLE mode for the sleep manager.
 *
 * \param adc Pointer to ADC module
 *
 * \note After enabling the ADC, one dummy conversion should be done to allow
 * for its common mode voltage to settle.
 */
void adc_enable(ADC_t *adc)
{
	irqflags_t flags = cpu_irq_save();
	adc_enable_clock(adc);
	adc->CTRLA |= ADC_ENABLE_bm;
	cpu_irq_restore(flags);

	sleepmgr_lock_mode(SLEEPMGR_IDLE);
}

/**
 * \brief Disable ADC
 *
 * Disables the ADC and unlocks IDLE mode for the sleep manager.
 *
 * \param adc Pointer to ADC module
 */
void adc_disable(ADC_t *adc)
{
	irqflags_t flags = cpu_irq_save();
	adc->CTRLA &= ~ADC_ENABLE_bm;
	adc_disable_clock(adc);
	cpu_irq_restore(flags);

	sleepmgr_unlock_mode(SLEEPMGR_IDLE);
}

/**
 * \brief Check if the ADC is enabled
 *
 * \param adc Pointer to ADC module.
 *
 * \retval true if ADC is enabled.
 * \retval false if ADC is disabled.
 */
bool adc_is_enabled(ADC_t *adc)
{
	/* It is sufficient to return the state of the ADC enable counters
	 * since all driver functions that change the counts are protected
	 * against interrupts and only the enable/disable functions leave the
	 * counts incremented/decremented upon return.
	 */
#ifdef ADCA
	if ((uintptr_t)adc == (uintptr_t)&ADCA) {
		return adca_enable_count;
	} else
#endif

#ifdef ADCB
	if ((uintptr_t)adc == (uintptr_t)&ADCB) {
		return adcb_enable_count;
	} else
#endif

	{
		Assert(0);
		return false;
	}
}

/**
 * \brief Write configuration to ADC module
 *
 * Disables the ADC and flushes its pipeline before writing the specified
 * configuration and factory calibration value to it. If the ADC was enabled
 * upon entry of the function, it is enabled upon function return.
 *
 * \param adc Pointer to ADC module.
 * \param conf Pointer to ADC module configuration.
 */
void adc_write_configuration(ADC_t *adc, const struct adc_config *conf)
{
	uint16_t cal;
	
#if XMEGA_A || XMEGA_D	
	uint8_t  enable;
#endif	
	
	irqflags_t flags;

#ifdef ADCA
	if ((uintptr_t)adc == (uintptr_t)&ADCA) {
		cal = adc_get_calibration_data(ADC_CAL_ADCA);
	} else
#endif

#ifdef ADCB
	if ((uintptr_t)adc == (uintptr_t)&ADCB) {
		cal = adc_get_calibration_data(ADC_CAL_ADCB);
	} else
#endif

	{
		Assert(0);
		return;
	}

	flags = cpu_irq_save();
	adc_enable_clock(adc);
	
#if XMEGA_A || XMEGA_D
	enable = adc->CTRLA & ADC_ENABLE_bm;
#endif
	
	adc->CTRLA = ADC_FLUSH_bm;
	adc->CAL = cal;
	adc->CMP = conf->cmp;
	adc->REFCTRL = conf->refctrl;
	adc->PRESCALER = conf->prescaler;
	adc->EVCTRL = conf->evctrl;
	adc->CTRLB = conf->ctrlb;

#if XMEGA_A
	adc->CTRLA = enable | conf->ctrla;
#elif XMEGA_D
	adc->CTRLA = enable;
#endif

	adc_disable_clock(adc);

	cpu_irq_restore(flags);
}

/**
 * \brief Read configuration from ADC module
 *
 * Reads out the current configuration of the ADC module to the specified
 * buffer.
 *
 * \param adc Pointer to ADC module.
 * \param conf Pointer to ADC module configuration.
 */
void adc_read_configuration(ADC_t *adc, struct adc_config *conf)
{
	irqflags_t flags = cpu_irq_save();

	adc_enable_clock(adc);

#if XMEGA_A
	conf->ctrla = adc->CTRLA & ADC_DMASEL_gm;
#endif

	conf->cmp = adc->CMP;
	conf->refctrl = adc->REFCTRL;
	conf->prescaler = adc->PRESCALER;
	conf->evctrl = adc->EVCTRL;
	conf->ctrlb = adc->CTRLB;

	adc_disable_clock(adc);

	cpu_irq_restore(flags);
}

/**
 * \brief Write configuration to ADC channel
 *
 * Writes the specified configuration to the ADC channel.
 *
 * \param adc Pointer to ADC module.
 * \param ch ADC channel number.
 * \param ch_conf Pointer to ADC channel configuration.
 *
 * \note The specified ADC's callback function must be set before this function
 * is called if callbacks are enabled and interrupts are enabled in the
 * channel configuration.
 */
void adcch_write_configuration(ADC_t *adc, uint8_t ch,
		const struct adc_channel_config *ch_conf)
{
	ADC_CH_t   *adc_ch;
	irqflags_t flags;

	Assert(ch < ADC_NR_OF_CHANNELS);
	adc_ch = &adc->CH0 + ch;

	flags = cpu_irq_save();

#if defined(CONFIG_ADC_CALLBACK_ENABLE) && defined(_ASSERT_ENABLE_)
	if ((adc_ch->INTCTRL & ADC_CH_INTLVL_gm) != ADC_CH_INTLVL_OFF_gc) {
#ifdef ADCA
		if ((uintptr_t)adc == (uintptr_t)&ADCA) {
			Assert(adca_callback);
		} else 
#endif /* ADCA */

#ifdef ADCB
		if ((uintptr_t)adc == (uintptr_t)&ADCB) {
			Assert(adcb_callback);
		} else
#endif /* ADCB */

		{
			Assert(0);
			return;
		}
	}
#endif /* CONFIG_ADC_CALLBACK_ENABLE */

	adc_enable_clock(adc);
	adc_ch->CTRL = ch_conf->ctrl;
	adc_ch->INTCTRL = ch_conf->intctrl;
	adc_ch->MUXCTRL = ch_conf->muxctrl;
	adc_disable_clock(adc);

	cpu_irq_restore(flags);
}

/**
 * \brief Read configuration from ADC channel
 *
 * Reads out the current configuration from the ADC channel to the specified
 * buffer.
 *
 * \param adc Pointer to ADC module.
 * \param ch ADC channel number.
 * \param ch_conf Pointer to ADC channel configuration.
 */
void adcch_read_configuration(ADC_t *adc, uint8_t ch,
		struct adc_channel_config *ch_conf)
{
	ADC_CH_t *adc_ch;
	irqflags_t flags;

	Assert(ch < ADC_NR_OF_CHANNELS);
	adc_ch = &adc->CH0 + ch;

	flags = cpu_irq_save();

	adc_enable_clock(adc);
	ch_conf->ctrl = adc_ch->CTRL;
	ch_conf->intctrl = adc_ch->INTCTRL;
	ch_conf->muxctrl = adc_ch->MUXCTRL;
	adc_disable_clock(adc);

	cpu_irq_restore(flags);
}

#if defined(CONFIG_ADC_CALLBACK_ENABLE) || defined(__DOXYGEN__)

/**
 * \brief Set ADC interrupt callback function
 *
 * Sets a new callback function for interrupts on the specified ADC.
 *
 * \param adc Pointer to ADC module.
 * \param callback Pointer to the callback function to set.
 */
void adc_set_callback(ADC_t *adc, adc_callback_t callback)
{
	irqflags_t flags;

	Assert(callback);

	flags = cpu_irq_save();

#ifdef ADCA
	if ((uintptr_t)adc == (uintptr_t)&ADCA) {
		adca_callback = callback;
	} else
#endif

#ifdef ADCB
	if ((uintptr_t)adc == (uintptr_t)&ADCB) {
		adcb_callback = callback;
	} else
#endif

	{
		Assert(0);
	}

	cpu_irq_restore(flags);
}

#endif /* CONFIG_ADC_CALLBACK_ENABLE */
