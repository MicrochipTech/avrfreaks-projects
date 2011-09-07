/**
 * \file
 *
 * \brief User Interface
 *
 * Copyright (C) 2011 Atmel Corporation. All rights reserved.
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

#include "compiler.h"
#include "conf_usb.h"
#include "udc.h"
#include "udi_hid_kbd.h"
#include "board.h"
#include "gpio.h"
#include "eic.h"
#include "ui.h"
#include "TouchHelper.h"

uint16_t keyMap[] = {KeyVolUp, KeyVolDown, KeyNext, KeyPrev, KeyPlayPause, KeyMute};

// Interrupt on "pin change" from switch to do wakeup on USB
// Note:
// This interrupt is enable when the USB host enable remotewakeup feature
// This interrupt wakeup the CPU if this one is in idle mode
ISR(ui_wakeup_isr, AVR32_GPIO_IRQ_GROUP, 0)
{
	// Clear GPIO interrupt.
	gpio_clear_pin_interrupt_flag(GPIO_PUSH_BUTTON_1);
	// Clear External Interrupt Line else Wakeup event always enabled
	eic_clear_interrupt_line(&AVR32_EIC, EXT_NMI);
	// It is a wakeup then send wakeup USB
	udc_wakeup();
}

void ui_init(void)
{
	irqflags_t flags = cpu_irq_save();
	irq_register_handler(ui_wakeup_isr, AVR32_GPIO_IRQ_0 + (GPIO_PUSH_BUTTON_1 / 8), 0);
	cpu_irq_restore(flags);
	gpio_set_pin_low(LED0_GPIO);
	gpio_set_pin_high(LED1_GPIO);
	gpio_set_pin_high(LED2_GPIO);
	gpio_set_pin_high(LED3_GPIO);
}

void ui_powerdown(void)
{
	gpio_set_pin_high(LED0_GPIO);
	gpio_set_pin_high(LED1_GPIO);
}

//! Structure holding the configuration parameters of the EIC low level driver.
static const eic_options_t eic_options = {
	// Choose External Interrupt Controller Line
	.eic_line = EXT_NMI,
	// Enable level-triggered interrupt.
	.eic_mode = EIC_MODE_LEVEL_TRIGGERED,
	// Don't care value because the chosen eic mode is level-triggered.
	.eic_edge = 0,
	// Interrupt will trigger on low-level.
	.eic_level = EIC_LEVEL_LOW_LEVEL,
	// Enable filter.
	.eic_filter = EIC_FILTER_ENABLED,
	// For Wake Up mode, initialize in asynchronous mode
	.eic_async = EIC_ASYNCH_MODE
};

void ui_wakeup_enable(void)
{
	/* configure joystick to produce IT on all state change */
	gpio_enable_pin_interrupt(GPIO_PUSH_BUTTON_1, GPIO_PIN_CHANGE);
	/*
	 * Configure pin change interrupt for asynchronous wake-up (required to
	 * wake up from the STATIC sleep mode).
	 *
	 * First, map the interrupt line to the GPIO pin with the right
	 * peripheral function.
	 */
	gpio_enable_module_pin(GPIO_PUSH_BUTTON_1, AVR32_EIC_EXTINT_0_FUNCTION);

	/*
	 * Enable the internal pull-up resistor on that pin (because the EIC is
	 * configured such that the interrupt will trigger on low-level, see
	 * eic_options.eic_level).
	 */
	gpio_enable_pin_pull_up(GPIO_PUSH_BUTTON_1);

	// Init the EIC controller with the set options.
	eic_init(&AVR32_EIC, &eic_options, sizeof(eic_options) /
			sizeof(eic_options_t));

	// Enable External Interrupt Controller Line
	eic_enable_line(&AVR32_EIC, EXT_NMI);
}

void ui_wakeup_disable(void)
{
	eic_disable_line(&AVR32_EIC, EXT_NMI);
	/* Disable joystick input change ITs. */
	gpio_disable_pin_interrupt(GPIO_PUSH_BUTTON_1);
}


void ui_wakeup(void)
{
	gpio_set_pin_low(LED0_GPIO);
}

void ui_process(uint16_t framenumber)
{
	uint8_t i;
	uint16_t mediaButtons;
	uint8_t touchButtons;
	static uint8_t oldTouchButtons;

	mediaButtons = 0;
	if ((framenumber % 1000) == 0)
		gpio_set_pin_low(LED1_GPIO);
	if ((framenumber % 1000) == 500)
		gpio_set_pin_high(LED1_GPIO);
	
	//get touch buttons
	touchButtons = touch_get_keys();
	
	//only send if there is a change!
	if (oldTouchButtons != touchButtons)
	{
		for(i = 0; i < 6; i++)
		{
			//re-map keys to the descriptor positions
			if((touchButtons >> i) & 0x01)
				mediaButtons |= keyMap[i];
		}
	
		//send
		udi_hid_media_send(mediaButtons);
	}
	oldTouchButtons = touchButtons;	

}

void ui_kbd_led(uint8_t value)
{
	if (value&HID_LED_NUM_LOCK) {
		gpio_set_pin_low(LED2_GPIO);
	} else {
		gpio_set_pin_high(LED2_GPIO);
	}
	if (value&HID_LED_CAPS_LOCK) {
		gpio_set_pin_low(LED3_GPIO);
	} else {
		gpio_set_pin_high(LED3_GPIO);
	}
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on STK600:
 * - Led 0 is on when USB line is in IDLE mode, and off in SUSPEND mode
 * - Led 1 blinks when USB Host have checked and enabled HID Keyboard interface
 * - Led 2 displays numeric lock status.
 * - Led 3 displays caps lock status.
 * - The switch 0 open a note pad application on Windows O.S.
 * and send key sequence "Atmel AVR USB Keyboard"
 * - The switch 0 can be used to wakeup USB Host in remote wakeup mode.
 *
 * Setup for STK600:
 * - LEDS connector is connected to PORTA
 * - SWITCHES are connected to PORTB
 * - Warning! The AREF1 jumper must be removed 
 * because AREF1 connected on PORTB0 overrides switch 0.
 */
