/* This source file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3A-1.4.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
 *
 * \brief Management of the USB device mouse HID task.
 *
 * This file manages the USB device mouse HID task.
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

#include "conf_usb.h"
#include "joystick.h"
#if BOARD == EVK1101
# include "lis3l06al.h"
#endif


#if USB_DEVICE_FEATURE == ENABLED

#include "board.h"
#ifdef FREERTOS_USED
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "usb_drv.h"
#include "gpio.h"
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "device_mouse_hid_task.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

// Limits after which mouse speed is increased.
#define DISP_LIMIT_LOW         150 // Seems to be in pixel units
#define DISP_LIMIT_HIGH        300 // Seems to be in pixel units

#define BUTTON_0_EVENT_PUSH      1 // Windows "Left Click"
#define BUTTON_1_EVENT_PUSH      4 // Windows "Right Click"
#define BUTTON_2_EVENT_PUSH      2 // Option on some demo boards

//_____ D E C L A R A T I O N S ____________________________________________

static U16  sof_cnt;

U8  u8_report_buttons   =0;
S8  u8_report_disp_x    =0;
S8  u8_report_disp_y    =0;
S8  u8_report_disp_wheel=0;



//!
//! @brief This function initializes the hardware/software resources
//! required for device mouse HID task.
//!
void device_mouse_hid_task_init(void)
{
  sof_cnt = 0;

#if BOARD == EVK1101
  // Initialize accelerometer driver
  acc_init();
#endif

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
  xTaskCreate(device_mouse_hid_task,
              configTSK_USB_DHID_MOUSE_NAME,
              configTSK_USB_DHID_MOUSE_STACK_SIZE,
              NULL,
              configTSK_USB_DHID_MOUSE_PRIORITY,
              NULL);
#endif  // FREERTOS_USED
}


//!
//! @brief This function initializes the hardware/software resources
//! required for device mouse HID task.
//!
Bool is_usb_mouse_event( void )
{
   static U8   disp=1;
   static U16  count=0;
   static Bool old_click_0=FALSE;  // FALSE means released, TRUE means pushed
   static Bool old_click_1=FALSE;  // FALSE means released, TRUE means pushed
#if BOARD == EVK1100
   static Bool old_click_2=FALSE;  // FALSE means released, TRUE means pushed
#endif
   Bool b_activity;

   u8_report_buttons=0;
   u8_report_disp_x=0;
   u8_report_disp_y=0;
   u8_report_disp_wheel=0;
   b_activity=FALSE;

#if BOARD == EVK1101
   signed int res;

   // Get accelerometer acquisition and process datas
   acc_update();

   // Look joystick activity for the Wheel events
   // input is pulled up, if 1 : input is not active
   if( is_joystick_up() )
     u8_report_disp_wheel=disp, b_activity=TRUE;

   if( is_joystick_down() )
     u8_report_disp_wheel=-disp, b_activity=TRUE;

   // Look accelerometer activity for the X and Y events
   if( 0!=(res=is_acc_abs_angle_x(40)) )
   {
     if(      res>0 )
       u8_report_disp_x=-10*disp, b_activity=TRUE;
     else if( res<0 )
       u8_report_disp_x=10*disp, b_activity=TRUE;
   }
   else if( 0!=(res=is_acc_abs_angle_x(30)) )
   {
     if(      res>0 )
       u8_report_disp_x=-6*disp, b_activity=TRUE;
     else if( res<0 )
       u8_report_disp_x=6*disp, b_activity=TRUE;
   }
   else if( 0!=(res=is_acc_abs_angle_x(20)) )
   {
     if(      res>0 )
       u8_report_disp_x=-4*disp, b_activity=TRUE;
     else if( res<0 )
       u8_report_disp_x=4*disp, b_activity=TRUE;
   }

   else if( 0!=(res=is_acc_abs_angle_x(15)) )
   {
     if(      res>0 )
       u8_report_disp_x=-2*disp, b_activity=TRUE;
     else if( res<0 )
       u8_report_disp_x=2*disp, b_activity=TRUE;
   }
   else if( 0!=(res=is_acc_abs_angle_x(10)) )
   {
     if(      res>0 )
       u8_report_disp_x=-disp, b_activity=TRUE;
     else if( res<0 )
       u8_report_disp_x=disp, b_activity=TRUE;
   }



   if( 0!=(res=is_acc_abs_angle_y(40)) )
   {
     if(      res>0 )
       u8_report_disp_y=-10*disp, b_activity=TRUE;
     else if( res<0 )
       u8_report_disp_y=10*disp, b_activity=TRUE;
   }
   else if( 0!=(res=is_acc_abs_angle_y(30)) )
   {
     if(      res>0 )
       u8_report_disp_y=-6*disp, b_activity=TRUE;
     else if( res<0 )
       u8_report_disp_y=6*disp, b_activity=TRUE;
   }
   else if( 0!=(res=is_acc_abs_angle_y(20)) )
   {
     if(      res>0 )
       u8_report_disp_y=-4*disp, b_activity=TRUE;
     else if( res<0 )
       u8_report_disp_y=4*disp, b_activity=TRUE;
   }
   else if( 0!=(res=is_acc_abs_angle_y(15)) )
   {
     if(      res>0 )
       u8_report_disp_y=-2*disp, b_activity=TRUE;
     else if( res<0 )
       u8_report_disp_y=2*disp, b_activity=TRUE;
   }
   else if( 0!=(res=is_acc_abs_angle_y(10)) )
   {
     if(      res>0 )
       u8_report_disp_y=-disp, b_activity=TRUE;
     else if( res<0 )
       u8_report_disp_y=disp, b_activity=TRUE;
   }
#else
   // Look Joystick activity for the X and Y events
   // input is pulled up, if 1 : input is not active
   if (is_joystick_right())
      u8_report_disp_x=disp, b_activity=TRUE;

   if (is_joystick_left())
      u8_report_disp_x=-disp, b_activity=TRUE;

   if (is_joystick_down())
      u8_report_disp_y=disp, b_activity=TRUE;

   if (is_joystick_up())
      u8_report_disp_y=-disp, b_activity=TRUE;
#endif

   if( b_activity )
   {
      count++;
      if( count >= DISP_LIMIT_HIGH )
         disp=3;
      else if( count >= DISP_LIMIT_LOW )
         disp=2;
   }
   else
      count=0, disp=1;



   // Look for button activity
   // input is pulled up, if 1 : input is not active
   if( (!gpio_get_pin_value(GPIO_PUSH_BUTTON_0))
   ||  (is_joystick_pressed()                  ) )
   {
      Set_bits(u8_report_buttons, BUTTON_0_EVENT_PUSH);
      if( old_click_0==FALSE )
         old_click_0=TRUE, b_activity=TRUE;
   }
   else
      if( old_click_0==TRUE )
         old_click_0=FALSE, b_activity=TRUE;



   if( !gpio_get_pin_value(GPIO_PUSH_BUTTON_1) )
   {
      Set_bits(u8_report_buttons, BUTTON_1_EVENT_PUSH);
      if( old_click_1==FALSE )
         old_click_1=TRUE, b_activity=TRUE;
   }
   else
      if( old_click_1==TRUE )
         old_click_1=FALSE, b_activity=TRUE;

#if BOARD == EVK1100
   if( !gpio_get_pin_value(GPIO_PUSH_BUTTON_2) )
   {
      Set_bits(u8_report_buttons, BUTTON_2_EVENT_PUSH);
      if( old_click_2==FALSE )
         old_click_2=TRUE, b_activity=TRUE;
   }
   else
      if( old_click_2==TRUE )
         old_click_2=FALSE, b_activity=TRUE;
#endif

   if( b_activity )  return TRUE;
   else              return FALSE;
}


//!
//! @brief Entry point of the device mouse HID task management
//!
#ifdef FREERTOS_USED
void device_mouse_hid_task(void *pvParameters)
#else
void device_mouse_hid_task(void)
#endif
{
#ifdef FREERTOS_USED
  portTickType xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();
  while (TRUE)
  {
    vTaskDelayUntil(&xLastWakeTime, configTSK_USB_DHID_MOUSE_PERIOD);

    // First, check the device enumeration state
    if (!Is_device_enumerated()) continue;
#else
    // First, check the device enumeration state
    if (!Is_device_enumerated()) return;
#endif  // FREERTOS_USED

    if( is_usb_mouse_event() )
    {
       if( Is_usb_in_ready(EP_HID_MOUSE_IN) )
       {
          Usb_reset_endpoint_fifo_access(EP_HID_MOUSE_IN);
          
          //! Write report
          Usb_write_endpoint_data(EP_HID_MOUSE_IN, 8, u8_report_buttons);
          Usb_write_endpoint_data(EP_HID_MOUSE_IN, 8, u8_report_disp_x);
          Usb_write_endpoint_data(EP_HID_MOUSE_IN, 8, u8_report_disp_y);
          Usb_write_endpoint_data(EP_HID_MOUSE_IN, 8, u8_report_disp_wheel);
          
          Usb_ack_in_ready_send(EP_HID_MOUSE_IN);
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
