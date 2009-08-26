/* This header file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3A-1.4.0 Release */

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


#ifndef _USB_DESCRIPTORS_H_
#define _USB_DESCRIPTORS_H_


//_____ I N C L U D E S ____________________________________________________

#include "conf_usb.h"

#if USB_DEVICE_FEATURE == DISABLED
  #error usb_descriptors.h is #included although USB_DEVICE_FEATURE is disabled
#endif


#include "usb_standard_request.h"
#include "usb_task.h"
#include "usb_audio.h"


//_____ M A C R O S ________________________________________________________

#define Usb_unicode(c)                    (Usb_format_mcu_to_usb_data(16, (U16)(c)))
#define Usb_get_dev_desc_pointer()        (&(usb_dev_desc.bLength))
#define Usb_get_dev_desc_length()         (sizeof(usb_dev_desc))
#define Usb_get_conf_desc_pointer()       (&(usb_conf_desc.cfg.bLength))
#define Usb_get_conf_desc_length()        (sizeof(usb_conf_desc))


//_____ U S B    D E F I N E S _____________________________________________

            // USB Device descriptor
#define USB_SPECIFICATION     0x0200
#define DEVICE_CLASS          0                   //! Each configuration has its own class
#define DEVICE_SUB_CLASS      0                   //! Each configuration has its own subclass
#define DEVICE_PROTOCOL       0                   //! Each configuration has its own protocol
#define EP_CONTROL_LENGTH     64
#define VENDOR_ID             ATMEL_VID           //! Atmel vendor ID
#define PRODUCT_ID            AUDIO_MIC_EXAMPLE_PID
#define RELEASE_NUMBER        0x1000
#define MAN_INDEX             0x01
#define PROD_INDEX            0x02
#define SN_INDEX              0x03
#define NB_CONFIGURATION      1


            // USB mic configuration descriptor
#define NB_INTERFACE       2     //! The number of interfaces for this configuration
#define CONF_NB            1     //! Number of this configuration
#define CONF_INDEX         0
#define CONF_ATTRIBUTES    USB_CONFIG_BUSPOWERED
#define MAX_POWER          50    // 100 mA

             // Standard Audio Control (AC) interface descriptor
#define INTERFACE_NB        0
#define ALTERNATE           0
#define NB_ENDPOINT         0			     //! No endpoint for AC interface
#define INTERFACE_CLASS     AUDIO_CLASS  //! Audio Class
#define INTERFACE_SUB_CLASS 0x01		     //! Audio_control sub class
#define INTERFACE_PROTOCOL  0x00		     //! Unused
#define INTERFACE_INDEX     0

             // USB Endpoint 1 descriptor FS
#define ENDPOINT_NB_1       ( EP_AUDIO_IN | MSK_EP_DIR )
#define EP_ATTRIBUTES_1     TYPE_ISOCHRONOUS
#define EP_IN_LENGTH_1      16
#define EP_SIZE_1           EP_IN_LENGTH_1
#define EP_INTERVAL_1       0x01 				 // One packet per frame

				// AC interface descriptor Audio specific
#define AUDIO_CLASS_REVISION				0x0100
#define NB_OF_STREAMING_INTERFACE		0x01
#define BELONGS_AUDIO_INTERFACE			0x01
				// Input Terminal descriptor
#define INPUT_TERMINAL_ID					0x01
#define INPUT_TERMINAL_TYPE				0x0201 // Terminal is microphone
#define INPUT_TERMINAL_ASSOCIATION		0x00   // No association
#define INPUT_TERMINAL_NB_CHANNELS		0x01   // One channel for input terminal
#define INPUT_TERMINAL_CHANNEL_CONF		0x0000 // Mono sets no position
#define INPUT_TERMINAL_CH_NAME_ID		0x00	 // No channel name
            //MIC Feature Unit descriptor
