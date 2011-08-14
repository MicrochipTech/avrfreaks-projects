/**
 * \file
 *
 * \brief Chip-specific system clock management functions
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
#ifndef XMEGA_SYSCLK_H_INCLUDED
#define XMEGA_SYSCLK_H_INCLUDED

#include <board.h>
#include <compiler.h>
#include <parts.h>
#include <ccp.h>
#include <osc.h>
#include <pll.h>

// Include clock configuration for the project.
#include <conf_clock.h>

#ifdef __cplusplus
extern "C" {
#endif

// Use 2 MHz with no prescaling if config was empty.
#ifndef CONFIG_SYSCLK_SOURCE
# define CONFIG_SYSCLK_SOURCE    SYSCLK_SRC_RC2MHZ
#endif /* CONFIG_SYSCLK_SOURCE */

#ifndef CONFIG_SYSCLK_PSADIV
# define CONFIG_SYSCLK_PSADIV    SYSCLK_PSADIV_1
#endif /* CONFIG_SYSCLK_PSADIV */

#ifndef CONFIG_SYSCLK_PSBCDIV
# define CONFIG_SYSCLK_PSBCDIV   SYSCLK_PSBCDIV_1_1
#endif /* CONFIG_SYSCLK_PSBCDIV */

/**
 * \weakgroup sysclk_group
 *
 * \section sysclk_group_config Configuration Symbols
 *
 * The following configuration symbols may be used to specify the
 * initial system clock configuration. If any of the symbols are not
 * set, reasonable defaults will be provided.
 *   - \b CONFIG_SYSCLK_SOURCE: The initial system clock source.
 *   - \b CONFIG_SYSCLK_PSADIV: The initial Prescaler A setting.
 *   - \b CONFIG_SYSCLK_PSBCDIV: The initial Prescaler B setting.
 *
 * @{
 */

//! \name System Clock Sources
//@{
//! Internal 2 MHz RC oscillator
#define SYSCLK_SRC_RC2MHZ    CLK_SCLKSEL_RC2M_gc
//! Internal 32 MHz RC oscillator
#define SYSCLK_SRC_RC32MHZ   CLK_SCLKSEL_RC32M_gc
//! Internal 32 KHz RC oscillator
#define SYSCLK_SRC_RC32KHZ   CLK_SCLKSEL_RC32K_gc
//! External oscillator
#define SYSCLK_SRC_XOSC      CLK_SCLKSEL_XOSC_gc
//! Phase-Locked Loop
#define SYSCLK_SRC_PLL       CLK_SCLKSEL_PLL_gc
//@}

//! \name Prescaler A Setting (relative to CLKsys)
//@{
#define SYSCLK_PSADIV_1      CLK_PSADIV_1_gc      //!< Do not prescale
#define SYSCLK_PSADIV_2      CLK_PSADIV_2_gc      //!< Prescale CLKper4 by 2
#define SYSCLK_PSADIV_4      CLK_PSADIV_4_gc      //!< Prescale CLKper4 by 4
#define SYSCLK_PSADIV_8      CLK_PSADIV_8_gc      //!< Prescale CLKper4 by 8
#define SYSCLK_PSADIV_16     CLK_PSADIV_16_gc     //!< Prescale CLKper4 by 16
#define SYSCLK_PSADIV_32     CLK_PSADIV_32_gc     //!< Prescale CLKper4 by 32
#define SYSCLK_PSADIV_64     CLK_PSADIV_64_gc     //!< Prescale CLKper4 by 64
#define SYSCLK_PSADIV_128    CLK_PSADIV_128_gc    //!< Prescale CLKper4 by 128
#define SYSCLK_PSADIV_256    CLK_PSADIV_256_gc    //!< Prescale CLKper4 by 256
#define SYSCLK_PSADIV_512    CLK_PSADIV_512_gc    //!< Prescale CLKper4 by 512
//@}

