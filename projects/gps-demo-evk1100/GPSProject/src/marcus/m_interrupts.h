#ifndef M_INTERRUPTS_H_
#define M_INTERRUPTS_H_


#include <avr32/io.h>
#include "compiler.h"

// ============== FUNCTIONS ==================
extern void enable_usart_interrupts(volatile avr32_usart_t *usart, int enableTX, int enableRX);

// =============== ISR ========================
__attribute__((__interrupt__)) void uart_interrupt(void);
__attribute__((__interrupt__)) void compare_irq_handler(void);

#endif /*M_INTERRUPTS_H_*/
