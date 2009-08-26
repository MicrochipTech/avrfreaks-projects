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
#include "usart.h"
#include "audio_example.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ P R I V A T E   D E C L A R A T I O N S ____________________________

extern const    void *pbuffer;
extern          U8    data_to_transfer;
extern Bool mute;
extern S16 volume;


//_____ D E C L A R A T I O N S ____________________________________________

//! @brief This function configures the endpoints of the device application.
//! This function is called when the set configuration request has been received.
//!
void usb_user_endpoint_init(U8 conf_nb)
{
  (void)Usb_configure_endpoint(EP_AUDIO_IN,
                         EP_ATTRIBUTES_1,
                         DIRECTION_IN,
                         EP_SIZE_1,
                         DOUBLE_BANK);

}


//! This function is called by the standard USB read request function when
//! the USB request is not supported. This function returns TRUE when the
//! request is processed. This function returns FALSE if the request is not
//! supported. In this case, a STALL handshake will be automatically
//! sent by the standard USB read request function.
//!
Bool usb_user_read_request(U8 bmRequestType, U8 bmRequest)
{
  switch (bmRequest)
  {
     case BR_REQUEST_SET_CUR:
        audio_set_cur();
        return TRUE;
        // No need to break here !

     case BR_REQUEST_SET_MIN:     //! Set MIN,MAX and RES not supported
     case BR_REQUEST_SET_MAX:
     case BR_REQUEST_SET_RES:
        return FALSE;
        // No need to break here !

     case BR_REQUEST_GET_CUR:
        audio_get_cur();
        return TRUE;
        // No need to break here !
        
     case BR_REQUEST_GET_MIN:
        audio_get_min();
        return TRUE;
        // No need to break here !
        
     case BR_REQUEST_GET_MAX:
        audio_get_max();
        return TRUE;
        // No need to break here !
        
     case BR_REQUEST_GET_RES:
        audio_get_res();
        return TRUE;
        // No need to break here !

     default:
        return FALSE;
        // No need to break here !
  }
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


void audio_get_min(void)
{
   U16 cs;      // in wValue
   U16 i_unit;  // in wIndex
   U16 length;  // in wLength

   LSB(cs)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(cs)=Usb_read_endpoint_data(EP_CONTROL, 8);
   LSB(i_unit)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(i_unit)=Usb_read_endpoint_data(EP_CONTROL, 8);
   LSB(length)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(length)=Usb_read_endpoint_data(EP_CONTROL, 8);
   Usb_ack_setup_received_free();

   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   if( i_unit==MIC_FEATURE_UNIT_ID )
   {
      switch (cs)
      {
      case CS_MUTE:
         if( length==1 )
         {
            Usb_write_endpoint_data(EP_CONTROL, 8, mute);
         }
         break;
      case CS_VOLUME:
         if( length==2 )
         {
            Usb_write_endpoint_data(EP_CONTROL, 16, Usb_format_mcu_to_usb_data(16, VOL_MIN));
         }
         break;
      }
   }
   Usb_ack_control_in_ready_send();
   while(!Is_usb_control_out_received());
   Usb_ack_control_out_received_free();
}

void audio_get_max(void)
{
   U16 i_unit;
   U16 length;
   U16 cs;

   LSB(cs)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(cs)=Usb_read_endpoint_data(EP_CONTROL, 8);
   LSB(i_unit)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(i_unit)=Usb_read_endpoint_data(EP_CONTROL, 8);
   LSB(length)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(length)=Usb_read_endpoint_data(EP_CONTROL, 8);
   Usb_ack_setup_received_free();

   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   if( i_unit==MIC_FEATURE_UNIT_ID )
   {
      switch (cs)
      {
      case CS_MUTE:
         if( length==1 )
         {
            Usb_write_endpoint_data(EP_CONTROL, 8, mute);
         }
         break;
      case CS_VOLUME:
         if( length==2 )
         {
            Usb_write_endpoint_data(EP_CONTROL, 16, Usb_format_mcu_to_usb_data(16, VOL_MAX));
         }
         break;
      }
   }
   Usb_ack_control_in_ready_send();
   while(!Is_usb_control_out_received());
   Usb_ack_control_out_received_free();
}

void audio_get_res(void)
{
   U16 i_unit;
   U16 length;
   U16 cs;

   LSB(cs)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(cs)=Usb_read_endpoint_data(EP_CONTROL, 8);
   LSB(i_unit)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(i_unit)=Usb_read_endpoint_data(EP_CONTROL, 8);
   LSB(length)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(length)=Usb_read_endpoint_data(EP_CONTROL, 8);
   Usb_ack_setup_received_free();

   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   if( i_unit==MIC_FEATURE_UNIT_ID )
   {
      switch (cs)
      {
      case CS_MUTE:
         if( length==1 )
         {
            Usb_write_endpoint_data(EP_CONTROL, 8, mute);
         }
         break;
      case CS_VOLUME:
         if( length==2 )
         {
            Usb_write_endpoint_data(EP_CONTROL, 16, Usb_format_mcu_to_usb_data(16, VOL_RES));
         }
         break;
      }
   }
   Usb_ack_control_in_ready_send();
   while(!Is_usb_control_out_received());
   Usb_ack_control_out_received_free();
}

void audio_get_cur(void)
{
   U16 i_unit;
   U16 length;
   U16 cs;

   LSB(cs)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(cs)=Usb_read_endpoint_data(EP_CONTROL, 8);
   LSB(i_unit)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(i_unit)=Usb_read_endpoint_data(EP_CONTROL, 8);
   LSB(length)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(length)=Usb_read_endpoint_data(EP_CONTROL, 8);
   Usb_ack_setup_received_free();

   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   if( i_unit==MIC_FEATURE_UNIT_ID )
   {
      switch (cs)
      {
      case CS_MUTE:
         if( length==1 )
         {
            Usb_write_endpoint_data(EP_CONTROL, 8, mute);
         }
         break;
      case CS_VOLUME:
         if( length==2 )
         {
            Usb_write_endpoint_data(EP_CONTROL, 16, Usb_format_mcu_to_usb_data(16, volume));
         }
         break;
      }
   }
   Usb_ack_control_in_ready_send();

   while(!Is_usb_control_out_received());
   Usb_ack_control_out_received_free();
}

void audio_set_cur(void)
{
   U16 i_unit;
   U16 length;
   U16 cs;

   LSB(cs)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(cs)=Usb_read_endpoint_data(EP_CONTROL, 8);
   LSB(i_unit)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(i_unit)=Usb_read_endpoint_data(EP_CONTROL, 8);
   LSB(length)=Usb_read_endpoint_data(EP_CONTROL, 8);
   MSB(length)=Usb_read_endpoint_data(EP_CONTROL, 8);
   Usb_ack_setup_received_free();

   while(!Is_usb_control_out_received());
   Usb_reset_endpoint_fifo_access(EP_CONTROL);
   if( i_unit==MIC_FEATURE_UNIT_ID )
   {
      switch (cs)
      {
      case CS_MUTE:
         if( length==1 )
         {
            mute=Usb_read_endpoint_data(EP_CONTROL, 8);
         }
         break;
      case CS_VOLUME:
         if( length==2 )
         {
            LSB(volume)= Usb_read_endpoint_data(EP_CONTROL, 8);
            MSB(volume)= Usb_read_endpoint_data(EP_CONTROL, 8);
         }
         break;
      }
   }
   Usb_ack_control_out_received_free();

   Usb_ack_control_in_ready_send();
   while (!Is_usb_control_in_ready());
}

#endif  // USB_DEVICE_FEATURE == ENABLED
