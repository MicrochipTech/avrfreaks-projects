/*
 * interrupt.h
 * libavrutil
 *
 * Created by Árpád Goretity on 11/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef __AVRUTIL_INTERRUPT_H__
#define __AVRUTIL_INTERRUPT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "base.h"

#define avr_interrupt_enable() sei()
#define avr_interrupt_disable() cli()

/*
 * Number of external interrupts
 */

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

#define AVR_INTERRUPTS_NUM 8

#define AVR_INTR_EXT_0 0
#define AVR_INTR_EXT_1 1
#define AVR_INTR_EXT_2 2
#define AVR_INTR_EXT_3 3
#define AVR_INTR_EXT_4 4
#define AVR_INTR_EXT_5 5
#define AVR_INTR_EXT_6 6
#define AVR_INTR_EXT_7 7

#else /* __AVR_ATmega1280__ || __AVR_ATmega2560__ */

#define AVR_INTERRUPTS_NUM 2

#define AVR_INTR_EXT_0 0
#define AVR_INTR_EXT_1 1

#endif /* __AVR_ATmega1280__ || __AVR_ATmega2560__ */

/*
 * Interrupt trigger modes
 */

#define AVR_INTR_MODE_CHANGE  _BV(0)
#define AVR_INTR_MODE_FALLING _BV(1)
#define AVR_INTR_MODE_RISING  (_BV(1) | _BV(0))

/*
 * Interrupt handler function type
 */
 
typedef void (*avr_interrupt_handler)();

/*
 * Set an external interrupt handler
 * If any interrupt handlers were set before, this will overwrite them
 */
void avr_interrupt_set_handler(uint8_t intnum, avr_interrupt_handler hndl, uint8_t mode);

/*
 * Remove a set external interrupt handler
 * If no handler is set for the specified interrupt, does nothing.
 */
void avr_interrupt_remove_handler(uint8_t intnum);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !__AVRUTIL_INTERRUPT_H__ */