#define MIC_FEATURE_UNIT_ID            0x02
#define MIC_FEATURE_UNIT_SOURCE_ID     0x01
#define MIC_BMA_CONTROLS               0x0003 // Mute + Volume
//#define MIC_BMA_CONTROLS               0x0001 // Mute Only
				// Output Terminal descriptor
#define OUTPUT_TERMINAL_ID					0x03
#define OUTPUT_TERMINAL_TYPE				0x0101 // USB Streaming
#define OUTPUT_TERMINAL_ASSOCIATION		0x00   // No association
#define OUTPUT_TERMINAL_SOURCE_ID		0x02	 // From Feature Unit Terminal
				//Audio Streaming (AS) interface descriptor
#define STD_AS_INTERFACE_NB				0x01   // Index of Std AS Interface
				//Alternate O Audio Streaming (AS) interface descriptor
#define ALT0_AS_INTERFACE_INDEX			0x00   // Index of Std AS interface Alt0
#define ALT0_AS_NB_ENDPOINT				0x00   // Nb of endpoints for alt0 interface
#define ALT0_AS_INTERFACE_CLASS			0x01   // Audio class
#define ALT0_AS_INTERFACE_SUB_CLASS 	0x02   // Audio streamn sub class
#define ALT0_AS_INTERFACE_PROTOCOL		0x00   // Unused
				//Alternate 1 Audio Streaming (AS) interface descriptor
#define ALT1_AS_INTERFACE_INDEX			0x01   // Index of Std AS interface Alt0
#define ALT1_AS_NB_ENDPOINT				0x01   // Nb of endpoints for alt1 interface
#define ALT1_AS_INTERFACE_CLASS			0x01   // Audio class
#define ALT1_AS_INTERFACE_SUB_CLASS 	0x02   // Audio streamn sub class
#define ALT1_AS_INTERFACE_PROTOCOL		0x00   // Unused
				//AS general Interface descriptor
#define AS_TERMINAL_LINK					0x03   // Unit Id of the output terminal
#define AS_DELAY								0x01   // Interface delay
#define AS_FORMAT_TAG						0x0001 // PCM Format
				// Format type
#define FORMAT_TYPE							0x01	 // Format TypeI
#define FORMAT_NB_CHANNELS					0x01	 // One Channel
#define FORMAT_FRAME_SIZE					0x02	 // Two bytes per audio sample
#define FORMAT_BIT_RESOLUTION				0x10	 // 16 bits per sample
#define FORMAT_SAMPLE_FREQ_NB				0x01	 // One frequency supported
#define FORMAT_LSBYTE_SAMPLE_FREQ		0x1F40 // 0x001F40=8kHz Sample frequency
#define FORMAT_MSBYTE_SAMPLE_FREQ		0x00	 // MsByte of 0x001F40

				//Audio endpoint specific descriptor field
#define AUDIO_EP_ATRIBUTES					0x00	 // No sampling freq, no pitch, no pading
#define AUDIO_EP_DELAY_UNIT				0x00	 // Unused
#define AUDIO_EP_LOCK_DELAY				0x0000	// Unused

#define LANG_ID               0x00
#define DEVICE_STATUS         BUS_POWERED
#define INTERFACE_STATUS      0x00 // TBD






#define USB_MN_LENGTH         5
#define USB_MANUFACTURER_NAME \
{\
  Usb_unicode('A'),\
  Usb_unicode('T'),\
  Usb_unicode('M'),\
  Usb_unicode('E'),\
  Usb_unicode('L') \
}

#define USB_PN_LENGTH         20
#define USB_PRODUCT_NAME \
{\
  Usb_unicode('A')\
, Usb_unicode('V')\
, Usb_unicode('R')\
, Usb_unicode('3')\
, Usb_unicode('2')\
, Usb_unicode(' ')\
, Usb_unicode('U')\
, Usb_unicode('C')\
, Usb_unicode('3')\
, Usb_unicode(' ')\
, Usb_unicode('A') \
, Usb_unicode('U') \
, Usb_unicode('D') \
, Usb_unicode('I') \
, Usb_unicode('O') \
, Usb_unicode(' ') \
, Usb_unicode('D') \
, Usb_unicode('E') \
, Usb_unicode('M') \
, Usb_unicode('O') \
}