//! \name Prescaler B and C Setting (relative to CLKper4)
//@{
//! Do not prescale
#define SYSCLK_PSBCDIV_1_1   CLK_PSBCDIV_1_1_gc
//! Prescale CLKper and CLKcpu by 2
#define SYSCLK_PSBCDIV_1_2   CLK_PSBCDIV_1_2_gc
//! Prescale CLKper2, CLKper and CLKcpu by 4
#define SYSCLK_PSBCDIV_4_1   CLK_PSBCDIV_4_1_gc
//! Prescale CLKper2 by 2, CLKper and CLKcpu by 4
#define SYSCLK_PSBCDIV_2_2   CLK_PSBCDIV_2_2_gc
//@}

//! \name System Clock Port Numbers
enum sysclk_port_id {
	SYSCLK_PORT_GEN,   //!< Devices not associated with a specific port.
	SYSCLK_PORT_A,     //!< Devices on PORTA
	SYSCLK_PORT_B,     //!< Devices on PORTB
	SYSCLK_PORT_C,     //!< Devices on PORTC
	SYSCLK_PORT_D,     //!< Devices on PORTD
	SYSCLK_PORT_E,     //!< Devices on PORTE
	SYSCLK_PORT_F,     //!< Devices on PORTF
};

/*! \name Clocks not associated with any port
 *
 * \note See the datasheet for available modules in the device.
 */
//@{
#define SYSCLK_DMA        PR_DMA_bm     //!< DMA Controller
#define SYSCLK_EVSYS      PR_EVSYS_bm   //!< Event System
#define SYSCLK_RTC        PR_RTC_bm     //!< Real-Time Counter
#define SYSCLK_EBI        PR_EBI_bm     //!< Ext Bus Interface
#define SYSCLK_AES        PR_AES_bm     //!< AES Module
//@}

/*! \name Clocks on PORTA and PORTB
 *
 * \note See the datasheet for available modules in the device.
 */
//@{
#define SYSCLK_AC         PR_AC_bm      //!< Analog Comparator
#define SYSCLK_ADC        PR_ADC_bm     //!< A/D Converter
#define SYSCLK_DAC        PR_DAC_bm     //!< D/A Converter
//@}

/*! \name Clocks on PORTC, PORTD, PORTE and PORTF
 *
 * \note See the datasheet for available modules in the device.
 */
//@{
#define SYSCLK_TC0        PR_TC0_bm      //!< Timer/Counter 0
#define SYSCLK_TC1        PR_TC1_bm      //!< Timer/Counter 1
#define SYSCLK_HIRES      PR_HIRES_bm    //!< Hi-Res Extension
#define SYSCLK_SPI        PR_SPI_bm      //!< SPI controller
#define SYSCLK_USART0     PR_USART0_bm   //!< USART 0
#define SYSCLK_USART1     PR_USART1_bm   //!< USART 1
#define SYSCLK_TWI        PR_TWI_bm      //!< TWI controller
//@}

#ifndef __ASSEMBLY__

/**
 * \name Querying the system clock and its derived clocks
 */
//@{

/**
 * \brief Return the current rate in Hz of the main system clock
 *
 * \todo This function assumes that the main clock source never changes
 * once it's been set up, and that PLL0 always runs at the compile-time
 * configured default rate. While this is probably the most common
 * configuration, which we want to support as a special case for
 * performance reasons, we will at some point need to support more
 * dynamic setups as well.
 *
 * \return Frequency of the main system clock, in Hz.
 */
static inline uint32_t sysclk_get_main_hz(void)
{
	switch (CONFIG_SYSCLK_SOURCE) {
	case SYSCLK_SRC_RC2MHZ:
		return 2000000UL;

	case SYSCLK_SRC_RC32MHZ:
#ifdef CONFIG_OSC_RC32_CAL
		return CONFIG_OSC_RC32_CAL;
#else
		return 32000000UL;
#endif

	case SYSCLK_SRC_RC32KHZ:
		return 32768UL;

#ifdef BOARD_XOSC_HZ
	case SYSCLK_SRC_XOSC:
		return BOARD_XOSC_HZ;
#endif

#ifdef CONFIG_PLL0_SOURCE
	case SYSCLK_SRC_PLL:
		return pll_get_default_rate(0);
#endif

	default:
		//unhandled_case(CONFIG_SYSCLK_SOURCE);
		return 0;
	}
}

