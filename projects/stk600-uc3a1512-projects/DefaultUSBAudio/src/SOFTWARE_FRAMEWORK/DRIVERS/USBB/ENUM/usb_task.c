/* This source file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3A-1.4.0 Release */

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


//_____  I N C L U D E S ___________________________________________________

#include "compiler.h"
#include "intc.h"
#ifdef FREERTOS_USED
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#endif
#include "conf_usb.h"
#include "usb_drv.h"
#include "usb_task.h"

#if USB_DEVICE_FEATURE == ENABLED
#include "usb_descriptors.h"
#include "usb_device_task.h"
#endif

#if USB_HOST_FEATURE == ENABLED
#include "usb_host_task.h"
#endif


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

//!
//! Public: U16 g_usb_event
//! usb_connected is used to store USB events detected upon
//! USB general interrupt subroutine
//! Its value is managed by the following macros (See \ref usb_task.h file)
//! Usb_send_event(x)
//! Usb_ack_event(x)
//! Is_usb_event(x)
//! Usb_clear_all_event()
volatile U16 g_usb_event = 0;


#if USB_DEVICE_FEATURE == ENABLED

//!
//! Public: Bool usb_connected
//! usb_connected is set to TRUE when VBus has been detected
//! usb_connected is set to FALSE otherwise
//! Used with USB_DEVICE_FEATURE == ENABLED only
extern volatile Bool usb_connected;

  #ifdef FREERTOS_USED
//! Handle to the USB Device task
extern xTaskHandle usb_device_tsk;
  #endif

#endif


#if USB_HOST_FEATURE == ENABLED

static const char log_device_disconnected[] = "Device disconnected\n";

//!
//! Private: U8 private_sof_counter
//! Incremented  by host SOF interrupt subroutime
//! This counter is used to detect time-out in host requests.
//! It must not be modified by the user applicative tasks.
volatile U32 private_sof_counter;

  #if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE
extern volatile Bool g_sav_int_sof_enable;
extern volatile S_pipe_int it_pipe_str[MAX_PEP_NB];
  #endif

  #ifdef FREERTOS_USED
//! Handle to the USB Host task
extern xTaskHandle usb_host_tsk;
  #endif

#endif


#if USB_DEVICE_FEATURE == ENABLED && USB_HOST_FEATURE == ENABLED

static const char log_pin_id_changed[]      = "Pin Id changed\n";

//!
//! Public: U8 g_usb_mode
//! Used in dual-role application (both device/host) to store
//! the current mode the USB controller is operating
volatile U8 g_usb_mode = USB_MODE_UNDEFINED;
static volatile U8 g_old_usb_mode;

#endif


#ifdef FREERTOS_USED
//! Handle to the USB task semaphore
static xSemaphoreHandle usb_tsk_semphr = NULL;
#endif


//_____ D E C L A R A T I O N S ____________________________________________

#ifdef FREERTOS_USED

#if __GNUC__
__attribute__((__noinline__))
#endif
static portBASE_TYPE usb_general_interrupt_non_naked(void);


//! @brief USB interrupt routine
//!
//! When FreeRTOS is used, the USB interrupt routine may trigger task switches,
//! so it must use special OS prologue and epilogue. This function must be naked
//! in order to have no stack frame. usb_general_interrupt_non_naked is
//! therefore used for the required stack frame of the interrupt routine.
#if __GNUC__
__attribute__((__naked__))
#elif __ICCAVR32__
#pragma shadow_registers = full
#endif
static void usb_general_interrupt(void)
{
  portENTER_SWITCHING_ISR();
  usb_general_interrupt_non_naked();
  portEXIT_SWITCHING_ISR();
}

#else

#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
static void usb_general_interrupt(void);

#endif  // FREERTOS_USED


