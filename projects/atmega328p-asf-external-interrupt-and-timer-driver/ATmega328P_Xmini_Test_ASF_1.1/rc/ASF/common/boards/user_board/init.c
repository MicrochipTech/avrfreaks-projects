/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	ioport_init();

	ioport_set_pin_level(LED0_GPIO, IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_dir(LED0_GPIO, IOPORT_DIR_OUTPUT);
	ioport_enable_pin(LED0_GPIO);
	
	ioport_set_pin_dir(SW0_GPIO, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(SW0_GPIO, IOPORT_MODE_PULLUP);
	ioport_enable_pin(SW0_GPIO);
}
