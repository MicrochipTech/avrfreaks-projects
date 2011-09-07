/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * AVR Software Framework (ASF).
 */
#include <asf.h>
#include "compiler.h"
#include "preprocessor.h"
#include "board.h"
#include "gpio.h"
#include "sysclk.h"
#include "sleepmgr.h"
#include "conf_usb.h"
#include "udc.h"
#include "udd.h"
#include "ui.h"
#include "cycle_counter.h"
#include "flashc.h"
#include "at42qt1060.h"
#include "TouchHelper.h"

static bool main_b_kbd_enable = false;

int main(void)
{ 
	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize the sleep manager
	sleepmgr_init();

	sysclk_init();
	board_init();
	ui_init();
	ui_powerdown();
	touch_init();
	// Start USB stack to authorize VBus monitoring
	udc_start();

	if (!udc_include_vbus_monitoring()) {
		// VBUS monitoring is not available on this product
		// thereby VBUS has to be considered as present
		main_vbus_action(true);
	}

	// The main loop manages only the power mode
	// because the USB management is done by interrupt
	while (true) {

		sleepmgr_enter_sleep();
		
#ifdef   USB_DEVICE_LOW_SPEED
		// No USB "Keep a live" interrupt available in low speed 
		// to scan keyboard interface then use main loop 
		if (main_b_kbd_enable) {
			static uint16_t virtual_sof_sub = 0;
			static uint16_t virtual_sof = 0;
			if (700 == virtual_sof_sub++) {
				virtual_sof_sub = 0;
				ui_process(virtual_sof++);
			}
		}
#endif
	}
}


void main_vbus_action(bool b_high)
{
	if (b_high) {
		// Attach USB Device
		udc_attach();
	} else {
		// VBUS not present
		udc_detach();
	}
}

void main_suspend_action(void)
{
	ui_powerdown();
}

void main_resume_action(void)
{
	ui_wakeup();
}

void main_sof_action(void)
{
	if (!main_b_kbd_enable)
		return;
	ui_process(udd_get_frame_number());
}

void main_remotewakeup_enable(void)
{
	ui_wakeup_enable();
}

void main_remotewakeup_disable(void)
{
	ui_wakeup_disable();
}

bool main_kbd_enable(void)
{
	main_b_kbd_enable = true;
	return true;
}

void main_kbd_disable(void)
{
	main_b_kbd_enable = false;
}