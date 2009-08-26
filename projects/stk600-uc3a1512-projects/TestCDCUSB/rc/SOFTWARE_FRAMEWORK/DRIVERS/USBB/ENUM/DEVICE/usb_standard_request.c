/* This source file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3A-1.4.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
 *
 * \brief Processing of USB device enumeration requests.
 *
 * This file contains the USB control endpoint management
 * routines corresponding to the standard enumeration process (refer to
 * chapter 9 of the USB specification).
 * This file calls routines of the usb_specific_request.c file for
 * non-standard request management.
 * The enumeration parameters (descriptor tables) are contained in the
 * usb_descriptors.c file.
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


//_____ I N C L U D E S ____________________________________________________

#include "conf_usb.h"


#if USB_DEVICE_FEATURE == ENABLED

#include "usb_drv.h"
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "usb_specific_request.h"
#include "usb_task.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ P R I V A T E   D E C L A R A T I O N S ____________________________

static  void    usb_get_descriptor   (void);
static  void    usb_set_address      (void);
static  void    usb_set_configuration(void);
static  void    usb_clear_feature    (void);
static  void    usb_set_feature      (void);
static  void    usb_get_status       (void);
static  void    usb_get_configuration(void);
static  void    usb_get_interface    (void);
static  void    usb_set_interface    (void);


//_____ D E C L A R A T I O N S ____________________________________________

                  Bool                                endpoint_status[NB_ENDPOINTS];
        const     void                               *pbuffer;
                  U8                                  data_to_transfer;
static            U8                                  bmRequestType;
        volatile  U8                                  usb_configuration_nb;
extern  volatile  Bool                                usb_connected;
extern  const     S_usb_device_descriptor             usb_user_device_descriptor;
extern  const     S_usb_user_configuration_descriptor usb_user_configuration_descriptor;


//! This function reads the SETUP request sent to the default control endpoint
//! and calls the appropriate function. When exiting of the usb_read_request
//! function, the device is ready to manage the next request.
//!
//! If the received request is not supported or a non-standard USB request, the function
//! will call the custom decoding function in usb_specific_request module.
//!
//! @note List of supported requests:
//! GET_DESCRIPTOR
//! GET_CONFIGURATION
//! SET_ADDRESS
//! SET_CONFIGURATION
//! CLEAR_FEATURE
//! SET_FEATURE
//! GET_STATUS
//!
void usb_process_request(void)
{
  U8 bmRequest;

  Usb_reset_endpoint_fifo_access(EP_CONTROL);
  bmRequestType = Usb_read_endpoint_data(EP_CONTROL, 8);
  bmRequest     = Usb_read_endpoint_data(EP_CONTROL, 8);

  switch (bmRequest)
  {
  case GET_DESCRIPTOR:
    if (bmRequestType == 0x80) usb_get_descriptor();
    else goto unsupported_request;
    break;

  case GET_CONFIGURATION:
    if (bmRequestType == 0x80) usb_get_configuration();
    else goto unsupported_request;
    break;

  case SET_ADDRESS:
    if (bmRequestType == 0x00) usb_set_address();
    else goto unsupported_request;
    break;

  case SET_CONFIGURATION:
    if (bmRequestType == 0x00) usb_set_configuration();
    else goto unsupported_request;
    break;

  case CLEAR_FEATURE:
    if (bmRequestType <= 0x02) usb_clear_feature();
    else goto unsupported_request;
    break;

  case SET_FEATURE:
    if (bmRequestType <= 0x02) usb_set_feature();
    else goto unsupported_request;
    break;

  case GET_STATUS:
    if (0x7F < bmRequestType && bmRequestType <= 0x82) usb_get_status();
    else goto unsupported_request;
    break;

  case GET_INTERFACE:
    if (bmRequestType == 0x81) usb_get_interface();
    else goto unsupported_request;
    break;

  case SET_INTERFACE:
    if (bmRequestType == 0x01) usb_set_interface();
    else goto unsupported_request;
    break;

  case SET_DESCRIPTOR:
  case SYNCH_FRAME:
  default:  //!< unsupported request => call to user read request
unsupported_request:
    if (!usb_user_read_request(bmRequestType, bmRequest))
    {
      Usb_enable_stall_handshake(EP_CONTROL);
      Usb_ack_setup_received_free();
    }
    break;
  }
}


//! This function manages the SET ADDRESS request. When complete, the device
//! will filter the requests using the new address.
//!
void usb_set_address(void)
{
  U8 addr = Usb_read_endpoint_data(EP_CONTROL, 8);
  Usb_configure_address(addr);

  Usb_ack_setup_received_free();

  Usb_ack_control_in_ready_send();    //!< send a ZLP for STATUS phase
  while (!Is_usb_control_in_ready()); //!< waits for status phase done
                                      //!< before using the new address
  Usb_enable_address();
}


//! This function manages the SET CONFIGURATION request. If the selected
//! configuration is valid, this function call the usb_user_endpoint_init()
//! function that will configure the endpoints following the configuration
//! number.
//!
void usb_set_configuration(void)
{
  U8 configuration_number = Usb_read_endpoint_data(EP_CONTROL, 8);

  if (configuration_number <= NB_CONFIGURATION)
  {
    Usb_ack_setup_received_free();
    usb_configuration_nb = configuration_number;

    Usb_ack_control_in_ready_send();              //!< send a ZLP for STATUS phase

    usb_user_endpoint_init(usb_configuration_nb); //!< endpoint configuration
    Usb_set_configuration_action();
  }
  else
  {
    //!< keep that order (set StallRq/clear RxSetup) or a
    //!< OUT request following the SETUP may be acknowledged
    Usb_enable_stall_handshake(EP_CONTROL);
    Usb_ack_setup_received_free();
  }
}


//! This function manages the GET DESCRIPTOR request. The device descriptor,
//! the configuration descriptor and the device qualifier are supported. All
//! other descriptors must be supported by the usb_user_get_descriptor
//! function.
//! Only 1 configuration is supported.
//!
void usb_get_descriptor(void)
{
  Bool    zlp;
  U16     wLength;
  U8      descriptor_type;
  U8      string_type;
  Union32 temp;

  zlp             = FALSE;                                  /* no zero length packet */
  string_type     = Usb_read_endpoint_data(EP_CONTROL, 8);  /* read LSB of wValue    */
  descriptor_type = Usb_read_endpoint_data(EP_CONTROL, 8);  /* read MSB of wValue    */

  switch (descriptor_type)
  {
  case DEVICE_DESCRIPTOR:
    data_to_transfer = Usb_get_dev_desc_length();   //!< sizeof(usb_dev_desc);
    pbuffer          = Usb_get_dev_desc_pointer();
    break;

  case CONFIGURATION_DESCRIPTOR:
    data_to_transfer = Usb_get_conf_desc_length();  //!< sizeof(usb_conf_desc);
    pbuffer          = Usb_get_conf_desc_pointer();
    break;

  default:
    if (!usb_user_get_descriptor(descriptor_type, string_type))
    {
      Usb_enable_stall_handshake(EP_CONTROL);
      Usb_ack_setup_received_free();
      return;
    }
    break;
  }

  temp.u32 = Usb_read_endpoint_data(EP_CONTROL, 32);      //!< read wIndex and wLength with a 32-bit access
                                                          //!< since this access is aligned with a 32-bit
                                                          //!< boundary from the beginning of the endpoint
  wLength = usb_format_usb_to_mcu_data(16, temp.u16[1]);  //!< ignore wIndex, keep and format wLength
  Usb_ack_setup_received_free();                          //!< clear the setup received flag

  if (wLength > data_to_transfer)
  {
    zlp = !(data_to_transfer % EP_CONTROL_LENGTH);  //!< zero length packet condition
  }
  else
  {
    // No need to test ZLP sending since we send the exact number of bytes as
    // expected by the host.
    data_to_transfer = (U8)wLength; //!< send only requested number of data bytes
  }

  Usb_ack_nak_out(EP_CONTROL);

  while (data_to_transfer && !Is_usb_nak_out(EP_CONTROL))
  {
    while (!Is_usb_control_in_ready() && !Is_usb_nak_out(EP_CONTROL));

    if (Is_usb_nak_out(EP_CONTROL))
      break;  // don't clear the flag now, it will be cleared after

    Usb_reset_endpoint_fifo_access(EP_CONTROL);
    data_to_transfer = usb_write_ep_txpacket(EP_CONTROL, pbuffer,
                                             data_to_transfer, &pbuffer);
    if (Is_usb_nak_out(EP_CONTROL))
      break;
    else
      Usb_ack_control_in_ready_send();  //!< Send data until necessary
  }

  if (zlp && !Is_usb_nak_out(EP_CONTROL))
  {
    while (!Is_usb_control_in_ready());
    Usb_ack_control_in_ready_send();
  }

  while (!Is_usb_nak_out(EP_CONTROL));
  Usb_ack_nak_out(EP_CONTROL);
  while (!Is_usb_control_out_received());
  Usb_ack_control_out_received_free();
}


