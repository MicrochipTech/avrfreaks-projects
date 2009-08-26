/* This source file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3A-1.4.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Management of the generic host HID features.
 *
 * This file manages the generic host HID features.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a USB module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

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


//_____  I N C L U D E S _______________________________________________________

#include <stddef.h>
#include "compiler.h"
#include "conf_usb.h"
#include "usb_task.h"
#include "usb_host_task.h"
#include "usb_host_enum.h"
#include "hid.h"
#include "host_hid.h"


//_____ M A C R O S ____________________________________________________________


//_____ D E F I N I T I O N S __________________________________________________

//! HID report descriptor parser control structure.
static struct
{
  size_t length;
  const hid_item_t *item;
} host_hid_report_descriptor_parser =
{
  0,
  NULL
};


//_____ D E C L A R A T I O N S ________________________________________________

/*! \name Standard Requests Applied to HID
 */
//! @{


Status_t host_hid_get_descriptor(U8 descriptor_type, U8 descriptor_index, U8 s_interface)
{
  Status_t status;
  const hid_descriptor_t *hid_descriptor = (hid_descriptor_t *)&data_stage;
  U8 i;

  usb_request.bmRequestType   = 0x81;
  usb_request.bRequest        = GET_DESCRIPTOR;
  usb_request.wValue          = descriptor_type << 8 | descriptor_index;
  usb_request.wIndex          = Get_interface_number(s_interface);
  usb_request.wLength         = SIZEOF_DATA_STAGE;
  usb_request.incomplete_read = FALSE;

  status = host_transfer_control(data_stage);

  switch (descriptor_type)
  {
  case HID_DESCRIPTOR:
    for (i = 0; i < hid_descriptor->bNumDescriptors; i++)
    {
      if (hid_descriptor->Descriptor[i].bType == HID_REPORT_DESCRIPTOR)
      {
        host_hid_report_descriptor_parser.length =
          usb_format_usb_to_mcu_data(16, hid_descriptor->Descriptor[i].wLength);
        break;
      }
    }
    break;

  case HID_REPORT_DESCRIPTOR:
    host_hid_report_descriptor_parser.item = (hid_item_t *)&data_stage;
    break;
  }

  return status;
}


Status_t host_hid_set_descriptor(U8 descriptor_type, U8 descriptor_index, U8 s_interface, U16 length)
{
  usb_request.bmRequestType   = 0x01;
  usb_request.bRequest        = SET_DESCRIPTOR;
  usb_request.wValue          = descriptor_type << 8 | descriptor_index;
  usb_request.wIndex          = Get_interface_number(s_interface);
  usb_request.wLength         = length;
  usb_request.incomplete_read = FALSE;

  return host_transfer_control(data_stage);
}


//! @}


/*! \name HID-Specific Requests
 */
//! @{


Status_t host_hid_get_report(U8 report_type, U8 report_id, U8 s_interface)
{
  usb_request.bmRequestType   = 0xA1;
  usb_request.bRequest        = HID_GET_REPORT;
  usb_request.wValue          = report_type << 8 | report_id;
  usb_request.wIndex          = Get_interface_number(s_interface);
  usb_request.wLength         = SIZEOF_DATA_STAGE;
  usb_request.incomplete_read = FALSE;

  return host_transfer_control(data_stage);
}


Status_t host_hid_set_report(U8 report_type, U8 report_id, U8 s_interface, U16 length)
{
  usb_request.bmRequestType   = 0x21;
  usb_request.bRequest        = HID_SET_REPORT;
  usb_request.wValue          = report_type << 8 | report_id;
  usb_request.wIndex          = Get_interface_number(s_interface);
  usb_request.wLength         = length;
  usb_request.incomplete_read = FALSE;

  return host_transfer_control(data_stage);
}


U8 host_hid_get_idle(U8 report_id, U8 s_interface)
{
  usb_request.bmRequestType   = 0xA1;
  usb_request.bRequest        = HID_GET_IDLE;
  usb_request.wValue          = 0x00 << 8 | report_id;
  usb_request.wIndex          = Get_interface_number(s_interface);
  usb_request.wLength         = 0x0001;
  usb_request.incomplete_read = FALSE;

  host_transfer_control(data_stage);

  return data_stage[0x00];
}


