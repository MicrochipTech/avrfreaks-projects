/* This source file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3A-1.4.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
 *
 * \brief USB identifiers.
 *
 * This file contains the USB parameters that uniquely identify the USB
 * application through descriptor tables.
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

// usb_user_device_descriptor
const S_usb_device_descriptor usb_dev_desc =
{
  sizeof(S_usb_device_descriptor),
  DEVICE_DESCRIPTOR,
  Usb_format_mcu_to_usb_data(16, USB_SPECIFICATION),
  DEVICE_CLASS,
  DEVICE_SUB_CLASS,
  DEVICE_PROTOCOL,
  EP_CONTROL_LENGTH,
  Usb_format_mcu_to_usb_data(16, VENDOR_ID),
  Usb_format_mcu_to_usb_data(16, PRODUCT_ID),
  Usb_format_mcu_to_usb_data(16, RELEASE_NUMBER),
  MAN_INDEX,
  PROD_INDEX,
  SN_INDEX,
  NB_CONFIGURATION
};


// usb_user_configuration_descriptor FS
const S_usb_user_configuration_descriptor usb_conf_desc =
{
   {  sizeof(S_usb_configuration_descriptor)
   ,  CONFIGURATION_DESCRIPTOR
   ,  Usb_format_mcu_to_usb_data(16, sizeof(S_usb_user_configuration_descriptor))
   ,  NB_INTERFACE
   ,  CONF_NB
   ,  CONF_INDEX
   ,  CONF_ATTRIBUTES
   ,  MAX_POWER
   }
 ,
   {  sizeof(S_usb_interface_descriptor)
   ,  INTERFACE_DESCRIPTOR
   ,  INTERFACE_NB
   ,  ALTERNATE
   ,  NB_ENDPOINT
   ,  INTERFACE_CLASS
   ,  INTERFACE_SUB_CLASS
   ,  INTERFACE_PROTOCOL
   ,  INTERFACE_INDEX
   }
 ,
   {  sizeof(S_usb_ac_interface_descriptor)
   ,  CS_INTERFACE
   ,  HEADER_SUB_TYPE
   ,  Usb_format_mcu_to_usb_data(16, AUDIO_CLASS_REVISION)
   ,  Usb_format_mcu_to_usb_data(16, sizeof(S_usb_ac_interface_descriptor)+sizeof(S_usb_mic_in_ter_descriptor)\
                                    +sizeof(S_usb_feature_unit_descriptor)+sizeof(S_usb_mic_out_ter_descriptor))
   ,  NB_OF_STREAMING_INTERFACE
   ,  BELONGS_AUDIO_INTERFACE
   }  
 ,
   {  sizeof(S_usb_mic_in_ter_descriptor)
   ,  CS_INTERFACE
   ,  INPUT_TERMINAL_SUB_TYPE
   ,  INPUT_TERMINAL_ID
   ,  Usb_format_mcu_to_usb_data(16, INPUT_TERMINAL_TYPE)
   ,  INPUT_TERMINAL_ASSOCIATION
   ,  INPUT_TERMINAL_NB_CHANNELS
   ,  Usb_format_mcu_to_usb_data(16, INPUT_TERMINAL_CHANNEL_CONF)
   ,  INPUT_TERMINAL_CH_NAME_ID
   ,  0x00
   } 
,
   {  sizeof(S_usb_feature_unit_descriptor)
   ,  CS_INTERFACE
   ,  FEATURE_UNIT_SUB_TYPE
   ,  MIC_FEATURE_UNIT_ID
   ,  MIC_FEATURE_UNIT_SOURCE_ID
   ,  0x01
   ,  Usb_format_mcu_to_usb_data(16, MIC_BMA_CONTROLS)
   }  
,
   {  sizeof(S_usb_mic_out_ter_descriptor)
   ,  CS_INTERFACE
   ,  OUTPUT_TERMINAL_SUB_TYPE
   ,  OUTPUT_TERMINAL_ID
   ,  Usb_format_mcu_to_usb_data(16, OUTPUT_TERMINAL_TYPE)
   ,  OUTPUT_TERMINAL_ASSOCIATION
   ,  OUTPUT_TERMINAL_SOURCE_ID
   ,  0x00
   }  
,  
   {  sizeof(S_usb_as_interface_descriptor)
   ,  INTERFACE_DESCRIPTOR
   ,  STD_AS_INTERFACE_NB
   ,  ALT0_AS_INTERFACE_INDEX
   ,  ALT0_AS_NB_ENDPOINT
   ,  ALT0_AS_INTERFACE_CLASS
   ,  ALT0_AS_INTERFACE_SUB_CLASS
   ,  ALT0_AS_INTERFACE_PROTOCOL
   ,  0x00
   }  
,  
   {  sizeof(S_usb_as_interface_descriptor)
   ,  INTERFACE_DESCRIPTOR
   ,  STD_AS_INTERFACE_NB
   ,  ALT1_AS_INTERFACE_INDEX
   ,  ALT1_AS_NB_ENDPOINT
   ,  ALT1_AS_INTERFACE_CLASS
   ,  ALT1_AS_INTERFACE_SUB_CLASS
   ,  ALT1_AS_INTERFACE_PROTOCOL
   ,  0x00
   }  
,  
   {  sizeof(S_usb_as_g_interface_descriptor)
   ,  CS_INTERFACE
   ,  GENERAL_SUB_TYPE
   ,  AS_TERMINAL_LINK
   ,  AS_DELAY
   ,  Usb_format_mcu_to_usb_data(16, AS_FORMAT_TAG)
   }  
,  
   {  sizeof(S_usb_format_type)
   ,  CS_INTERFACE
   ,  FORMAT_SUB_TYPE
   ,  FORMAT_TYPE
   ,  FORMAT_NB_CHANNELS
   ,  FORMAT_FRAME_SIZE
   ,  FORMAT_BIT_RESOLUTION
   ,  FORMAT_SAMPLE_FREQ_NB
   ,  Usb_format_mcu_to_usb_data(16, FORMAT_LSBYTE_SAMPLE_FREQ)
   ,  FORMAT_MSBYTE_SAMPLE_FREQ
   }  
,  
   {   sizeof(S_usb_endpoint_audio_descriptor)
   ,   ENDPOINT_DESCRIPTOR
   ,   ENDPOINT_NB_1
   ,   EP_ATTRIBUTES_1
   ,   Usb_format_mcu_to_usb_data(16, EP_SIZE_1)
   ,   EP_INTERVAL_1
   ,   0x00
   ,   0x00
   }  
,  
   {  sizeof(S_usb_endpoint_audio_specific)
   ,  CS_ENDPOINT
   ,  GENERAL_SUB_TYPE
   ,  AUDIO_EP_ATRIBUTES
   ,  AUDIO_EP_DELAY_UNIT
   ,  Usb_format_mcu_to_usb_data(16, AUDIO_EP_LOCK_DELAY)
   }  
};


// usb_user_language_id
const S_usb_language_id usb_user_language_id =
{
  sizeof(S_usb_language_id),
  STRING_DESCRIPTOR,
  Usb_format_mcu_to_usb_data(16, LANGUAGE_ID)
};


// usb_user_manufacturer_string_descriptor
const S_usb_manufacturer_string_descriptor usb_user_manufacturer_string_descriptor =
{
  sizeof(S_usb_manufacturer_string_descriptor),
  STRING_DESCRIPTOR,
  USB_MANUFACTURER_NAME
};


// usb_user_product_string_descriptor
const S_usb_product_string_descriptor usb_user_product_string_descriptor =
{
  sizeof(S_usb_product_string_descriptor),
  STRING_DESCRIPTOR,
  USB_PRODUCT_NAME
};


// usb_user_serial_number
const S_usb_serial_number usb_user_serial_number =
{
  sizeof(S_usb_serial_number),
  STRING_DESCRIPTOR,
  USB_SERIAL_NUMBER
};


#endif  // USB_DEVICE_FEATURE == ENABLED