#define USB_SN_LENGTH         13
#define USB_SERIAL_NUMBER \
{\
  Usb_unicode('1'),\
  Usb_unicode('.'),\
  Usb_unicode('0'),\
  Usb_unicode('.'),\
  Usb_unicode('0'),\
  Usb_unicode('.'),\
  Usb_unicode('0'),\
  Usb_unicode('.'),\
  Usb_unicode('0'),\
  Usb_unicode('.'),\
  Usb_unicode('0'),\
  Usb_unicode('.'),\
  Usb_unicode('A') \
}

#define LANGUAGE_ID           0x0409


//! USB Request
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
  U8      bmRequestType;        //!< Characteristics of the request
  U8      bRequest;             //!< Specific request
  U16     wValue;               //!< Field that varies according to request
  U16     wIndex;               //!< Field that varies according to request
  U16     wLength;              //!< Number of bytes to transfer if Data
}
#if __ICCAVR32__
#pragma pack()
#endif
S_UsbRequest;


//! USB Device Descriptor
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
  U8      bLength;              //!< Size of this descriptor in bytes
  U8      bDescriptorType;      //!< DEVICE descriptor type
  U16     bscUSB;               //!< Binay Coded Decimal Spec. release
  U8      bDeviceClass;         //!< Class code assigned by the USB
  U8      bDeviceSubClass;      //!< Subclass code assigned by the USB
  U8      bDeviceProtocol;      //!< Protocol code assigned by the USB
  U8      bMaxPacketSize0;      //!< Max packet size for EP0
  U16     idVendor;             //!< Vendor ID. ATMEL = 0x03EB
  U16     idProduct;            //!< Product ID assigned by the manufacturer
  U16     bcdDevice;            //!< Device release number
  U8      iManufacturer;        //!< Index of manu. string descriptor
  U8      iProduct;             //!< Index of prod. string descriptor
  U8      iSerialNumber;        //!< Index of S.N.  string descriptor
  U8      bNumConfigurations;   //!< Number of possible configurations
}
#if __ICCAVR32__
#pragma pack()
#endif
S_usb_device_descriptor;


//! USB Configuration Descriptor
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
  U8      bLength;              //!< Size of this descriptor in bytes
  U8      bDescriptorType;      //!< CONFIGURATION descriptor type
  U16     wTotalLength;         //!< Total length of data returned
  U8      bNumInterfaces;       //!< Number of interfaces for this conf.
  U8      bConfigurationValue;  //!< Value for SetConfiguration resquest
  U8      iConfiguration;       //!< Index of string descriptor
  U8      bmAttributes;         //!< Configuration characteristics
  U8      MaxPower;             //!< Maximum power consumption
}
#if __ICCAVR32__
#pragma pack()
#endif
S_usb_configuration_descriptor;


//! USB Interface Descriptor
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
  U8      bLength;              //!< Size of this descriptor in bytes
  U8      bDescriptorType;      //!< INTERFACE descriptor type
  U8      bInterfaceNumber;     //!< Number of interface
  U8      bAlternateSetting;    //!< Value to select alternate setting
  U8      bNumEndpoints;        //!< Number of EP except EP 0
  U8      bInterfaceClass;      //!< Class code assigned by the USB
  U8      bInterfaceSubClass;   //!< Subclass code assigned by the USB
  U8      bInterfaceProtocol;   //!< Protocol code assigned by the USB
  U8      iInterface;           //!< Index of string descriptor
}
#if __ICCAVR32__
#pragma pack()
#endif
S_usb_interface_descriptor;