//! @brief This function initializes the USB process.
//!
//! Depending on the mode supported (HOST/DEVICE/DUAL_ROLE) the function
//! calls the coresponding USB mode initialization function
void usb_task_init(void)
{
#ifdef FREERTOS_USED
  // Create the semaphore
  vSemaphoreCreateBinary(usb_tsk_semphr);

  xTaskCreate(usb_task,
              configTSK_USB_NAME,
              configTSK_USB_STACK_SIZE,
              NULL,
              configTSK_USB_PRIORITY,
              NULL);
}


void usb_task(void *pvParameters)
{
#endif  // FREERTOS_USED
  // Register the USB interrupt handler to the interrupt controller and enable
  // the USB interrupt.
  Disable_global_interrupt();
  INTC_register_interrupt((__int_handler)&usb_general_interrupt, AVR32_USBB_IRQ, USB_INT_LEV);
  Enable_global_interrupt();

#ifdef FREERTOS_USED
  while (TRUE)
  {
    // Wait for the semaphore
    while (!xSemaphoreTake(usb_tsk_semphr, portMAX_DELAY));

#endif  // FREERTOS_USED
// ---- DUAL-ROLE DEVICE/HOST USB MODE -----------------------------------------
#if USB_DEVICE_FEATURE == ENABLED && USB_HOST_FEATURE == ENABLED
  #ifdef FREERTOS_USED
    if (usb_device_tsk) vTaskDelete(usb_device_tsk), usb_device_tsk = NULL;
    if (usb_host_tsk) vTaskDelete(usb_host_tsk), usb_host_tsk = NULL;
  #endif
    Usb_input_id_pin();
    Usb_enable_id_pin();
    if (Is_usb_id_device())
    {
      g_usb_mode = USB_MODE_DEVICE;
      usb_device_task_init();
    }
    else
    {
      private_sof_counter = 0;
      g_usb_mode = USB_MODE_HOST;
      usb_host_task_init();
    }
    g_old_usb_mode = g_usb_mode;  // Store current USB mode, for mode change detection
    Usb_raise_id_transition();  // Check no ID transition has been missed during initialization
    Usb_enable_id_interrupt();
    Enable_global_interrupt();
// -----------------------------------------------------------------------------

// ---- DEVICE-ONLY USB MODE ---------------------------------------------------
#elif USB_DEVICE_FEATURE == ENABLED
  #ifdef FREERTOS_USED
    if (usb_device_tsk) vTaskDelete(usb_device_tsk), usb_device_tsk = NULL;
  #endif
    Usb_force_device_mode();
    usb_device_task_init();
// -----------------------------------------------------------------------------

// ---- REDUCED-HOST-ONLY USB MODE ---------------------------------------------
#elif USB_HOST_FEATURE == ENABLED
  #ifdef FREERTOS_USED
    if (usb_host_tsk) vTaskDelete(usb_host_tsk), usb_host_tsk = NULL;
  #endif
    private_sof_counter = 0;
    Usb_force_host_mode();
    usb_host_task_init();
// -----------------------------------------------------------------------------

// ---- ERROR, NO MODE ENABLED -------------------------------------------------
#else
  #error At least one of USB_DEVICE_FEATURE and USB_HOST_FEATURE must be enabled
#endif
// -----------------------------------------------------------------------------
#ifdef FREERTOS_USED
  }
#endif
}


