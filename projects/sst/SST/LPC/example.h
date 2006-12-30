#ifndef ARM_EXAMPLE_H_
#define ARM_EXAMPLE_H_

/**************************************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 *************************************************************************************************/

#include <lpc2114.h>

#define UART_INT_LOCK()     do { VICIntEnable &= ~(1 << UART0_INT); } while (0)
#define UART_INT_UNLOCK()   do { VICIntEnable = (1 << UART0_INT); } while (0)

#define LED7    (1 << 23)
#define LED6    (1 << 22)
#define LED5    (1 << 21)
#define LED4    (1 << 20)
#define LED3    (1 << 19)
#define LED2    (1 << 18)
#define LED1    (1 << 17)
#define LED0    (1 << 16)

#define EINT1   (1 << 14)


#define IS_BTN_PRESS()      (0 == (GPIO0_IOPIN & EINT1))

#define LED_OFF(_led_)      do { GPIO1_IOCLR = _led_; } while (0)
#define LED_ON(_led_)      do { GPIO1_IOSET = _led_; } while (0)

#define LED_TOGGLE(_led_) \
    do { \
        if (GPIO1_IOPIN & (_led_)) \
        { \
            LED_OFF(_led_); \
        } \
        else \
        { \
            LED_ON(_led_); \
        } \
    } while (0)


#define TIMER100MS_ON()    do { T0_IR = 0x1; T0_TCR = 0x1; } while (0)
#define TIMER100MS_OFF()   do { T0_TCR = 0x0; } while (0)

void ioinit(void);


#endif /*ARM_EXAMPLE_H_*/
