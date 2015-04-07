/**
 * \file
 *
 * \brief User board configuration template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

#include <ioport.h>

#define LED0_GPIO	IOPORT_CREATE_PIN(PORTB, 5)
#define SW0_GPIO	IOPORT_CREATE_PIN(PORTB, 7)

#endif // CONF_BOARD_H