//! @brief Entry point of the USB mamnagement
//!
//! Depending on the USB mode supported (HOST/DEVICE/DUAL_ROLE) the function
//! calls the coresponding USB management function.
#ifndef FREERTOS_USED
void usb_task(void)
{
// ---- DUAL-ROLE DEVICE/HOST USB MODE -----------------------------------------
  #if USB_DEVICE_FEATURE == ENABLED && USB_HOST_FEATURE == ENABLED
  // Depending on current USB mode, launch the correct USB task (device or host)
  switch (g_usb_mode)
  {
  case USB_MODE_DEVICE:
    usb_device_task();
    break;
  case USB_MODE_HOST:
    usb_host_task();
    break;
  case USB_MODE_UNDEFINED:
  default:
    break;
  }
// -----------------------------------------------------------------------------

// ---- DEVICE-ONLY USB MODE ---------------------------------------------------
  #elif USB_DEVICE_FEATURE == ENABLED
  usb_device_task();
// -----------------------------------------------------------------------------

// ---- REDUCED-HOST-ONLY USB MODE ---------------------------------------------
  #elif USB_HOST_FEATURE == ENABLED
  usb_host_task();
// -----------------------------------------------------------------------------

// ---- ERROR, NO MODE ENABLED -------------------------------------------------
  #else
    #error At least one of USB_DEVICE_FEATURE and USB_HOST_FEATURE must be enabled
  #endif
// -----------------------------------------------------------------------------
}
#endif


//! @brief USB interrupt routine
//!
//! This function is called each time a USB interrupt occurs.
//! The following USB DEVICE events are taken in charge:
//! - VBus On / Off
//! - Start-of-Frame
//! - Suspend
//! - Wake-Up
//! - Resume
//! - Reset
//!
//! The following USB HOST events are taken in charge:
//! - Device connection
//! - Device Disconnection
//! - Start-of-Frame
//! - ID pin change
//! - SOF (or Keep alive in low-speed) sent
//! - Wake-up on USB line detected
//! - Pipe events
//!
//! For each event, the user can launch an action by completing the associated
//! \#define (see the conf_usb.h file to add actions on events).
//!
//! Note: Only interrupt events that are enabled are processed.
//!
//! Warning: If device and host tasks are not tasks in an RTOS, rough events
//! like ID transition, VBus transition, device disconnection, etc. that need to
//! kill then restart these tasks may lead to an undefined state if they occur
//! just before something is activated in the USB macro (e.g. pipe/endpoint
//! transfer...).
//!
//! @return Nothing in the standalone configuration; a boolean indicating
//!         whether a task switch is required in the FreeRTOS configuration
#ifdef FREERTOS_USED

#if __GNUC__
__attribute__((__noinline__))
#elif __ICCAVR32__
#pragma optimize = no_inline
#endif
static portBASE_TYPE usb_general_interrupt_non_naked(void)

#else

#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
static void usb_general_interrupt(void)

