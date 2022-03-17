/* This source file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3A-1.4.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Main file of the USB HID example.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a USB module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/*! \mainpage AVR32 USB software framework for HID
 *
 * \section intro License
 * Use of this program is subject to Atmel's End User License Agreement.
 *
 * Please read the \ref license at the bottom of this page.
 *
 * \section install Description
 * This embedded application source code illustrates how to implement a USB HID-mouse application
 * on the AVR32 microcontroller.
 *
 * As the AVR32 implements a device/host USB controller, the embedded application can operate
 * in one of the following USB operating modes:
 *   - USB device;
 *   - USB reduced-host controller;
 *   - USB dual-role device (depending on the ID pin).
 *
 * To optimize embedded code/RAM size and reduce the number of source modules, the application can be
 * configured to use one and only one of these operating modes.
 *
 * \section sample About the Sample Application
 * By default the sample code is delivered with a simple preconfigured dual-role USB application.
 * It means that the code generated allows to operate as a device or a host depending on the USB ID pin:
 *
 *   - Attached to a mini-B plug (ID pin unconnected) the application will be used in the device operating mode.
 * Thus, the application can be connected to a system host (e.g. a PC) to operate as a USB mouse device:
 *
 * \image html appli_evk1100_device.jpg "EVK1100 USB Device Mode"
 * \image html appli_evk1101_device.jpg "EVK1101 USB Device Mode"
 *
 * The mouse can be controlled thanks to the joystick and the buttons (shown in a red box in the above pictures).
 *
 *   - Attached to a mini-A plug (ID pin tied to ground) the application will operate in reduced-host mode.
 * This mode allows to connect a USB mouse device:
 *
 * \image html appli_evk1100_host.jpg "EVK1100 USB Host Mode"
 * \image html appli_evk1101_host.jpg "EVK1101 USB Host Mode"
 *
 * In this mode, a mouse ascii pointer is output on the USART1 connector. Open a serial terminal (e.g. HyperTerminal under
 * Windows systems or minicom under Linux systems; USART settings: 57600 bauds, 8-bit data, no parity bit,
 * 1 stop bit, no flow control). Make sure that the  serial terminal takes in charge the ANSI codes.
 *
 * \image html terminal.JPG "Debug output in terminal"
 *
 * \section device_use Using the USB Device Mode
 * Connect the application to a USB host (e.g. a PC) with a mini-B (embedded side) to A (PC host side) cable.
 * The application will behave as a mouse peripheral. The mouse can be controlled thanks to the joystick and
 * the buttons of the EVK110x.
 *
 * \section host_use Using the USB Host Mode
 * Connect the application to a USB mouse device. The application will behave as a USB mouse
 * reduced host. Leds will blink according to the mouse eactivity and a mouse ascii pointer is output on the USART1 connector.
 *
 * @section arch Architecture
 * As illustrated in the figure bellow, the application entry point is located is the hid_example.c file.
 * The application can be ran using the FreeRTOS operating system, or in standalone mode.
 * In the standalone mode, the main function runs the usb_task and device_mouse_hid_task tasks in an infinite loop.
 *
 * The application is based on three different tasks:
 * - The usb_task  (usb_task.c associated source file), is the task performing the USB low level
 * enumeration process in device mode.
 * Once this task has detected that the usb connection is fully operationnal, it updates different status flags
 * that can be check within the high level application tasks.
 * - The device_mouse_hid_task task performs the high level device application operation.
 * Once the device is fully enumerated (DEVICE SET_CONFIGURATION request received), the task
 * transmit data (reports) on its IN endpoint.
 * - The host_mouse_hid_task task performs the high level host application operation.
 * Once a device is connected and fully enumerated, the task
 * receive data (reports) on its IN endpoint.
 *
 * \image html arch_full.jpg "Architecture Overview"
 *
 * \section files Main Files
 * - device_mouse_hid_task.c : high level device mouse application operation
 * - device_mouse_hid_task.h : high level device mouse application operation header
 * - host_mouse_hid_task.c : high level host mouse application operation
 * - host_mouse_hid_task.h : high level host mouse application operation header
 * - hid_example.c : HID code example
 *
 * \section license Copyright notice
 * Copyright (C) 2006-2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/


//_____  I N C L U D E S ___________________________________________________

#ifndef FREERTOS_USED
#if __GNUC__
#include "nlao_cpu.h"
#include "nlao_usart.h"
#endif
#else
#include <stdio.h>
#endif
#include "compiler.h"
#include "board.h"
#include "print_funcs.h"
#include "intc.h"
#include "pm.h"
#ifdef FREERTOS_USED
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "conf_usb.h"
#include "usb_task.h"
#if USB_DEVICE_FEATURE == ENABLED
#include "device_mouse_hid_task.h"
#endif
#if USB_HOST_FEATURE == ENABLED
//#include "host_keyboard_hid_task.h"
#include "host_mouse_hid_task.h"
#endif


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

#ifndef FREERTOS_USED

  #if __GNUC__

/*! \brief Low-level initialization routine called during startup, before the
 *         main function.
 *
 * This version comes in replacement to the default one provided by the Newlib
 * add-ons library.
 * Newlib add-ons' _init_startup only calls init_exceptions, but Newlib add-ons'
 * exception and interrupt vectors are defined in the same section and Newlib
 * add-ons' interrupt vectors are not compatible with the interrupt management
 * of the INTC module.
 * More low-level initializations are besides added here.
 */