//! This function manages the GET CONFIGURATION request. The current
//! configuration number is returned.
//!
void usb_get_configuration(void)
{
  Usb_ack_setup_received_free();

  Usb_reset_endpoint_fifo_access(EP_CONTROL);
  Usb_write_endpoint_data(EP_CONTROL, 8, usb_configuration_nb);
  Usb_ack_control_in_ready_send();

  while (!Is_usb_control_out_received());
  Usb_ack_control_out_received_free();
}


//! This function manages the GET STATUS request. The device, interface or
//! endpoint status is returned.
//!
void usb_get_status(void)
{
  U8 wIndex;

  switch (bmRequestType)
  {
  case REQUEST_DEVICE_STATUS:
    Usb_ack_setup_received_free();
    Usb_reset_endpoint_fifo_access(EP_CONTROL);
    Usb_write_endpoint_data(EP_CONTROL, 8, DEVICE_STATUS);
    break;

  case REQUEST_INTERFACE_STATUS:
    Usb_ack_setup_received_free();
    Usb_reset_endpoint_fifo_access(EP_CONTROL);
    Usb_write_endpoint_data(EP_CONTROL, 8, INTERFACE_STATUS);
    break;

  case REQUEST_ENDPOINT_STATUS:
    Usb_read_endpoint_data(EP_CONTROL, 16); //!< dummy read (wValue)
    wIndex = Usb_read_endpoint_data(EP_CONTROL, 8);
    wIndex = Get_desc_ep_nbr(wIndex);
    Usb_ack_setup_received_free();
    Usb_reset_endpoint_fifo_access(EP_CONTROL);
    Usb_write_endpoint_data(EP_CONTROL, 8, endpoint_status[wIndex]);
    break;

  default:
    Usb_enable_stall_handshake(EP_CONTROL);
    Usb_ack_setup_received_free();
    return;
  }

  Usb_write_endpoint_data(EP_CONTROL, 8, 0x00);
  Usb_ack_control_in_ready_send();

  while (!Is_usb_control_out_received());
  Usb_ack_control_out_received_free();
}