//! A U D I O Specific
//! Audio AC interface descriptor
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
  U8  bLength;               /* Size of this descriptor in bytes */
  U8  bDescriptorType;       /* CS interface*/
  U8 	bDescritorSubtype;     /* HEADER Subtype */
  U16 bcdADC;          		  /* Revision of class spec */
  U16 wTotalLength;       	  /* Total size of class specific descriptor */
  U8  bInCollection;         /* Number of streaming interface */
  U8  baInterfaceNr;		     /* Total length of Report descriptor */
} S_usb_ac_interface_descriptor;


//! USB mic INPUT Terminal Descriptor
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
	U8		bLenght;
	U8 	bDescriptorType;
	U8 	bDescriptorSubType;
	U8		bTerminalID;
	U16	wTerminalType;
	U8		bAssocTerminal;
	U8		bNrChannels;
	U16	wChannelConfig;
	U8		iChannelNames;
	U8		iTerminal;
} S_usb_mic_in_ter_descriptor;


//! USB Audio Feature Unit descriptor
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
	U8		bLenght;
	U8 	bDescriptorType;
	U8 	bDescriptorSubType;
	U8		bUnitID;
	U8    bSourceID;
	U8		bControSize;
	U16	bmaControls;
} S_usb_feature_unit_descriptor;


//! USB mic OUTPUT Terminal Descriptor
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
	U8		bLenght;
	U8 	bDescriptorType;
	U8 	bDescriptorSubType;
	U8		bTerminalID;
	U16	wTerminalType;
	U8		bAssocTerminal;
	U8		bSourceID;
	U8		iTerminal;
} S_usb_mic_out_ter_descriptor;


//! USB Standard AS interface Descriptor
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
	U8		bLenght;
	U8 	bDescriptorType;
	U8		bInterfaceNumber;
	U8		bAlternateSetting;
	U8		bNumEndpoints;
	U8		bInterfaceClass;
	U8		bInterfaceSubclass;
	U8		bInterfaceProtocol;
	U8		iInterface;
} S_usb_as_interface_descriptor;


//! USB AS general interface descriptor
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
	U8		bLenght;
	U8 	bDescriptorType;
	U8 	bDescriptorSubType;
	U8		bTerminalLink;
	U8    bDelay;
	U16	wFormatTag;
} S_usb_as_g_interface_descriptor;


//! Audio Format Type descriptor
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
	U8		bLenght;
	U8 	bDescriptorType;
	U8 	bDescriptorSubType;
	U8		bFormatType;
	U8		bNrChannels;
	U8		bSubFrameSize;
	U8		bBitResolution;
	U8		bSampleFreqType;
	U16	wLsbyteiSamFreq;
	U8		bMsbyteiSamFreq;
} S_usb_format_type;


//! Endpoint AUDIO Specific descriptor
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
	U8		bLenght;
	U8 	bDescriptorType;
	U8 	bDescriptorSubType;	
	U8		bmAttributes;
	U8    bLockDelayUnits;
	U16	wLockDelay;
}S_usb_endpoint_audio_specific;


//! Usb Audio Endpoint Descriptor
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
   U8      bLength;               //!< Size of this descriptor in bytes
   U8      bDescriptorType;       //!< ENDPOINT descriptor type
   U8      bEndpointAddress;      //!< Address of the endpoint
   U8      bmAttributes;          //!< Endpoint's attributes
   U16     wMaxPacketSize;        //!< Maximum packet size for this EP
   U8      bInterval;             //!< Interval for polling EP in ms
	U8		  bRefresh;
	U8		  bSynAddress;
} S_usb_endpoint_audio_descriptor;


