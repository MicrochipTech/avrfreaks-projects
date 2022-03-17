/* This header file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3A-1.4.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
 *
 * \brief Management of the USB task either device/host or both.
 *
 * The USB task selects the correct USB task (USB device task or USB host
 * task) to be executed depending on the current mode available.
 *
 * According to the values of USB_DEVICE_FEATURE and USB_HOST_FEATURE
 * (located in the conf_usb.h file), the USB task can be configured to
 * support USB device mode or USB host mode or both for a dual-role device
 * application.
 *
 * This module also contains the general USB interrupt subroutine. This
 * subroutine is used to detect asynchronous USB events.
 *
 * Note:
 *   - The USB task belongs to main; the USB device and host tasks do not.
 *     They are called from the general USB task.
 *   - See the conf_usb.h file for more details about the configuration of
 *     this module.
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


#ifndef _USB_TASK_H_
#define _USB_TASK_H_


//! @defgroup usb_task USB task entry point module
//! @{

//_____ I N C L U D E S ____________________________________________________

#include "compiler.h"
#include "preprocessor.h"
#include "conf_usb.h"


//_____ M A C R O S ________________________________________________________

      //! @defgroup usb_software_evts USB software Events Management
      //! Macros to manage USB events detected under interrupt
      //! @{
#define Usb_send_event(x)               (Set_bits(g_usb_event, 1 << (x)))
#define Usb_ack_event(x)                (Clr_bits(g_usb_event, 1 << (x)))
#define Is_usb_event(x)                 (Tst_bits(g_usb_event, 1 << (x)))
#define Usb_clear_all_event()           (g_usb_event = 0x0000)
#define Is_host_emergency_exit()        (Is_usb_event(EVT_HOST_DISCONNECTION) || Is_usb_event(EVT_USB_DEVICE_FUNCTION))
#define Is_usb_device()                 (g_usb_mode == USB_MODE_DEVICE)
#define Is_usb_host()                   (g_usb_mode == USB_MODE_HOST)

#define EVT_USB_POWERED               1         // USB plugged
#define EVT_USB_UNPOWERED             2         // USB unplugged
#define EVT_USB_DEVICE_FUNCTION       3         // USB in device
#define EVT_USB_HOST_FUNCTION         4         // USB in host
#define EVT_USB_SUSPEND               5         // USB suspend
#define EVT_USB_WAKE_UP               6         // USB wake-up
#define EVT_USB_RESUME                7         // USB resume
#define EVT_USB_RESET                 8         // USB reset
#define EVT_HOST_SOF                  9         // Host start-of-frame sent
#define EVT_HOST_HWUP                 10        // Host wake-up detected
#define EVT_HOST_DISCONNECTION        11        // The target device is disconnected
      //! @}

      //! @defgroup std_req_values Standard requests defines
      //! @{
#define GET_STATUS                            0x00
#define GET_DEVICE                            0x01
#define CLEAR_FEATURE                         0x01
#define GET_STRING                            0x03
#define SET_FEATURE                           0x03
#define SET_ADDRESS                           0x05
#define GET_DESCRIPTOR                        0x06
#define SET_DESCRIPTOR                        0x07
#define GET_CONFIGURATION                     0x08
#define SET_CONFIGURATION                     0x09
#define GET_INTERFACE                         0x0A
#define SET_INTERFACE                         0x0B
#define SYNCH_FRAME                           0x0C

#define GET_DEVICE_DESCRIPTOR                    1
#define GET_CONFIGURATION_DESCRIPTOR             4

#define REQUEST_DEVICE_STATUS                 0x80
#define REQUEST_INTERFACE_STATUS              0x81
#define REQUEST_ENDPOINT_STATUS               0x82
#define ZERO_TYPE                             0x00
#define INTERFACE_TYPE                        0x01
#define ENDPOINT_TYPE                         0x02

                  // Descriptor Types
#define DEVICE_DESCRIPTOR                     0x01
#define CONFIGURATION_DESCRIPTOR              0x02
#define STRING_DESCRIPTOR                     0x03
#define INTERFACE_DESCRIPTOR                  0x04
#define ENDPOINT_DESCRIPTOR                   0x05
#define DEVICE_QUALIFIER_DESCRIPTOR           0x06
#define OTHER_SPEED_CONFIGURATION_DESCRIPTOR  0x07

                  // Standard Features
#define FEATURE_DEVICE_REMOTE_WAKEUP          0x01
#define FEATURE_ENDPOINT_HALT                 0x00

#define TEST_J                                0x01
#define TEST_K                                0x02
#define TEST_SEO_NAK                          0x03
#define TEST_PACKET                           0x04
#define TEST_FORCE_ENABLE                     0x05

                  // Device Status
#define BUS_POWERED                              0
#define SELF_POWERED                             1

                  // Bit-masks for the endpoint number and direction bit-fields of endpoint addresses
#define MSK_EP_NBR                            0x0F
#define MSK_EP_DIR                            0x80

        //! @brief Extract endpoint number from endpoint address in endpoint descriptor
        //! @param ep_addr U8: Endpoint address
        //! @return U8: Endpoint number
#define Get_desc_ep_nbr(ep_addr)              (Rd_bitfield(ep_addr, MSK_EP_NBR))

        //! @brief Extract endpoint direction from endpoint address in endpoint descriptor
        //! @param ep_addr U8: Endpoint address
        //! @return U8: Endpoint direction
#define Get_desc_ep_dir(ep_addr)              (Rd_bitfield(ep_addr, MSK_EP_DIR))

        //! Convert 16-, 32- or 64-bit data between MCU and USB endianisms.
        //! Depending on MCU endianism, swap or not data bytes.
        //! @param width    Data width in bits: 16, 32 or 64
        //! @param data     16-, 32- or 64-bit data to format
        //! @return         Formatted 16-, 32- or 64-bit data
        //! @{
#if LITTLE_ENDIAN_MCU
  #define Usb_format_mcu_to_usb_data(width, data) ((TPASTE2(U, width))(data))
  #define Usb_format_usb_to_mcu_data(width, data) ((TPASTE2(U, width))(data))
  #define usb_format_mcu_to_usb_data(width, data) ((TPASTE2(U, width))(data))
  #define usb_format_usb_to_mcu_data(width, data) ((TPASTE2(U, width))(data))
#else // BIG_ENDIAN_MCU
  #define Usb_format_mcu_to_usb_data(width, data) (TPASTE2(Swap, width)(data))
  #define Usb_format_usb_to_mcu_data(width, data) (TPASTE2(Swap, width)(data))
  #define usb_format_mcu_to_usb_data(width, data) (TPASTE2(swap, width)(data))
  #define usb_format_usb_to_mcu_data(width, data) (TPASTE2(swap, width)(data))
#endif
        //! @}
      //! @}

#define USB_MODE_UNDEFINED                    0x00
#define USB_MODE_DEVICE                       0x01
#define USB_MODE_HOST                         0x02


//_____ D E C L A R A T I O N S ____________________________________________

extern volatile U16 g_usb_event;

#if USB_HOST_FEATURE == ENABLED
extern volatile U32 private_sof_counter;
#endif

#if USB_DEVICE_FEATURE == ENABLED && USB_HOST_FEATURE == ENABLED
extern volatile U8 g_usb_mode;
#elif USB_DEVICE_FEATURE == ENABLED
  #define g_usb_mode  USB_MODE_DEVICE
#elif USB_HOST_FEATURE == ENABLED
  #define g_usb_mode  USB_MODE_HOST
#else
  #error At least one of USB_DEVICE_FEATURE and USB_HOST_FEATURE must be enabled
#endif

//! @brief This function initializes the USB process.
//!
//! This function enables the USB controller and init the USB interrupts.
//! The aim is to allow the USB connection detection in order to send
//! the appropriate USB event to the operating mode manager.
//! Depending on the mode supported (HOST/DEVICE/DUAL_ROLE) the function
//! calls the corespong USB mode initialization function
extern void usb_task_init(void);

//! @brief Entry point of the USB mamnagement
//!
//! Depending on the mode supported (HOST/DEVICE/DUAL_ROLE) the function
//! calls the corespong USB management function
#ifdef FREERTOS_USED
extern void usb_task(void *pvParameters);
#else
extern void usb_task(void);
#endif

//! @}


#endif  // _USB_TASK_H_
