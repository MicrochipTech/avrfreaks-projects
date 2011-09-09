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

#include <compiler.h>

#include <sysclk.h>
#include <osc.h>
#include <pll.h>

#ifdef CONFIG_PLL0_SOURCE
static void sysclk_init_pll(void)
{
	struct pll_config pllcfg;

	switch (CONFIG_PLL0_SOURCE) {
	case PLL_SRC_RC2MHZ:
		break;

	case PLL_SRC_RC32MHZ:
		osc_enable(OSC_ID_RC32MHZ);
		osc_wait_ready(OSC_ID_RC32MHZ);
		break;

	case PLL_SRC_XOSC:
		osc_enable(OSC_ID_XOSC);
		osc_wait_ready(OSC_ID_XOSC);
		break;

	default:
		//unhandled_case(CONFIG_PLL0_SOURCE);
		break;
	}

	pll_config_defaults(&pllcfg, 0);
	pll_enable(&pllcfg, 0);
	pll_wait_for_lock(0);
}
#endif

void sysclk_init(void)
{
	uint8_t *reg = (uint8_t *)&PR.PRGEN;
	uint8_t i;
#ifdef CONFIG_OSC_RC32_CAL
	uint16_t cal;
#endif

	/* Turn off all peripheral clocks that can be turned off. */
	for (i = 0; i <= SYSCLK_PORT_F; i++) {
		*(reg++) = 0xff;
	}

	/* Set up system clock prescalers if different from defaults */
	if ((CONFIG_SYSCLK_PSADIV != SYSCLK_PSADIV_1)
			|| (CONFIG_SYSCLK_PSBCDIV != SYSCLK_PSBCDIV_1_1)) {
		sysclk_set_prescalers(CONFIG_SYSCLK_PSADIV,
				CONFIG_SYSCLK_PSBCDIV);
	}

	/*
	 * Switch to the selected initial system clock source, unless
	 * the default internal 2 MHz oscillator is selected.
	 */
	if (CONFIG_SYSCLK_SOURCE != SYSCLK_SRC_RC2MHZ) {
		bool need_rc2mhz = false;

		switch (CONFIG_SYSCLK_SOURCE) {
		case SYSCLK_SRC_RC32MHZ:
			osc_enable(OSC_ID_RC32MHZ);
			osc_wait_ready(OSC_ID_RC32MHZ);
			break;

		case SYSCLK_SRC_RC32KHZ:
			osc_enable(OSC_ID_RC32KHZ);
			osc_wait_ready(OSC_ID_RC32KHZ);
			break;

		case SYSCLK_SRC_XOSC:
			osc_enable(OSC_ID_XOSC);
			osc_wait_ready(OSC_ID_XOSC);
			break;

#ifdef CONFIG_PLL0_SOURCE
		case SYSCLK_SRC_PLL:
			if (CONFIG_PLL0_SOURCE == PLL_SRC_RC2MHZ) {
				need_rc2mhz = true;
			}
			sysclk_init_pll();
			break;
#endif

		default:
			//unhandled_case(CONFIG_SYSCLK_SOURCE);
			return;
		}

		ccp_write_io((uint8_t *)&CLK.CTRL, CONFIG_SYSCLK_SOURCE);
		Assert(CLK.CTRL == CONFIG_SYSCLK_SOURCE);

#ifdef CONFIG_OSC_AUTOCAL
		osc_enable_autocalibration(CONFIG_OSC_AUTOCAL,CONFIG_OSC_AUTOCAL_REF_OSC);
		if (CONFIG_OSC_AUTOCAL == OSC_ID_RC2MHZ\
			|| CONFIG_OSC_AUTOCAL_REF_OSC == OSC_ID_RC2MHZ) {
				need_rc2mhz = true;
		}
#endif
		
		if (!need_rc2mhz) {
			osc_disable(OSC_ID_RC2MHZ);
		}
	}
}

void sysclk_enable_module(enum sysclk_port_id port, uint8_t id)
{
	irqflags_t flags = cpu_irq_save();

	*((uint8_t *)&PR.PRGEN + port) &= ~id;

	cpu_irq_restore(flags);
}

void sysclk_disable_module(enum sysclk_port_id port, uint8_t id)
{
	irqflags_t flags = cpu_irq_save();

	*((uint8_t *)&PR.PRGEN + port) |= id;

	cpu_irq_restore(flags);
}