/**
 * \brief Return the current rate in Hz of clk_PER4.
 *
 * This clock can run up to four times faster than the CPU clock.
 *
 * \return Frequency of the clk_PER4 clock, in Hz.
 */
static inline uint32_t sysclk_get_per4_hz(void)
{
	uint8_t shift = 0;

	if (CONFIG_SYSCLK_PSADIV & (1U << CLK_PSADIV_gp)) {
		shift = (CONFIG_SYSCLK_PSADIV >> (1 + CLK_PSADIV_gp)) + 1;
	}

	return sysclk_get_main_hz() >> shift;
}

/**
 * \brief Return the current rate in Hz of clk_PER2.
 *
 * This clock can run up to two times faster than the CPU clock.
 *
 * \return Frequency of the clk_PER2 clock, in Hz.
 */
static inline uint32_t sysclk_get_per2_hz(void)
{
	switch (CONFIG_SYSCLK_PSBCDIV) {
	case SYSCLK_PSBCDIV_1_1: /* Fall through */
	case SYSCLK_PSBCDIV_1_2:
		return sysclk_get_per4_hz();

	case SYSCLK_PSBCDIV_4_1:
		return sysclk_get_per4_hz() / 4;

	case SYSCLK_PSBCDIV_2_2:
		return sysclk_get_per4_hz() / 2;

	default:
		//unhandled_case(CONFIG_SYSCLK_PSBCDIV);
		return 0;
	}
}

/**
 * \brief Return the current rate in Hz of clk_PER.
 *
 * This clock always runs at the same rate as the CPU clock unless the divider
 * is set.
 *
 * \return Frequency of the clk_PER clock, in Hz.
 */
static inline uint32_t sysclk_get_per_hz(void)
{
	if (CONFIG_SYSCLK_PSBCDIV & (1U << CLK_PSBCDIV_gp))
		return sysclk_get_per2_hz() / 2;
	else
		return sysclk_get_per2_hz();
}

/**
 * \brief Return the current rate in Hz of the CPU clock.
 *
 * \return Frequency of the CPU clock, in Hz.
 */
static inline uint32_t sysclk_get_cpu_hz(void)
{
	return sysclk_get_per_hz();
}

/**
 * \brief Retrieves the current rate in Hz of the Peripheral Bus clock attached
 *        to the specified peripheral.
 *
 * \param module Pointer to the module's base address.
 *
 * \return Frequency of the bus attached to the specified peripheral, in Hz.
 */