int _init_startup(void)
{
  // Import the Exception Vector Base Address.
  extern void _evba;

  // Load the Exception Vector Base Address in the corresponding system register.
  Set_system_register(AVR32_EVBA, (int)&_evba);

  // Enable exceptions.
  Enable_global_exception();

  // Initialize interrupt handling.
  INTC_init_interrupts();

  // init debug serial line
  init_dbg_rs232(FOSC0);

  // Give the used CPU clock frequency to Newlib, so it can work properly.
  set_cpu_hz(FOSC0);

  // Initialize the USART used for the debug trace with the configured parameters.
  set_usart_base( ( void * ) DBG_USART );

  // Don't-care value for GCC.
  return 1;
}

  #elif __ICCAVR32__

/*! \brief Low-level initialization routine called during startup, before the
 *         main function.
 */
int __low_level_init(void)
{
  // Enable exceptions.
  Enable_global_exception();

  // Initialize interrupt handling.
  INTC_init_interrupts();

  // init debug serial line
  init_dbg_rs232(FOSC0);

  // Initialize the USART used for the debug trace with the configured parameters.
  extern volatile avr32_usart_t *volatile stdio_usart_base;
  stdio_usart_base = DBG_USART;

  // Request initialization of data segments.
  return 1;
}

  #endif  // Compiler

#endif  // FREERTOS_USED


/*! \brief Main function. Execution starts here.
 *
 * \retval 42 Fatal error.
 */
int main(void)
{
  // switch to oscillator 0
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Initialize usart comm
  init_dbg_rs232(FOSC0);

  // Initialize USB clock
  pm_configure_usb_clock();

  // Initialize USB task
  usb_task_init();

#if USB_DEVICE_FEATURE == ENABLED
  // Initialize device mouse USB task
  device_mouse_hid_task_init();
#endif
#if USB_HOST_FEATURE == ENABLED
  //host_keyboard_hid_task_init();

  // Initialize host mouse USB task
  host_mouse_hid_task_init();
#endif

#ifdef FREERTOS_USED
  // Start OS scheduler
  vTaskStartScheduler();
  portDBG_TRACE("FreeRTOS returned.");
  return 42;
#else
  // No OS here. Need to call each task in round-robin mode.
  while (TRUE)
  {
    usb_task();
  #if USB_DEVICE_FEATURE == ENABLED
    device_mouse_hid_task();
  #endif
  #if USB_HOST_FEATURE == ENABLED
    //host_keyboard_hid_task();
    host_mouse_hid_task();
  #endif
  }
#endif  // FREERTOS_USED
}
