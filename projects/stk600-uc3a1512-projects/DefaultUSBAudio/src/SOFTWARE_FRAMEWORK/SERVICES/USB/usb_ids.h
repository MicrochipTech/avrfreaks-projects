/* This header file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3A-1.4.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
 *
 * \brief Contains some IDs (VID, PID, Class numbers) used in the various
 *        demos.
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


#ifndef _USB_IDS_H_
#define _USB_IDS_H_


//_____ I N C L U D E S ____________________________________________________

//_____ M A C R O S ________________________________________________________

//_____ D E C L A R A T I O N S ____________________________________________

/*! \name Vendor ID (VID)
 */
//! @{
#define ATMEL_VID                             0x03EB
#define APPLE_VID                             0x05AC
//! @}

/*! \name USB Product ID (PID)
 */
//! @{
#define IPOD_PID                              0x1200
#define IPOD_SHUFFLE_PID                      0x1300
#define ENUM_EXAMPLE_PID                      0x2300
#define MS_EXAMPLE_PID                        0x2301
#define MS_SDRAM_LOADER_PID                   0x2302
#define EVK1100_CTRL_PANEL_DEMO_PID           0x2303
#define HID_EXAMPLE_PID                       0x2304
#define EVK1101_CTRL_PANEL_DEMO_HID_PID       0x2305
#define EVK1101_CTRL_PANEL_DEMO_HID_MS_PID    0x2306
#define CDC_EXAMPLE_PID                       0x2307
#define AUDIO_MIC_EXAMPLE_PID                 0x2308
#define EVK_11xx_VIRTUAL_COM_PORT_PID         0x2310
#define ISP_UC3A_PID                          0x2FF8
#define ISP_UC3B_PID                          0x2FF6
//! @}

/*! \name Generic definitions (Class, subclass and protocol)
 */
//! @{
#define NO_CLASS                              0x00
#define NO_SUBCLASS                                0x00
#define NO_PROTOCOL                                     0x00
//! @}

/*! \name Audio specific definitions (Class, subclass and protocol)
 */
//! @{
#define AUDIO_CLASS                           0x01
#define AUDIOCONTROL_SUBCLASS                      0x01
#define AUDIOSTREAMING_SUBCLASS                    0x02
#define MIDISTREAMING_SUBCLASS                     0x03
//! @}

/*! \name CDC specific definitions (Class, subclass and protocol)
 */
//! @{
#define CDC_COMM_DEVICE_CLASS                 0x02
#define CDC_COMM_CLASS                        0x02
#define CDC_COMM_DIRECT_LINE_CM_SUBCLASS           0x01
#define CDC_COMM_ABSTRACT_CM_SUBCLASS              0x02
#define CDC_COMM_TELEPHONE_CM_SUBCLASS             0x03
#define CDC_COMM_MULTICHANNEL_CM_SUBCLASS          0x04
#define CDC_COMM_CAPI_CM_SUBCLASS                  0x05
#define CDC_COMM_ETHERNET_NETWORKING_CM_SUBCLASS   0x06
#define CDC_COMM_ATM_NETWORKING_CM_SUBCLASS        0x07
#define CDC_COMM_V25ter_PROTOCOL                        0x01
#define CDC_DATA_CLASS                        0x0A
#define CDC_DATA_SUBCLASS                          0x00
#define CDC_DATA_PHYS_ISDN_BRI_PROTOCOL                 0x30
#define CDC_DATA_HDLC_PROTOCOL                          0x31
#define CDC_DATA_TRANSPARENT_PROTOCOL                   0x32
//! @}

/*! \name HID specific definitions (Class, subclass and protocol)
 */
//! @{
#define HID_CLASS                             0x03
#define BOOT_SUBCLASS                              0x01
#define KEYBOARD_PROTOCOL                               0x01
#define MOUSE_PROTOCOL                                  0x02
//! @}

/*! \name MS specific definitions (Class, subclass and protocol)
 */
//! @{
#define MS_CLASS                              0x08
#define SCSI_SUBCLASS                              0x06
#define BULK_PROTOCOL                                   0x50
//! @}

/*! \name DFU specific definitions (Class, subclass and protocol)
 */
//! @{
#define APPLICATION_CLASS                     0xFE
#define DFU_SUBCLASS                               0x01
#define RUNTIME_PROTOCOL                                0x01
#define DFU_MODE_PROTOCOL                               0x02
//! @}

/*! \name Others specific definitions (Class, subclass and protocol)
 */
//! @{
#define VENDOR_CLASS                          0xFF
//! @}

#endif  // _USB_IDS_H_