static inline uint32_t sysclk_get_peripheral_bus_hz(const volatile void *module)
{
	if (module == NULL) {
		Assert(false);
		return 0;
	}
#ifdef AES
	else if (module == &AES) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef EBI
	else if (module == &EBI) {
		return sysclk_get_per2_hz();
	}
#endif
#ifdef RTC
	else if (module == &RTC) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef EVSYS
	else if (module == &EVSYS) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef DMA
	else if (module == &DMA) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef ACA
	else if (module == &ACA) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef ACB
	else if (module == &ACB) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef ADCA
	else if (module == &ADCA) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef ADCB
	else if (module == &ADCB) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef DACA
	else if (module == &DACA) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef DACB
	else if (module == &DACB) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef TCC0
	else if (module == &TCC0) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef TCD0
	else if (module == &TCD0) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef TCE0
	else if (module == &TCE0) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef TCF0
	else if (module == &TCF0) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef TCC1
	else if (module == &TCC1) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef TCD1
	else if (module == &TCD1) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef TCE1
	else if (module == &TCE1) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef TCF1
	else if (module == &TCF1) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef HIRESC
	else if (module == &HIRESC) {
		return sysclk_get_per4_hz();
	}
#endif
#ifdef HIRESD
	else if (module == &HIRESD) {
		return sysclk_get_per4_hz();
	}
#endif
#ifdef HIRESE
	else if (module == &HIRESE) {
		return sysclk_get_per4_hz();
	}
#endif
#ifdef HIRESF
	else if (module == &HIRESF) {
		return sysclk_get_per4_hz();
	}
#endif
#ifdef SPIC
	else if (module == &SPIC) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef SPID
	else if (module == &SPID) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef SPIE
	else if (module == &SPIE) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef SPIF
	else if (module == &SPIF) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef USART0C
	else if (module == &USART0C) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef USART0D
	else if (module == &USART0D) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef USART0E
	else if (module == &USART0E) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef USART0F
	else if (module == &USART0F) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef USART1C
	else if (module == &USART1C) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef USART1D
	else if (module == &USART1D) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef USART1E
	else if (module == &USART1E) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef USART1F
	else if (module == &USART1F) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef TWIC
	else if (module == &TWIC) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef TWID
	else if (module == &TWID) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef TWIE
	else if (module == &TWIE) {
		return sysclk_get_per_hz();
	}
#endif
#ifdef TWIF
	else if (module == &TWIF) {
		return sysclk_get_per_hz();
	}
#endif
	else {
		Assert(false);
		return 0;
	}
}

//@}

//! \name Enabling and disabling synchronous clocks
//@{

/**
 * \brief Enable the clock to peripheral \a id on port \a port
 *
 * \param port ID of the port to which the module is connected (one of
 * the \c SYSCLK_PORT_* definitions).
 * \param id The ID (bitmask) of the peripheral module to be enabled.
 */
extern void sysclk_enable_module(enum sysclk_port_id port, uint8_t id);

/**
 * \brief Disable the clock to peripheral \a id on port \a port
 *
 * \param port ID of the port to which the module is connected (one of
 * the \c SYSCLK_PORT_* definitions).
 * \param id The ID (bitmask) of the peripheral module to be disabled.
 */
extern void sysclk_disable_module(enum sysclk_port_id port, uint8_t id);

/**
 * \brief Enable a peripheral's clock from its base address.
 *
 *  Enables the clock to a peripheral, given its base address. If the peripheral
 *  has an associated clock on the HSB bus, this will be enabled also.
 *
 * \param module Pointer to the module's base address.
 */
static inline void sysclk_enable_peripheral_clock(const volatile void *module)
{
	if (module == NULL) {
		Assert(false);
	}
#ifdef AES
	else if (module == &AES) {
		sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_AES);
	}
#endif
#ifdef EBI
	else if (module == &EBI) {
		sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EBI);
	}
#endif
#ifdef RTC
	else if (module == &RTC) {
		sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_RTC);
	}
#endif
#ifdef EVSYS
	else if (module == &EVSYS) {
		sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS);
	}
#endif
#ifdef DMA
	else if (module == &DMA) {
		sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_DMA);
	}
#endif
#ifdef ACA
	else if (module == &ACA) {
		sysclk_enable_module(SYSCLK_PORT_A, SYSCLK_AC);
	}
#endif
#ifdef ACB
	else if (module == &ACB) {
		sysclk_enable_module(SYSCLK_PORT_B, SYSCLK_AC);
	}
#endif
#ifdef ADCA
	else if (module == &ADCA) {
		sysclk_enable_module(SYSCLK_PORT_A, SYSCLK_ADC);
	}
#endif
#ifdef ADCB
	else if (module == &ADCB) {
		sysclk_enable_module(SYSCLK_PORT_B, SYSCLK_ADC);
	}
#endif
#ifdef DACA
	else if (module == &DACA) {
		sysclk_enable_module(SYSCLK_PORT_A, SYSCLK_DAC);
	}
#endif
#ifdef DACB
	else if (module == &DACB) {
		sysclk_enable_module(SYSCLK_PORT_B, SYSCLK_DAC);
	}
