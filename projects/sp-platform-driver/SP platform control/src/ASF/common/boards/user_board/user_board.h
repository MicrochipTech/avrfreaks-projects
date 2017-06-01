/*
 * user_board.h
 *
 * Created: 5/11/2017 5:22:28 PM
 *  Author: Big Daddy
 */ 
#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

// ********************************************* External oscillator settings.
// Osc0 crystal is not mounted by default. Set the following definitions to the
// appropriate values if a custom Osc0 crystal is mounted on your board
//! @}
#define BOARD_OSC0_HZ           16000000
#define BOARD_OSC0_STARTUP_US   2000
#define BOARD_OSC0_IS_XTAL      true

// GPIO pin definitions

#define GRN_LED	AVR32_PIN_PC02
#define RED_LED	AVR32_PIN_PC03

#define QC0		AVR32_PIN_PD11
#define QC1		AVR32_PIN_PD12
#define QC2		AVR32_PIN_PD13
#define QC3		AVR32_PIN_PD14
#define QC4		AVR32_PIN_PD29
#define QC5		AVR32_PIN_PD21

#define QD0		AVR32_PIN_PC17
#define QD1		AVR32_PIN_PC18
#define QD2		AVR32_PIN_PC19
#define QD3		AVR32_PIN_PC20
#define QD4		AVR32_PIN_PC21
#define QD5		AVR32_PIN_PC22

#define DIR0	AVR32_PIN_PB00
#define DIR1	AVR32_PIN_PB01
#define DIR2	AVR32_PIN_PA20
#define DIR3	AVR32_PIN_PA21
#define DIR4	AVR32_PIN_PA22
#define DIR5	AVR32_PIN_PA23

#define PWM0	AVR32_PIN_PC04
#define PWM1	AVR32_PIN_PC05
#define PWM2	AVR32_PIN_PD28
#define PWM3	AVR32_PIN_PD30
#define PWM4	AVR32_PIN_PD03
#define PWM5	AVR32_PIN_PD01

// LED macros
#define LED_ON(x) gpio_set_pin_low(x);
#define LED_OFF(x) gpio_set_pin_high(x);
#define LED_TOGGLE(x) gpio_toggle_pin(x);

// local or extern?
#ifdef _USER_BOARD_
#define GLOBAL
#define _I(x) x
#else // end #ifdef USER_BOARD_H
#define GLOBAL extern
#define _I(x)
#endif // end #ifndef USER_BOARD_H

GLOBAL void board_init(void);

	// OK to drive flag
GLOBAL U8 gn_DriveFlag _I(=0);
GLOBAL U8 gn_DirFlag _I(=0);

// GLOBAL U8 U8variableInitialized _I(= 0);
// GLOBAL S8 S8variableUninitialized();

GLOBAL U32 gn_interruptCause0 __attribute__ ((section (".noinit")));
GLOBAL U32 gn_interruptCause1 __attribute__ ((section (".noinit")));
GLOBAL U32 gn_interruptCause2 __attribute__ ((section (".noinit")));
GLOBAL U32 gn_interruptCause3 __attribute__ ((section (".noinit")));

// clean up
#undef _I
#undef GLOBAL


#endif /* _SP_MODEL_BOARD_R0_H_ */
/* end of SP  model board r0.h */
