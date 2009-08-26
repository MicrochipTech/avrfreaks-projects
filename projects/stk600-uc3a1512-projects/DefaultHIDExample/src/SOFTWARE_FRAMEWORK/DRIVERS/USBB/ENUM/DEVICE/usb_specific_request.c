/* This source file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3A-1.4.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
 *
 * \brief Processing of USB device specific enumeration requests.
 *
 * This file contains the specific request decoding for enumeration process.
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


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ P R I V A T E   D E C L A R A T I O N S ____________________________

extern volatile U8    usb_hid_report_mouse[];

extern const    void *pbuffer;
extern          U8    data_to_transfer;


//_____ D E C L A R A T I O N S ____________________________________________

//! This function manages the HID Get_Descriptor request.
//!
static void hid_get_descriptor(U8 size_of_report, const U8* p_usb_hid_report)
{
  Bool  zlp;
  U16   wIndex;
  U16   wLength;

  zlp = FALSE;                                              /* no zero length packet */

  data_to_transfer = size_of_report;
  pbuffer          = p_usb_hid_report;

  wIndex = Usb_read_endpoint_data(EP_CONTROL, 16);
  wIndex = usb_format_usb_to_mcu_data(16, wIndex);
  wLength = Usb_read_endpoint_data(EP_CONTROL, 16);
  wLength = usb_format_usb_to_mcu_data(16, wLength);
  Usb_ack_setup_received_free();                          //!< clear the setup received flag

  if (wLength > data_to_transfer)
  {
    zlp = !(data_to_transfer % EP_CONTROL_LENGTH);  //!< zero length packet condition
  }
  else
  {
    data_to_transfer = (U8)wLength; //!< send only requested number of data bytes
  }

  Usb_ack_nak_out(EP_CONTROL);

  while (data_to_transfer && (!Is_usb_nak_out(EP_CONTROL)))
  {
    while( !Is_usb_control_in_ready() && !Is_usb_nak_out(EP_CONTROL) );

    if( Is_usb_nak_out(EP_CONTROL) )
       break;    // don't clear the flag now, it will be cleared after

    Usb_reset_endpoint_fifo_access(EP_CONTROL);
    data_to_transfer = usb_write_ep_txpacket(EP_CONTROL, pbuffer,
                                             data_to_transfer, &pbuffer);
    if( Is_usb_nak_out(EP_CONTROL) )
       break;
    else
       Usb_ack_control_in_ready_send();  //!< Send data until necessary
  }

  if ( zlp && (!Is_usb_nak_out(EP_CONTROL)) )
  {
    while (!Is_usb_control_in_ready());
    Usb_ack_control_in_ready_send();
  }

  while (!(Is_usb_nak_out(EP_CONTROL)));
  Usb_ack_nak_out(EP_CONTROL);
  while (!Is_usb_control_out_received());
  Usb_ack_control_out_received_free();
}


//! @brief This function configures the endpoints of the device application.
//! This function is called when the set configuration request has been received.
//!
void usb_user_endpoint_init(U8 conf_nb)
{
  (void)Usb_configure_endpoint(EP_HID_MOUSE_IN,
                               EP_ATTRIBUTES_1,
                               DIRECTION_IN,
                               EP_SIZE_1,
                               SINGLE_BANK);
}


//! This function is called by the standard USB read request function when
//! the USB request is not supported. This function returns TRUE when the
//! request is processed. This function returns FALSE if the request is not
//! supported. In this case, a STALL handshake will be automatically
//! sent by the standard USB read request function.
//!
Bool usb_user_read_request(U8 type, U8 request)
{
  U8 descriptor_type;

  Usb_read_endpoint_data(EP_CONTROL, 8); // string_type
  descriptor_type = Usb_read_endpoint_data(EP_CONTROL, 8);

  switch (request)
  {
  case GET_DESCRIPTOR:
    switch (descriptor_type)
    {
    case HID_DESCRIPTOR:
      hid_get_descriptor(
         sizeof(usb_conf_desc.hid_mouse)
      ,  (const U8*)&usb_conf_desc.hid_mouse);
      return TRUE;

    case HID_REPORT_DESCRIPTOR:
      hid_get_descriptor(
         sizeof(usb_hid_report_descriptor_mouse)
      ,  usb_hid_report_descriptor_mouse);
      return TRUE;

    default:
      break;
    }
    break;

  case HID_GET_REPORT:
    switch (descriptor_type)
    {
    case HID_REPORT_INPUT:
      //hid_get_descriptor( 
      //   sizeof(usb_hid_report_mouse)
      //,  usb_hid_report_mouse );
      return TRUE;

    default:
      break;
    }
    break;

  case HID_SET_REPORT:
    switch (descriptor_type)
    {
    case HID_REPORT_OUTPUT:
      Usb_ack_setup_received_free();
      while (!Is_usb_control_out_received());
      Usb_ack_control_out_received_free();
      Usb_ack_control_in_ready_send();
      while (!Is_usb_control_in_ready());
      return TRUE;

    default:
      break;
    }
    break;

  case HID_SET_IDLE:
    Usb_ack_setup_received_free();
    Usb_ack_control_in_ready_send();
    while (!Is_usb_control_in_ready());
    return TRUE;

  default:
    break;
  }

  return FALSE;
}


//! This function returns the size and the pointer on a user information
//! structure
//!
Bool usb_user_get_descriptor(U8 type, U8 string)
{
  pbuffer = NULL;

  switch (type)
  {
  case STRING_DESCRIPTOR:
    switch (string)
    {
    case LANG_ID:
      data_to_transfer = sizeof(usb_user_language_id);
      pbuffer = &usb_user_language_id;
      break;

    case MAN_INDEX:
      data_to_transfer = sizeof(usb_user_manufacturer_string_descriptor);
      pbuffer = &usb_user_manufacturer_string_descriptor;
      break;

    case PROD_INDEX:
      data_to_transfer = sizeof(usb_user_product_string_descriptor);
      pbuffer = &usb_user_product_string_descriptor;
      break;

    case SN_INDEX:
      data_to_transfer = sizeof(usb_user_serial_number);
      pbuffer = &usb_user_serial_number;
      break;

    default:
      break;
    }
    break;

  default:
    break;
  }

  return pbuffer != NULL;
}


#endif  // USB_DEVICE_FEATURE == ENABLED