#endif
#ifdef TCC0
	else if (module == &TCC0) {
		sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC0);
	}
#endif
#ifdef TCD0
	else if (module == &TCD0) {
		sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_TC0);
	}
#endif
#ifdef TCE0
	else if (module == &TCE0) {
		sysclk_enable_module(SYSCLK_PORT_E, SYSCLK_TC0);
	}
#endif
#ifdef TCF0
	else if (module == &TCF0) {
		sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_TC0);
	}
#endif
#ifdef TCC1
	else if (module == &TCC1) {
		sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TC1);
	}
#endif
#ifdef TCD1
	else if (module == &TCD1) {
		sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_TC1);
	}
#endif
#ifdef TCE1
	else if (module == &TCE1) {
		sysclk_enable_module(SYSCLK_PORT_E, SYSCLK_TC1);
	}
#endif
#ifdef TCF1
	else if (module == &TCF1) {
		sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_TC1);
	}
#endif
#ifdef HIRESC
	else if (module == &HIRESC) {
			sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_HIRES);
	}
#endif
#ifdef HIRESD
	else if (module == &HIRESD) {
		sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_HIRES);
	}
#endif
#ifdef HIRESE
	else if (module == &HIRESE) {
		sysclk_enable_module(SYSCLK_PORT_E, SYSCLK_HIRES);
	}
#endif
#ifdef HIRESF
	else if (module == &HIRESF) {
		sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_HIRES);
	}
#endif
#ifdef SPIC
	else if (module == &SPIC) {
		sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_SPI);
	}
#endif
#ifdef SPID
	else if (module == &SPID) {
		sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_SPI);
	}
#endif
#ifdef SPIE
	else if (module == &SPIE) {
		sysclk_enable_module(SYSCLK_PORT_E, SYSCLK_SPI);
	}
#endif
#ifdef SPIF
	else if (module == &SPIF) {
		sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_SPI);
	}
#endif
#ifdef USART0C
	else if (module == &USART0C) {
		sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_USART0);
	}
#endif
#ifdef USART0D
	else if (module == &USART0D) {
		sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_USART0);
	}
#endif
#ifdef USART0E
	else if (module == &USART0E) {
		sysclk_enable_module(SYSCLK_PORT_E, SYSCLK_USART0);
	}
#endif
#ifdef USART0F
	else if (module == &USART0F) {
		sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_USART0);
	}
#endif
#ifdef USART1C
	else if (module == &USART1C) {
		sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_USART1);
	}
#endif
#ifdef USART1D
	else if (module == &USART1D) {
		sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_USART1);
	}
#endif
#ifdef USART1E
	else if (module == &USART1E) {
		sysclk_enable_module(SYSCLK_PORT_E, SYSCLK_USART1);
	}
#endif
#ifdef USART1F
	else if (module == &USART1F) {
		sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_USART1);
	}
#endif
#ifdef TWIC
	else if (module == &TWIC) {
		sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_TWI);
	}
#endif
#ifdef TWID
	else if (module == &TWID) {
		sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_TWI);
	}
#endif
#ifdef TWIE
	else if (module == &TWIE) {
		sysclk_enable_module(SYSCLK_PORT_E, SYSCLK_TWI);
	}
#endif
#ifdef TWIF
	else if (module == &TWIF) {
		sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_TWI);
	}
#endif
	else {
		Assert(false);
	}
}

/**
 * \brief Disable a peripheral's clock from its base address.
 *
 *  Disables the clock to a peripheral, given its base address. If the peripheral
 *  has an associated clock on the HSB bus, this will be disabled also.
 *
 * \param module Pointer to the module's base address.
 */