Status_t host_hid_set_idle(U8 duration_4_ms, U8 report_id, U8 s_interface)
{
  usb_request.bmRequestType   = 0x21;
  usb_request.bRequest        = HID_SET_IDLE;
  usb_request.wValue          = duration_4_ms << 8 | report_id;
  usb_request.wIndex          = Get_interface_number(s_interface);
  usb_request.wLength         = 0x0000;
  usb_request.incomplete_read = FALSE;

  return host_transfer_control(data_stage);
}


U8 host_hid_get_protocol(U8 s_interface)
{
  usb_request.bmRequestType   = 0xA1;
  usb_request.bRequest        = HID_GET_PROTOCOL;
  usb_request.wValue          = 0x0000;
  usb_request.wIndex          = Get_interface_number(s_interface);
  usb_request.wLength         = 0x0001;
  usb_request.incomplete_read = FALSE;

  host_transfer_control(data_stage);

  return data_stage[0x00];
}


Status_t host_hid_set_protocol(U8 protocol, U8 s_interface)
{
  usb_request.bmRequestType   = 0x21;
  usb_request.bRequest        = HID_SET_PROTOCOL;
  usb_request.wValue          = protocol;
  usb_request.wIndex          = Get_interface_number(s_interface);
  usb_request.wLength         = 0x0000;
  usb_request.incomplete_read = FALSE;

  return host_transfer_control(data_stage);
}


//! @}


/*! \name HID Report Descriptor Parsing Functions
 */
//! @{


Status_bool_t host_hid_get_item(host_hid_item_t *item)
{
  const hid_item_t *hid_item = host_hid_report_descriptor_parser.item;
  U8 size;

  if (host_hid_report_descriptor_parser.length <
      (U8 *)&hid_item->header - (U8 *)hid_item +
      sizeof(hid_item->header))
    return FALSE;

  item->type = hid_item->header.bType;

  if (hid_item->header.bTag == HID_ITEM_TAG_LONG_ITEM)
  {
    if (host_hid_report_descriptor_parser.length <
        (U8 *)&hid_item->long_format.bDataSize - (U8 *)hid_item +
        sizeof(hid_item->long_format.bDataSize))
      return FALSE;

    size = hid_item->long_format.bDataSize;

    if (host_hid_report_descriptor_parser.length <
        (U8 *)&hid_item->long_format.data - (U8 *)hid_item +
        size)
      return FALSE;

    item->tag = hid_item->long_format.bLongItemTag;

    item->long_format = TRUE;

    item->long_data.size = size;
    item->long_data.data = &hid_item->long_format.data;

    host_hid_report_descriptor_parser.length -=
      (U8 *)&hid_item->long_format.data - (U8 *)hid_item +
      size;

    host_hid_report_descriptor_parser.item =
      (hid_item_t *)&hid_item->long_format.data[size];
  }
  else
  {
    U8 i;

    size = (hid_item->short_format.bSize) ?
             1 << (hid_item->short_format.bSize - 1) :
             0;

    if (host_hid_report_descriptor_parser.length <
        (U8 *)&hid_item->short_format.data - (U8 *)hid_item +
        size)
      return FALSE;

    item->tag = hid_item->short_format.bTag;

    item->long_format = FALSE;

    item->short_data.value = 0x00000000;
    for (i = 0; i < size; i++)
    {
      item->short_data.value = item->short_data.value << 8 |
                               hid_item->short_format.data[i];
    }
    item->short_data.value =
      usb_format_usb_to_mcu_data(32, item->short_data.value <<
                                     ((sizeof(U32) - size) << 3));

    host_hid_report_descriptor_parser.length -=
      (U8 *)&hid_item->short_format.data - (U8 *)hid_item +
      size;

    host_hid_report_descriptor_parser.item =
      (hid_item_t *)&hid_item->short_format.data[size];
  }

  return TRUE;
}


//! @}