#endif
{
#ifdef FREERTOS_USED
  portBASE_TYPE task_woken = pdFALSE;
#endif
#if USB_HOST_FEATURE == ENABLED && USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE
  U8 i;
#endif

// ---------- DEVICE/HOST events management ------------------------------------
#if USB_DEVICE_FEATURE == ENABLED && USB_HOST_FEATURE == ENABLED
  // ID pin change detection
  if (Is_usb_id_transition() && Is_usb_id_interrupt_enabled())
  {
    g_usb_mode = (Is_usb_id_device()) ? USB_MODE_DEVICE : USB_MODE_HOST;
    Usb_ack_id_transition();
    if (g_usb_mode != g_old_usb_mode) // Basic debounce
    {
      // Previously in device mode, check if disconnection was detected
      if (g_old_usb_mode == USB_MODE_DEVICE)
      {
        if (usb_connected)
        {
          // Device mode diconnection actions
          usb_connected = FALSE;
          usb_configuration_nb = 0;
          Usb_vbus_off_action();
        }
      }
      // Previously in host mode, check if disconnection was detected
      else if (Is_host_attached())
      {
        // Host mode diconnection actions
        device_state = DEVICE_UNATTACHED;
        Host_device_disconnection_action();
      }
      LOG_STR(log_pin_id_changed);
      Usb_send_event((Is_usb_device()) ? EVT_USB_DEVICE_FUNCTION :
                                         EVT_USB_HOST_FUNCTION);
      Usb_id_transition_action();
      //! @todo ID pin hot state change!!!
      // Preliminary management: HARDWARE RESET!!!
  #if ID_PIN_CHANGE_GENERATE_RESET == ENABLE
      // Hot ID transition generates CPU reset
      Usb_disable();
      Usb_disable_otg_pad();
    #ifdef FREERTOS_USED
      // Release the semaphore in order to start a new device/host task
      taskENTER_CRITICAL();
      task_woken = xSemaphoreGiveFromISR(usb_tsk_semphr, task_woken);
      taskEXIT_CRITICAL();
    #else
//      Reset_CPU();
    #endif
  #endif
      g_old_usb_mode = g_usb_mode;  // Store current USB mode, for mode change detection
    }
  }
#endif  // End DEVICE/HOST FEATURE MODE

// ---------- DEVICE events management -----------------------------------------
#if USB_DEVICE_FEATURE == ENABLED
  #if USB_HOST_FEATURE == ENABLED
  // If both device and host features are enabled, check if device mode is engaged
  // (accessing the USB registers of a non-engaged mode, even with load operations,
  // may corrupt USB FIFO data).
  if (Is_usb_device())
  #endif
  {
    // VBus state detection
    if (Is_usb_vbus_transition() && Is_usb_vbus_interrupt_enabled())
    {
      Usb_ack_vbus_transition();
      if (Is_usb_vbus_high())
      {
        usb_start_device();
        Usb_send_event(EVT_USB_POWERED);
        Usb_vbus_on_action();
      }
      else
      {
        usb_connected = FALSE;
        usb_configuration_nb = 0;
        Usb_send_event(EVT_USB_UNPOWERED);
        Usb_vbus_off_action();
  #ifdef FREERTOS_USED
        // Release the semaphore in order to start a new device/host task
        taskENTER_CRITICAL();
        task_woken = xSemaphoreGiveFromISR(usb_tsk_semphr, task_woken);
        taskEXIT_CRITICAL();
  #endif
      }
    }
    // Device Start-of-Frame received
    if (Is_usb_sof() && Is_usb_sof_interrupt_enabled())
    {
      Usb_ack_sof();
      Usb_sof_action();
    }
    // Device Suspend event (no more USB activity detected)
    if (Is_usb_suspend() && Is_usb_suspend_interrupt_enabled())
    {
      Usb_ack_suspend();
      Usb_enable_wake_up_interrupt();
      (void)Is_usb_wake_up_interrupt_enabled();
      Usb_freeze_clock();
      Usb_send_event(EVT_USB_SUSPEND);
      Usb_suspend_action();
    }
    // Wake-up event (USB activity detected): Used to resume
    if (Is_usb_wake_up() && Is_usb_wake_up_interrupt_enabled())
    {
      Usb_unfreeze_clock();
      (void)Is_usb_clock_frozen();
      Usb_ack_wake_up();
      Usb_disable_wake_up_interrupt();
      Usb_wake_up_action();
      Usb_send_event(EVT_USB_WAKE_UP);
    }
    // Resume state bus detection
    if (Is_usb_resume() && Is_usb_resume_interrupt_enabled())
    {
      Usb_disable_wake_up_interrupt();
      Usb_ack_resume();
      Usb_disable_resume_interrupt();
      Usb_resume_action();
      Usb_send_event(EVT_USB_RESUME);
    }
    // USB bus reset detection
    if (Is_usb_reset() && Is_usb_reset_interrupt_enabled())
    {
      Usb_ack_reset();
      usb_init_device();
      Usb_reset_action();
      Usb_send_event(EVT_USB_RESET);
    }
  }
#endif  // End DEVICE FEATURE MODE

// ---------- HOST events management -------------------------------------------
#if USB_HOST_FEATURE == ENABLED
  #if USB_DEVICE_FEATURE == ENABLED
  // If both device and host features are enabled, check if host mode is engaged
  // (accessing the USB registers of a non-engaged mode, even with load operations,
  // may corrupt USB FIFO data).
  else
  #endif
  {
    // The device has been disconnected
    if (Is_host_device_disconnection() && Is_host_device_disconnection_interrupt_enabled())
    {
      host_disable_all_pipes();
      Host_ack_device_disconnection();
  #if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE
      reset_it_pipe_str();
  #endif
      device_state = DEVICE_UNATTACHED;
      LOG_STR(log_device_disconnected);
      Usb_send_event(EVT_HOST_DISCONNECTION);
      Host_device_disconnection_action();
  #ifdef FREERTOS_USED
      // Release the semaphore in order to start a new device/host task
      taskENTER_CRITICAL();
      task_woken = xSemaphoreGiveFromISR(usb_tsk_semphr, task_woken);
      taskEXIT_CRITICAL();
  #endif
    }
    // Device connection
    if (Is_host_device_connection() && Is_host_device_connection_interrupt_enabled())
    {
      Host_ack_device_connection();
      host_disable_all_pipes();
      Host_device_connection_action();
    }
    // Host Start-of-Frame has been sent
    if (Is_host_sof() && Is_host_sof_interrupt_enabled())
    {
      Host_ack_sof();
      Usb_send_event(EVT_HOST_SOF);
      private_sof_counter++;
      // Delay time-out management for interrupt tranfer mode in host mode
  #if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE && TIMEOUT_DELAY_ENABLE == ENABLE
      if (private_sof_counter >= 250) // Count 250 ms (SOF @ 1 ms)
      {
        private_sof_counter = 0;
        for (i = 0; i < MAX_PEP_NB; i++)
        {
          if (it_pipe_str[i].enable &&
              ++it_pipe_str[i].timeout > TIMEOUT_DELAY && Host_get_pipe_type(i) != TYPE_INTERRUPT)
          {
            it_pipe_str[i].enable = FALSE;
            it_pipe_str[i].status = PIPE_DELAY_TIMEOUT;
            Host_reset_pipe(i);
            if (!is_any_interrupt_pipe_active() && !g_sav_int_sof_enable) // If no more transfer is armed
            {
              Host_disable_sof_interrupt();
            }
            it_pipe_str[i].handler(PIPE_DELAY_TIMEOUT, it_pipe_str[i].nb_byte_processed);
          }
        }
      }
  #endif
      Host_sof_action();
    }
    // Host Wake-up has been received
    if (Is_host_hwup() && Is_host_hwup_interrupt_enabled())
    {
      // CAUTION: HWUP can be cleared only when USB clock is active (not frozen)!
      //! @todo Implement this on the silicon version
      //Pll_start_auto();               // First Restart the PLL for USB operation
      //Wait_pll_ready();               // Make sure PLL is locked
      Usb_unfreeze_clock();           // Enable clock on USB interface
      (void)Is_usb_clock_frozen();    // Make sure USB interface clock is enabled
      Host_disable_hwup_interrupt();  // Wake-up interrupt should be disabled as host is now awoken!
      Host_ack_hwup();                // Clear HWUP interrupt flag
      Usb_send_event(EVT_HOST_HWUP);  // Send software event
      Host_hwup_action();             // Map custom action
    }
  #if USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE
    // Host pipe interrupts
    while ((i = Host_get_interrupt_pipe_number()) < MAX_PEP_NB) usb_pipe_interrupt(i);
  #endif
  }
#endif  // End HOST FEATURE MODE

#ifdef FREERTOS_USED
  return task_woken;
#endif
}


#if USB_DEVICE_FEATURE == ENABLED
void usb_suspend_action(void)
{
  Enable_global_interrupt();
  //! @todo Implement this on the silicon version
  //Enter_power_down_mode();  // For example...
}
#endif


#if USB_HOST_FEATURE == ENABLED
void host_suspend_action(void)
{
  Enable_global_interrupt();
  //! @todo Implement this on the silicon version
  //Enter_power_down_mode();  // For example...
}

U32  host_get_timeout( void )
{
  return private_sof_counter;
}

#endif