static inline void sysclk_disable_peripheral_clock(const volatile void *module)
{
	if (module == NULL) {
		Assert(false);
	}
#ifdef AES
	else if (module == &AES) {
		sysclk_disable_module(SYSCLK_PORT_GEN, SYSCLK_AES);
	}
#endif
#ifdef EBI
	else if (module == &EBI) {
		sysclk_disable_module(SYSCLK_PORT_GEN, SYSCLK_EBI);
	}
#endif
#ifdef RTC
	else if (module == &RTC) {
		sysclk_disable_module(SYSCLK_PORT_GEN, SYSCLK_RTC);
	}
#endif
#ifdef EVSYS
	else if (module == &EVSYS) {
		sysclk_disable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS);
	}
#endif
#ifdef DMA
	else if (module == &DMA) {
		sysclk_disable_module(SYSCLK_PORT_GEN, SYSCLK_DMA);
	}
#endif
#ifdef ACA
	else if (module == &ACA) {
		sysclk_disable_module(SYSCLK_PORT_A, SYSCLK_AC);
	}
#endif
#ifdef ACB
	else if (module == &ACB) {
		sysclk_disable_module(SYSCLK_PORT_B, SYSCLK_AC);
	}
#endif
#ifdef ADCA
	else if (module == &ADCA) {
		sysclk_disable_module(SYSCLK_PORT_A, SYSCLK_ADC);
	}
#endif
#ifdef ADCB
	else if (module == &ADCB) {
		sysclk_disable_module(SYSCLK_PORT_B, SYSCLK_ADC);
	}
#endif
#ifdef DACA
	else if (module == &DACA) {
		sysclk_disable_module(SYSCLK_PORT_A, SYSCLK_DAC);
	}
#endif
#ifdef DACB
	else if (module == &DACB) {
		sysclk_disable_module(SYSCLK_PORT_B, SYSCLK_DAC);
	}
#endif
#ifdef TCC0
	else if (module == &TCC0) {
		sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_TC0);
	}
#endif
#ifdef TCD0
	else if (module == &TCD0) {
		sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_TC0);
	}
#endif
#ifdef TCE0
	else if (module == &TCE0) {
		sysclk_disable_module(SYSCLK_PORT_E, SYSCLK_TC0);
	}
#endif
#ifdef TCF0
	else if (module == &TCF0) {
		sysclk_disable_module(SYSCLK_PORT_F, SYSCLK_TC0);
	}
#endif
#ifdef TCC1
	else if (module == &TCC1) {
		sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_TC1);
	}
#endif
#ifdef TCD1
	else if (module == &TCD1) {
		sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_TC1);
	}
#endif
#ifdef TCE1
	else if (module == &TCE1) {
		sysclk_disable_module(SYSCLK_PORT_E, SYSCLK_TC1);
	}
#endif
#ifdef TCF1
	else if (module == &TCF1) {
		sysclk_disable_module(SYSCLK_PORT_F, SYSCLK_TC1);
	}
#endif
#ifdef HIRESC
	else if (module == &HIRESC) {
			sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_HIRES);
	}
#endif
#ifdef HIRESD
	else if (module == &HIRESD) {
		sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_HIRES);
	}
#endif
#ifdef HIRESE
	else if (module == &HIRESE) {
		sysclk_disable_module(SYSCLK_PORT_E, SYSCLK_HIRES);
	}
#endif
#ifdef HIRESF
	else if (module == &HIRESF) {
		sysclk_disable_module(SYSCLK_PORT_F, SYSCLK_HIRES);
	}
#endif
#ifdef SPIC
	else if (module == &SPIC) {
		sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_SPI);
	}
#endif
#ifdef SPID
	else if (module == &SPID) {
		sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_SPI);
	}
#endif
#ifdef SPIE
	else if (module == &SPIE) {
		sysclk_disable_module(SYSCLK_PORT_E, SYSCLK_SPI);
	}
#endif
#ifdef SPIF
	else if (module == &SPIF) {
		sysclk_disable_module(SYSCLK_PORT_F, SYSCLK_SPI);
	}
#endif
#ifdef USART0C
	else if (module == &USART0C) {
		sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_USART0);
	}
#endif
#ifdef USART0D
	else if (module == &USART0D) {
		sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_USART0);
	}
