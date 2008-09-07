/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

	/* Includes: */
		#include <MyUSB/Drivers/USB/USB.h>

		#include <avr/pgmspace.h>

	/* Type Defines: */
		typedef struct
		{
			USB_Descriptor_Header_t               Header;
				
			uint16_t                              HIDSpec;
			uint8_t                               CountryCode;
		
			uint8_t                               TotalHIDDescriptors;

			uint8_t                               HIDReportType;
			uint16_t                              HIDReportLength;
		} USB_Descriptor_HID_t;
		
		typedef uint8_t USB_Descriptor_HIDReport_Datatype_t;

		typedef struct
		{
			USB_Descriptor_Configuration_Header_t Config;
			USB_Descriptor_Interface_t            Interface;
			USB_Descriptor_HID_t                  HIDHID;
	        USB_Descriptor_Endpoint_t             HIDEndpointIn;
		#ifndef OUTPUT_ON_CONTROL
			USB_Descriptor_Endpoint_t             HIDEndpointOut;
		#endif
		} USB_Descriptor_Configuration_t;
					
	/* Macros: */
		#define HID_EPNUM_IN            1
		#define HID_EPNUM_OUT			2
		#define HID_EPSIZE              16
		

		#define DTYPE_HID                    0x21
		#define DTYPE_Report                 0x22

		#ifdef OUTPUT_ON_CONTROL
		#define HID_INTERFACE_ENDPOINTS 1
		#else
		#define HID_INTERFACE_ENDPOINTS 2
		#endif

		#define HID_INTERFACE_NUMBER    0
		#define HID_INTERFACE_ALTERNATE 0
				
		#define HID_INTERFACE_CLASS     0x03
		#define HID_INTERFACE_SUBCLASS  0x00
		#define HID_INTERFACE_PROTOCOL  0x00
		
		#define HID_REPORT_PORT_IN		0x01
		#define HID_REPORT_PORT_OUT		0x01
		#define HID_REPORT_EE_WRITE		0x01

		#define USB_PRODUCT_ID(x, y)              (((uint16_t)x << 8) | y)
#endif
