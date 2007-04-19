/** @file timer0.h Timer0 driver header file. */

#ifndef __TIMER0_HEADER__
#define __TIMER0_HEADER__

/*******************************************************************************
* Exported global variables
*******************************************************************************/

/** @def TMR0_INTERRUPT_FLAG Macro for reading the #timer0InterruptFlag
		variable */
#define TMR0_INTERRUPT_FLAG() timer0InterruptFlag

/** @def TMR0_CLEAR_INTERRUPT_FLAG Macro for clearing the #timer0InterruptFlag
		variable */
#define TMR0_CLEAR_INTERRUPT_FLAG() timer0InterruptFlag = FALSE

extern BOOL timer0InterruptFlag;

/*******************************************************************************
* Exported functions
*******************************************************************************/

extern void Timer0Start(void);
extern void Timer0Stop(void);

#endif // __TIMER0_HEADER__