//! USB Endpoint Descriptor
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
  U8      bLength;              //!< Size of this descriptor in bytes
  U8      bDescriptorType;      //!< ENDPOINT descriptor type
  U8      bEndpointAddress;     //!< Address of the endpoint
  U8      bmAttributes;         //!< Endpoint's attributes
  U16     wMaxPacketSize;       //!< Maximum packet size for this EP
  U8      bInterval;            //!< Interval for polling EP in ms
}
#if __ICCAVR32__
#pragma pack()
#endif
S_usb_endpoint_descriptor;


//! USB Device Qualifier Descriptor
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
  U8      bLength;              //!< Size of this descriptor in bytes
  U8      bDescriptorType;      //!< Device Qualifier descriptor type
  U16     bscUSB;               //!< Binay Coded Decimal Spec. release
  U8      bDeviceClass;         //!< Class code assigned by the USB
  U8      bDeviceSubClass;      //!< Subclass code assigned by the USB
  U8      bDeviceProtocol;      //!< Protocol code assigned by the USB
  U8      bMaxPacketSize0;      //!< Max packet size for EP0
  U8      bNumConfigurations;   //!< Number of possible configurations
  U8      bReserved;            //!< Reserved for future use, must be zero
}
#if __ICCAVR32__
#pragma pack()
#endif
S_usb_device_qualifier_descriptor;


//! USB Language Descriptor
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
  U8      bLength;              //!< Size of this descriptor in bytes
  U8      bDescriptorType;      //!< STRING descriptor type
  U16     wlangid;              //!< Language id
}
#if __ICCAVR32__
#pragma pack()
#endif
S_usb_language_id;


//_____ U S B   M A N U F A C T U R E R   D E S C R I P T O R _______________

//! struct usb_st_manufacturer
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
  U8  bLength;                  //!< Size of this descriptor in bytes
  U8  bDescriptorType;          //!< STRING descriptor type
  U16 wstring[USB_MN_LENGTH];   //!< Unicode characters
}
#if __ICCAVR32__
#pragma pack()
#endif
S_usb_manufacturer_string_descriptor;


//_____ U S B   P R O D U C T   D E S C R I P T O R _________________________

//! struct usb_st_product
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
  U8  bLength;                  //!< Size of this descriptor in bytes
  U8  bDescriptorType;          //!< STRING descriptor type
  U16 wstring[USB_PN_LENGTH];   //!< Unicode characters
}
#if __ICCAVR32__
#pragma pack()
#endif
S_usb_product_string_descriptor;


//_____ U S B   S E R I A L   N U M B E R   D E S C R I P T O R _____________

//! struct usb_st_serial_number
typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
  U8  bLength;                  //!< Size of this descriptor in bytes
  U8  bDescriptorType;          //!< STRING descriptor type
  U16 wstring[USB_SN_LENGTH];   //!< Unicode characters
}
#if __ICCAVR32__
#pragma pack()
#endif
S_usb_serial_number;


//_____ U S B   D E V I C E   A U D I O   D E S C R I P T O R ___________________

typedef
#if __ICCAVR32__
#pragma pack(1)
#endif
struct
#if __GNUC__
__attribute__((__packed__))
#endif
{
   S_usb_configuration_descriptor 	cfg;
   S_usb_interface_descriptor     	ifc;
	S_usb_ac_interface_descriptor  	audioac;
	S_usb_mic_in_ter_descriptor	 	mic_in_ter;
   S_usb_feature_unit_descriptor    mic_fea_unit; // extra
	S_usb_mic_out_ter_descriptor	 	mic_out_ter;
	S_usb_as_interface_descriptor	 	mic_as_alt0;	
	S_usb_as_interface_descriptor	 	mic_as_alt1;	
	S_usb_as_g_interface_descriptor	mic_g_as;
	S_usb_format_type						mic_format_type;
   S_usb_endpoint_audio_descriptor  ep1;
	S_usb_endpoint_audio_specific	 	ep1_s;
}
#if __ICCAVR32__
#pragma pack()
#endif
S_usb_user_configuration_descriptor;


#endif  // _USB_DESCRIPTORS_H_
