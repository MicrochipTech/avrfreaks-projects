/* This source file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3A-1.4.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
 *
 * \brief Management of the USB host mouse HID task.
 *
 * This file manages the USB host mouse HID task.
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


#if USB_HOST_FEATURE == ENABLED

#include "board.h"
#ifdef FREERTOS_USED
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "conf_usb.h"
#include "usb_drv.h"
#include "usb_host_enum.h"
#include "usb_host_task.h"
#include "host_hid.h"
#include "host_mouse_hid_task.h"
#include <stdio.h>


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

#define MOUSE_X_MIN   0
#define MOUSE_X_MAX   80
#define MOUSE_Y_MIN   0
#define MOUSE_Y_MAX   18

//_____ D E C L A R A T I O N S ____________________________________________

U8 pipe_mouse_in;
volatile Bool mouse_hid_new_device_connected;
volatile Bool mouse_hid_connected;
static U16 sof_cnt;
volatile S8 usb_report[4];
S8 mouse_x;
S8 mouse_y;
S8 new_x, new_y;
Bool mouse_b0;
Bool mouse_b1;
Bool mouse_b2;

static void disp_led_mouse(void);
static void disp_ascii_mouse(void);

//!
//! @brief This function initializes the host mouse HID task.
//!
void host_mouse_hid_task_init(void)
{
  sof_cnt = 0;
  mouse_hid_new_device_connected = FALSE;
  mouse_hid_connected = FALSE;

#ifdef FREERTOS_USED
  xTaskCreate(host_mouse_hid_task,
              configTSK_USB_HHID_MOUSE_NAME,
              configTSK_USB_HHID_MOUSE_STACK_SIZE,
              NULL,
              configTSK_USB_HHID_MOUSE_PRIORITY,
              NULL);
#endif  // FREERTOS_USED
}


//!
//! @brief This function manages the host mouse HID task.
//!
#ifdef FREERTOS_USED
void host_mouse_hid_task(void *pvParameters)
#else
void host_mouse_hid_task(void)
#endif
{
  U8 i;

#ifdef FREERTOS_USED
  portTickType xLastWakeTime;

  xLastWakeTime = xTaskGetTickCount();
  while (TRUE)
  {
    vTaskDelayUntil(&xLastWakeTime, configTSK_USB_HHID_MOUSE_PERIOD);

#endif  // FREERTOS_USED
    // First, check the host controller is in full operating mode with the
    // B-device attached and enumerated
    if (Is_host_ready())
    {
      // New device connection (executed only once after device connection)
      if (mouse_hid_new_device_connected)
      {
        mouse_hid_new_device_connected = FALSE;

        // For all supported interfaces
        for (i = 0; i < Get_nb_supported_interface(); i++)
        {
          if(Get_class(i)==HID_CLASS && Get_protocol(i)==MOUSE_PROTOCOL)
          {
            host_hid_set_idle(HID_IDLE_DURATION_INDEFINITE, HID_REPORT_ID_ALL, i);
            host_hid_get_report(HID_REPORT_DESCRIPTOR, 0, i);
            pipe_mouse_in = Get_ep_pipe(i, 0);
            Host_enable_continuous_in_mode(pipe_mouse_in);
            Host_unfreeze_pipe(pipe_mouse_in);
            mouse_hid_connected=TRUE;
            break;
          }
        }
      }

      if( Is_host_mouse_hid_configured() )
      {
        if((Is_host_in_received(pipe_mouse_in)) && (Is_host_stall(pipe_mouse_in)==FALSE) )
        {
           Host_reset_pipe_fifo_access(pipe_mouse_in);
           usb_report[0]=
           usb_report[1]=
           usb_report[2]=
           usb_report[3]=0;
           host_read_p_rxpacket(pipe_mouse_in, (void*)usb_report, 4, NULL);
           Host_ack_in_received(pipe_mouse_in);
           Host_free_in(pipe_mouse_in);
           new_x = usb_report[1];
           new_y = usb_report[2];
           mouse_x += new_x;
           mouse_y += new_y;
           if(      mouse_x<MOUSE_X_MIN ) mouse_x=MOUSE_X_MIN;
           else if( mouse_x>MOUSE_X_MAX ) mouse_x=MOUSE_X_MAX;
           if(      mouse_y<MOUSE_Y_MIN ) mouse_y=MOUSE_Y_MIN;
           else if( mouse_y>MOUSE_Y_MAX ) mouse_y=MOUSE_Y_MAX;
           mouse_b0=usb_report[0] & 1;
           mouse_b1=usb_report[0] & 2;
           mouse_b2=usb_report[0] & 4;
           disp_led_mouse();
           disp_ascii_mouse();
        }
        if(Is_host_nak_received(pipe_mouse_in))
        {
           Host_ack_nak_received(pipe_mouse_in);
           LED_Off(LED_HOST_MOUSE_B0 );
           LED_Off(LED_HOST_MOUSE_B1 );
           LED_Off(LED_HOST_MOUSE_B2 );
           LED_Off(LED_HOST_MOUSE_B3 );
        }
      }
    }


#ifdef FREERTOS_USED
  }
#endif
}


//!
//! @brief host_sof_action
//!
//! This function increments the sof_cnt counter each time
//! the USB Start-of-Frame interrupt subroutine is executed (1 ms).
//! Useful to manage time delays
//!
void host_sof_action(void)
{
  sof_cnt++;
}


//!
//! @brief Display some animation on the LED according to the mouse status.
//!
static void disp_led_mouse(void)
{
   // Use software debounce for the Led (can not use LED_Toggle(x) !)
   LED_Display_Field(LED_HOST_MOUSE_B0 |
                     LED_HOST_MOUSE_B1,
                     new_x %4);

   LED_Display_Field(LED_HOST_MOUSE_B2 |
                     LED_HOST_MOUSE_B3,
                     new_y %4);

   if( mouse_b0 ) LED_Toggle( LED_HOST_MOUSE_B0 );
   //else           LED_Off(LED_HOST_MOUSE_B0 );
   if( mouse_b1 ) LED_Toggle( LED_HOST_MOUSE_B1 );
   //else           LED_Off(LED_HOST_MOUSE_B1 );
   if( mouse_b2 ) LED_Toggle( LED_HOST_MOUSE_B2 );
   //else           LED_Off(LED_HOST_MOUSE_B2 );
}


//!
//! @brief Display the mouse pointer on a terminal.
//!
//! This function displays a mouse cursor through the USART port.
//  The terminal shall takein charge the use of ANSI code.
//!
static void disp_ascii_mouse(void)
{
   static const char* pub[]={"High Performance", "Code Density", "Low consumption", "CALL NOW !!!"};
   static U8 pub_id=0;
   printf("\x1B[2J\x1B[H--- HID MOUSE EXAMPLE ------------------------------------\r\n");
   if( mouse_b0 ) { // Thanks Banner!
#if BOARD == EVK1100 || BOARD == EVK1105
      printf("\x1B[%.2d;%.2dHOOOO OOOO   OOO O   OOOO    OOOO    \r\n", mouse_y, mouse_x);
      printf("\x1B[%.2d;%.2dH O     O   O   OO       O     O O   \r\n", mouse_y+1, mouse_x);
      printf("\x1B[%.2d;%.2dH O     O  O     O       O     O O   \r\n", mouse_y+2, mouse_x);
      printf("\x1B[%.2d;%.2dH O     O  O           OO     O   O  \r\n", mouse_y+3, mouse_x);
      printf("\x1B[%.2d;%.2dH O     O  O             O    OOOOO  \r\n", mouse_y+4, mouse_x);
      printf("\x1B[%.2d;%.2dH OO   OO   O    O       O   O     O \r\n", mouse_y+5, mouse_x);
      printf("\x1B[%.2d;%.2dH   OOO      OOOO   OOOOO   OOO   OOO\r\n", mouse_y+6, mouse_x);
#elif BOARD == EVK1101
      printf("\x1B[%.2d;%.2dHOOOO OOOO   OOO O   OOOO    OOOOOO \r\n" , mouse_y, mouse_x);
      printf("\x1B[%.2d;%.2dH O     O   O   OO       O    O    O\r\n", mouse_y+1, mouse_x);
      printf("\x1B[%.2d;%.2dH O     O  O     O       O    O    O\r\n", mouse_y+2, mouse_x);
      printf("\x1B[%.2d;%.2dH O     O  O           OO     OOOOO \r\n" , mouse_y+3, mouse_x);
      printf("\x1B[%.2d;%.2dH O     O  O             O    O    O\r\n", mouse_y+4, mouse_x);
      printf("\x1B[%.2d;%.2dH OO   OO   O    O       O    O    O\r\n", mouse_y+5, mouse_x);
      printf("\x1B[%.2d;%.2dH   OOO      OOOO   OOOOO    OOOOOO \r\n" , mouse_y+6, mouse_x);
#else
#warning Board not supported
#endif
   }
   else if( mouse_b1 ) {
#if BOARD == EVK1100 || BOARD == EVK1105
      printf("\x1B[%.2d;%.2dH   OOO     OOOO      OOOOO   OOO    OO  OOOOO   OOOOOO    OOOOOO\r\n", mouse_y, mouse_x);
      printf("\x1B[%.2d;%.2dH  O   O        O      O   O   OO   OO     O      O    O  O     O\r\n", mouse_y+1, mouse_x);
      printf("\x1B[%.2d;%.2dH  O   O        O      O    O  O O O O     O      O    O  O      \r\n", mouse_y+2, mouse_x);
      printf("\x1B[%.2d;%.2dH   OOO       OO       O    O  O O O O     O      O    O   OOOOO \r\n", mouse_y+3, mouse_x);
      printf("\x1B[%.2d;%.2dH OO   O        O      O    O  O  O  O     O      OOOOO         O\r\n", mouse_y+4, mouse_x);
      printf("\x1B[%.2d;%.2dH O    O        O      O   O   O     O     O      O       O     O\r\n", mouse_y+5, mouse_x);
      printf("\x1B[%.2d;%.2dH  OOOO    OOOOO      OOOOO   OOO   OOO  OOOOO   OOOO     OOOOOO \r\n", mouse_y+6, mouse_x);
#elif BOARD == EVK1101
      printf("\x1B[%.2d;%.2dH OOOOOOO   OOOOO      OOOOO   OOO    OO  OOOOO   OOOOOO    OOOOOO\r\n", mouse_y, mouse_x);
      printf("\x1B[%.2d;%.2dH O     O   O           O   O   OO   OO     O      O    O  O     O\r\n", mouse_y+1, mouse_x);
      printf("\x1B[%.2d;%.2dH      O    O           O    O  O O O O     O      O    O  O      \r\n", mouse_y+2, mouse_x);
      printf("\x1B[%.2d;%.2dH      O    OOOOO       O    O  O O O O     O      O    O   OOOOO \r\n", mouse_y+3, mouse_x);
      printf("\x1B[%.2d;%.2dH     O          O      O    O  O  O  O     O      OOOOO         O\r\n", mouse_y+4, mouse_x);
      printf("\x1B[%.2d;%.2dH    O           O      O   O   O     O     O      O       O     O\r\n", mouse_y+5, mouse_x);
      printf("\x1B[%.2d;%.2dH    O      OOOOO      OOOOO   OOO   OOO  OOOOO   OOOO     OOOOOO \r\n", mouse_y+6, mouse_x);
#else
#warning Board not supported
#endif
   }
   else if( mouse_b2 ) {
      printf("\x1B[%.2d;%.2dH%s\r\n", mouse_y, mouse_x, pub[pub_id++]);
      if( pub_id==4 ) pub_id=0;
   }
   else
   {
      printf("\x1B[%.2d;%.2dH#    \r\n", mouse_y, mouse_x);
      printf("\x1B[%.2d;%.2dH###  \r\n", mouse_y+1, mouse_x);
      printf("\x1B[%.2d;%.2dH#####\r\n", mouse_y+2, mouse_x);
      printf("\x1B[%.2d;%.2dH# ## \r\n", mouse_y+3, mouse_x);
      printf("\x1B[%.2d;%.2dH   ##\r\n", mouse_y+4, mouse_x);
   }
}


#endif  // USB_HOST_FEATURE == ENABLED
