/**
 * \file
 *
 * \brief User board configuration template
 *
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

#define ENABLE_WL    ioport_set_pin_low(IOPORT_CREATE_PIN(PORTR,0))
#define DISABLE_WL   ioport_set_pin_high(IOPORT_CREATE_PIN(PORTR,0))
#define ENABLE_MEM   ioport_set_pin_low(IOPORT_CREATE_PIN(PORTF,4))
#define DISABLE_MEM	 ioport_set_pin_high(IOPORT_CREATE_PIN(PORTF,4))
#define STROBE_ON    ioport_set_pin_high(IOPORT_CREATE_PIN(PORTR,1))
#define STROBE_OFF	 ioport_set_pin_low(IOPORT_CREATE_PIN(PORTR,1))

#endif // CONF_BOARD_H