//! This function manages the SET FEATURE request. The USB test modes are
//! supported by this function.
//!
void usb_set_feature(void)
{
  U8 wValue;
  U8 wIndex;

  if (bmRequestType == INTERFACE_TYPE)
  {
    //!< keep that order (set StallRq/clear RxSetup) or a
    //!< OUT request following the SETUP may be acknowledged
    Usb_enable_stall_handshake(EP_CONTROL);
    Usb_ack_setup_received_free();
  }
  else if (bmRequestType == ENDPOINT_TYPE)
  {
    wValue = Usb_read_endpoint_data(EP_CONTROL, 8);

    if (wValue == FEATURE_ENDPOINT_HALT)
    {
      Usb_read_endpoint_data(EP_CONTROL, 8);  //!< dummy read (MSB of wValue)
      wIndex = Usb_read_endpoint_data(EP_CONTROL, 8);
      wIndex = Get_desc_ep_nbr(wIndex);

      if (wIndex == EP_CONTROL)
      {
        Usb_enable_stall_handshake(EP_CONTROL);
        Usb_ack_setup_received_free();
      }
      else if (Is_usb_endpoint_enabled(wIndex))
      {
        Usb_enable_stall_handshake(wIndex);
        endpoint_status[wIndex] = ENABLED;
        Usb_ack_setup_received_free();
        Usb_ack_control_in_ready_send();
      }
      else
      {
        Usb_enable_stall_handshake(EP_CONTROL);
        Usb_ack_setup_received_free();
      }
    }
    else
    {
      Usb_enable_stall_handshake(EP_CONTROL);
      Usb_ack_setup_received_free();
    }
  }
}


//! This function manages the CLEAR FEATURE request.
//!
void usb_clear_feature(void)
{
  U8 wValue;
  U8 wIndex;

  if (bmRequestType == ZERO_TYPE || bmRequestType == INTERFACE_TYPE)
  {
    //!< keep that order (set StallRq/clear RxSetup) or a
    //!< OUT request following the SETUP may be acknowledged
    Usb_enable_stall_handshake(EP_CONTROL);
    Usb_ack_setup_received_free();
  }
  else if (bmRequestType == ENDPOINT_TYPE)
  {
    wValue = Usb_read_endpoint_data(EP_CONTROL, 8);

    if (wValue == FEATURE_ENDPOINT_HALT)
    {
      Usb_read_endpoint_data(EP_CONTROL, 8);  //!< dummy read (MSB of wValue)
      wIndex = Usb_read_endpoint_data(EP_CONTROL, 8);
      wIndex = Get_desc_ep_nbr(wIndex);

      if (Is_usb_endpoint_enabled(wIndex))
      {
        if (wIndex != EP_CONTROL)
        {
          Usb_disable_stall_handshake(wIndex);
          Usb_reset_endpoint(wIndex);
          Usb_reset_data_toggle(wIndex);
        }
        endpoint_status[wIndex] = DISABLED;
        Usb_ack_setup_received_free();
        Usb_ack_control_in_ready_send();
      }
      else
      {
        Usb_enable_stall_handshake(EP_CONTROL);
        Usb_ack_setup_received_free();
      }
    }
    else
    {
      Usb_enable_stall_handshake(EP_CONTROL);
      Usb_ack_setup_received_free();
    }
  }
}


//! This function manages the GET INTERFACE request.
//!
void usb_get_interface(void)
{
  Usb_ack_setup_received_free();
  Usb_ack_control_in_ready_send();  //!< send a ZLP for STATUS phase

  while (!Is_usb_control_out_received());
  Usb_ack_control_out_received_free();
}


//! This function manages the SET INTERFACE request.
//!
void usb_set_interface(void)
{
  Usb_ack_setup_received_free();
  Usb_ack_control_in_ready_send();  //!< send a ZLP for STATUS phase
  while (!Is_usb_control_in_ready());
}


#endif  // USB_DEVICE_FEATURE == ENABLED