#endif
#ifdef USART0E
	else if (module == &USART0E) {
		sysclk_disable_module(SYSCLK_PORT_E, SYSCLK_USART0);
	}
#endif
#ifdef USART0F
	else if (module == &USART0F) {
		sysclk_disable_module(SYSCLK_PORT_F, SYSCLK_USART0);
	}
#endif
#ifdef USART1C
	else if (module == &USART1C) {
		sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_USART1);
	}
#endif
#ifdef USART1D
	else if (module == &USART1D) {
		sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_USART1);
	}
#endif
#ifdef USART1E
	else if (module == &USART1E) {
		sysclk_disable_module(SYSCLK_PORT_E, SYSCLK_USART1);
	}
#endif
#ifdef USART1F
	else if (module == &USART1F) {
		sysclk_disable_module(SYSCLK_PORT_F, SYSCLK_USART1);
	}
#endif
#ifdef TWIC
	else if (module == &TWIC) {
		sysclk_disable_module(SYSCLK_PORT_C, SYSCLK_TWI);
	}
#endif
#ifdef TWID
	else if (module == &TWID) {
		sysclk_disable_module(SYSCLK_PORT_D, SYSCLK_TWI);
	}
#endif
#ifdef TWIE
	else if (module == &TWIE) {
		sysclk_disable_module(SYSCLK_PORT_E, SYSCLK_TWI);
	}
#endif
#ifdef TWIF
	else if (module == &TWIF) {
		sysclk_disable_module(SYSCLK_PORT_F, SYSCLK_TWI);
	}
#endif
	else {
		Assert(false);
	}
}

/**
 * \brief Check if the synchronous clock is enabled for a module
 *
 * \param port ID of the port to which the module is connected (one of
 * the \c SYSCLK_PORT_* definitions).
 * \param id The ID (bitmask) of the peripheral module to check (one of
 * the \c SYSCLK_* module definitions).
 *
 * \retval true If the clock for module \a id on \a port is enabled.
 * \retval false If the clock for module \a id on \a port is disabled.
 */
static inline bool sysclk_module_is_enabled(enum sysclk_port_id port,
		uint8_t id)
{
	uint8_t mask = *((uint8_t *)&PR.PRGEN + port);
	return (mask & id) == 0;
}

//@}

//! \name System Clock Source and Prescaler configuration
//@{

/**
 * \brief Set system clock prescaler configuration
 *
 * This function will change the system clock prescaler configuration to
 * match the parameters.
 *
 * \note The parameters to this function are device-specific.
 *
 * \param psadiv The prescaler A setting (one of the \c SYSCLK_PSADIV_*
 * definitions). This determines the clkPER4 frequency.
 * \param psbcdiv The prescaler B and C settings (one of the \c SYSCLK_PSBCDIV_*
 * definitions). These determine the clkPER2, clkPER and clkCPU frequencies.
 */
static inline void sysclk_set_prescalers(uint8_t psadiv, uint8_t psbcdiv)
{
	ccp_write_io((uint8_t *)&CLK.PSCTRL, psadiv | psbcdiv);
}

/**
 * \brief Change the source of the main system clock.
 *
 * \param src The new system clock source. Must be one of the constants
 * from the <em>System Clock Sources</em> section.
 */
static inline void sysclk_set_source(uint8_t src)
{
	ccp_write_io((uint8_t *)&CLK.CTRL, src);
}

/**
 * \brief Lock the system clock configuration
 *
 * This function will lock the current system clock source and prescaler
 * configuration, preventing any further changes.
 */
static inline void sysclk_lock(void)
{
	ccp_write_io((uint8_t *)&CLK.LOCK, CLK_LOCK_bm);
}

//@}

//! \name System Clock Initialization
//@{

extern void sysclk_init(void);

//@}

#endif /* !__ASSEMBLY__ */

//! @}

#ifdef __cplusplus
}
#endif

#endif /* XMEGA_SYSCLK_H_INCLUDED */
