/* This source file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3A-1.4.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
 *
 * \brief Management of the USB device CDC task.
 *
 * This file manages the USB device CDC task.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a USB module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ***************************************************************************/

/* Copyright (C) 2006-2008, Atmel Corporation All rights reserved.
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
 */


//_____  I N C L U D E S ___________________________________________________

#include <stdio.h>
#include "usart.h"     // Shall be included before FreeRTOS header files, since 'inline' is defined to ''; leading to
                       // link errors
#include "conf_usb.h"


#if USB_DEVICE_FEATURE == ENABLED

#include "board.h"
#ifdef FREERTOS_USED
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "usb_drv.h"
#include "gpio.h"
#if BOARD == EVK1100 || BOARD == EVK1101
# include "joystick.h"
#endif
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "device_cdc_task.h"
#include "uart_usb_lib.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

//_____ D E C L A R A T I O N S ____________________________________________

static volatile U16  sof_cnt;



//!
//! @brief This function initializes the hardware/software resources
//! required for device CDC task.
//!
void device_cdc_task_init(void)
{
  sof_cnt   =0 ;
  uart_usb_init();

#ifndef FREERTOS_USED
  #if USB_HOST_FEATURE == ENABLED
  // If both device and host features are enabled, check if device mode is engaged
  // (accessing the USB registers of a non-engaged mode, even with load operations,
  // may corrupt USB FIFO data).
  if (Is_usb_device())
  #endif  // USB_HOST_FEATURE == ENABLED
    Usb_enable_sof_interrupt();
#endif  // FREERTOS_USED

#ifdef FREERTOS_USED
  xTaskCreate(device_cdc_task,
              configTSK_USB_DCDC_NAME,
              configTSK_USB_DCDC_STACK_SIZE,
              NULL,
              configTSK_USB_DCDC_PRIORITY,
              NULL);

#endif  // FREERTOS_USED
}


//!
//! @brief Entry point of the device CDC task management
//!
#ifdef FREERTOS_USED
void device_cdc_task(void *pvParameters)
#else
void device_cdc_task(void)
#endif
{
  int c;
  static Bool startup=TRUE;

#ifdef FREERTOS_USED
  portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  while (TRUE)
  {
    vTaskDelayUntil(&xLastWakeTime, configTSK_USB_DCDC_PERIOD);

    // First, check the device enumeration state
    if (!Is_device_enumerated()) continue;
#else
    // First, check the device enumeration state
    if (!Is_device_enumerated()) return;
#endif  // FREERTOS_USED

    if( startup )
    {
       printf("\r\nUSB DEVICE Communications Device Class demo.\r\n");
       startup=FALSE;
    }

    if( sof_cnt>=NB_MS_BEFORE_FLUSH )  //Flush buffer in Timeout
    {
       sof_cnt=0;
       uart_usb_flush();
    }

#if BOARD == EVK1100 || BOARD == EVK1101
    if ( is_joystick_right() )
       printf("Joystick Right key pressed.\r\n");

    if ( is_joystick_left() )
       printf("Joystick Left key pressed.\r\n");

    if ( is_joystick_down() )
       printf("Joystick Down key pressed.\r\n");

    if ( is_joystick_up() )
       printf("Joystick Up key pressed.\r\n");

    if ( is_joystick_pressed() )
       printf("Joystick Select key pressed.\r\n");

    if (!gpio_get_pin_value(GPIO_PUSH_BUTTON_0))
       printf("Button 0 key pressed.\r\n");

    if (!gpio_get_pin_value(GPIO_PUSH_BUTTON_1))
       printf("Button 1 key pressed.\r\n");
#endif

    if (usart_test_hit(DBG_USART))    // Something on USART ?
    {
       if( uart_usb_tx_ready() )      // "USART"-USB free ?
       {
          if( USART_SUCCESS==usart_read_char(DBG_USART, &c) )
          {
             if( c=='\r' )
                uart_usb_putchar('\n');

             uart_usb_putchar(c);           // Loop back, USART to USB
#ifdef LED0
             LED_Toggle(LED0);
#endif

          }
          else {
             usart_reset_status( DBG_USART );
          }
       }
    }

    if (uart_usb_test_hit())          // Something received from the USB ?
    {
       if (usart_tx_ready(DBG_USART)) // USART free ?
       {
          c= uart_usb_getchar();
          if( c=='\r' )
            usart_putchar(DBG_USART, '\n');

          usart_putchar(DBG_USART, c);   // loop back USB to USART
#ifdef LED1
          LED_Toggle(LED1);
#endif
       }
    }
#ifdef FREERTOS_USED
  }
#endif
}


//!
//! @brief usb_sof_action
//!
//! This function increments the sof_cnt counter each time
//! the USB Start-of-Frame interrupt subroutine is executed (1 ms).
//! Useful to manage time delays
//!
void usb_sof_action(void)
{
  sof_cnt++;
}

#endif  // USB_DEVICE_FEATURE == ENABLED
