/*
 * stk600.h
 *
 *  Created on: Jul 7, 2009
 *      Author: nl9x1nt
 */

#ifndef STK600_H_
#define STK600_H_


#include <avr32/io.h>
#include "gpio.h"

#define LED_START 	      8

#define LED_MONO0_GREEN   (LED_START + 0)
#define LED_MONO1_GREEN   (LED_START + 1)
#define LED_MONO2_GREEN   (LED_START + 2)
#define LED_MONO3_GREEN   (LED_START + 3)
#define LED_BI0_GREEN     (LED_START + 4)
#define LED_BI0_RED       (LED_START + 5)
#define LED_BI1_GREEN     (LED_START + 6)
#define LED_BI1_RED       (LED_START + 7)


/*! \name Oscillator Definitions
 */
//! @{

// RCOsc has no custom calibration by default. Set the following definition to
// the appropriate value if a custom RCOsc calibration has been applied to your
// part.
//#define FRCOSC          AVR32_PM_RCOSC_FREQUENCY              //!< RCOsc frequency: Hz.

#define FOSC32          32768                                 //!< Osc32 frequency: Hz.
#define OSC32_STARTUP   AVR32_PM_OSCCTRL32_STARTUP_8192_RCOSC //!< Osc32 startup time: RCOsc periods.

#define FOSC0           12000000                              //!< Osc0 frequency: Hz.
#define OSC0_STARTUP    AVR32_PM_OSCCTRL0_STARTUP_2048_RCOSC  //!< Osc0 startup time: RCOsc periods.

// Osc1 crystal is not mounted by default. Set the following definitions to the
// appropriate values if a custom Osc1 crystal is mounted on your board.
//#define FOSC1           12000000                              //!< Osc1 frequency: Hz.
//#define OSC1_STARTUP    AVR32_PM_OSCCTRL1_STARTUP_2048_RCOSC  //!< Osc1 startup time: RCOsc periods.

//! @}


/*! \name USB Definitions
 */
//! @{

//! Multiplexed pin used for USB_ID: AVR32_USBB_USB_ID_x_x.
//! To be selected according to the AVR32_USBB_USB_ID_x_x_PIN and
//! AVR32_USBB_USB_ID_x_x_FUNCTION definitions from <avr32/uc3axxxx.h>.
#define USB_ID                      AVR32_USBB_USB_ID_0_0

//! Multiplexed pin used for USB_VBOF: AVR32_USBB_USB_VBOF_x_x.
//! To be selected according to the AVR32_USBB_USB_VBOF_x_x_PIN and
//! AVR32_USBB_USB_VBOF_x_x_FUNCTION definitions from <avr32/uc3axxxx.h>.
#define USB_VBOF                    AVR32_USBB_USB_VBOF_0_1

//! Active level of the USB_VBOF output pin.
#define USB_VBOF_ACTIVE_LEVEL       LOW

//! USB overcurrent detection pin.
#define USB_OVERCURRENT_DETECT_PIN  AVR32_PIN_PX33

#define LED_COUNT 8
//! Saved state of all LEDs.
static volatile U32 LED_State = (1 << LED_COUNT) - 1;


void LED_Toggle(U32 leds);



//! @}

#endif /* STK600_H_ */
