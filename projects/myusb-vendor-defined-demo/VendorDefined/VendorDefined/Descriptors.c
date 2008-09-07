/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
 */

#include "Descriptors.h"

USB_Descriptor_HIDReport_Datatype_t HIDReport [] PROGMEM =
{
	0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
	0x09, 0x01,                    // USAGE (Vendor Usage 1)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x09, 0x01,                    //   USAGE (Vendor Usage 1)
	
	// Input report

	0x09, 0x01,                    //   USAGE (Vendor Usage 1)
	0x85, 0x01,                    //   REPORT_ID (1)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
	0x95, 0x03,                    //   REPORT_COUNT (3)
	0x75, 0x08,                    //   REPORT_SIZE (8)
	0x81, 0x82,                    //   INPUT (Data,Var,Abs,Vol)
    
	0x09, 0x01,                    //   USAGE (Vendor Usage 1)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x27, 0xff, 0xff, 0x00, 0x00,  //   LOGICAL_MAXIMUM (65535)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0x75, 0x10,                    //   REPORT_SIZE (16)
	0x81, 0x82,                    //   INPUT (Data,Var,Abs,Vol)
    
	// Output report
	
	0x09, 0x01,                    //   USAGE (Vendor Usage 1)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
	0x95, 0x03,                    //   REPORT_COUNT (3)
	0x75, 0x08,                    //   REPORT_SIZE (8)
	0x91, 0x82,                    //   OUTPUT (Data,Var,Abs,Vol)
    
	0x09, 0x01,                    //   USAGE (Vendor Usage 1)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x27, 0xff, 0xff, 0x00, 0x00,  //   LOGICAL_MAXIMUM (65535)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0x75, 0x10,                    //   REPORT_SIZE (16)
	0x91, 0x82,                    //   OUTPUT (Data,Var,Abs,Vol)
	
	// Feature report
	
	0x09, 0x01,                    //   USAGE (Vendor Usage 1)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0x75, 0x08,                    //   REPORT_SIZE (8)
	0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
	0x09, 0x01,                    // USAGE (Vendor Usage 1)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x27, 0xff, 0xff, 0x00, 0x00,  //   LOGICAL_MAXIMUM (65535)
	0x95, 0x02,                    // REPORT_COUNT (2)
	0x75, 0x10,                    // REPORT_SIZE (16)
	0xb1, 0x02,                    // FEATURE (Data,Var,Abs)
	
	0xc0                           // END_COLLECTION
};

USB_Descriptor_Device_t DeviceDescriptor PROGMEM =
{
	Header:                 {Size: sizeof(USB_Descriptor_Device_t), Type: DTYPE_Device},
		
	USBSpecification:       VERSION_BCD(01.10),
	Class:                  0x00,
	SubClass:               0x00,
	Protocol:               0x00,
				
	Endpoint0Size:          8,
		
	VendorID:               0x0000,
	ProductID:              USB_PRODUCT_ID('A', 'P'),
	ReleaseNumber:          0x0000,
		
	ManufacturerStrIndex:   0x01,
	ProductStrIndex:        0x02,
	SerialNumStrIndex:      0x03,
		
	NumberOfConfigurations: 1
};
	
USB_Descriptor_Configuration_t ConfigurationDescriptor PROGMEM =
{
	Config:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Configuration_Header_t), Type: DTYPE_Configuration},

			TotalConfigurationSize: sizeof(USB_Descriptor_Configuration_t),
			TotalInterfaces:        1,
				
			ConfigurationNumber:    1,
			ConfigurationStrIndex:  NO_DESCRIPTOR_STRING,
				
			ConfigAttributes:       (USB_CONFIG_ATTR_BUSPOWERED | USB_CONFIG_ATTR_SELFPOWERED),
			
			MaxPowerConsumption:    USB_CONFIG_POWER_MA(100)
		},
		
	Interface:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Interface_t), Type: DTYPE_Interface},

			InterfaceNumber:        HID_INTERFACE_NUMBER,
			AlternateSetting:       HID_INTERFACE_ALTERNATE,
			
			TotalEndpoints:         HID_INTERFACE_ENDPOINTS,
				
			Class:                  HID_INTERFACE_CLASS,
			SubClass:               HID_INTERFACE_SUBCLASS,
			Protocol:               HID_INTERFACE_PROTOCOL,
				
			InterfaceStrIndex:      NO_DESCRIPTOR_STRING
		},

	HIDHID:
		{
			Header:                 {Size: sizeof(USB_Descriptor_HID_t), Type: DTYPE_HID},
									 
			HIDSpec:          		VERSION_BCD(01.10),
			CountryCode:      		0x00,
			TotalHIDDescriptors:    0x01,
			HIDReportType:    		0x22,
			HIDReportLength:        sizeof(HIDReport)
		},

	HIDEndpointIn:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Endpoint_t), Type: DTYPE_Endpoint},
										 
			EndpointAddress:        (ENDPOINT_DESCRIPTOR_DIR_IN | HID_EPNUM_IN),
			Attributes:       		EP_TYPE_INTERRUPT,
			EndpointSize:           HID_EPSIZE,
			PollingIntervalMS:		0x02
		},	

#ifndef OUTPUT_ON_CONTROL
	HIDEndpointOut:
		{
			Header:                 {Size: sizeof(USB_Descriptor_Endpoint_t), Type: DTYPE_Endpoint},
										 
			EndpointAddress:        (ENDPOINT_DESCRIPTOR_DIR_OUT | HID_EPNUM_OUT),
			Attributes:       		EP_TYPE_INTERRUPT,
			EndpointSize:           HID_EPSIZE,
			PollingIntervalMS:		0x02
		}	
#endif
};

USB_Descriptor_String_t LanguageString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(1), Type: DTYPE_String},
		
	UnicodeString:          {LANGUAGE_ID_ENG}
};

USB_Descriptor_String_t ManufacturerString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(18), Type: DTYPE_String},
		
	UnicodeString:          L"Phill Harvey-Smith"
};

USB_Descriptor_String_t ProductString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(18), Type: DTYPE_String},
		
	UnicodeString:          L"MyUSB AVRPort Demo"
};

USB_Descriptor_String_t SerialNumberString PROGMEM =
{
	Header:                 {Size: USB_STRING_LEN(12), Type: DTYPE_String},
		
	UnicodeString:          L"000000000000"
};

//USB_Descriptor_String_t SerialNumberString PROGMEM =
//{
//	Header:                 {Size: USB_STRING_LEN(12), Type: DTYPE_String},
//		
//	UnicodeString:          L"000000000000"				
//};

bool USB_GetDescriptor(const uint16_t wValue, const uint8_t wIndex,
                       void** const DescriptorAddress, uint16_t* const DescriptorSize)
{
	void*    Address = NULL;
	uint16_t Size    = 0;

	switch (wValue >> 8)
	{
		case DTYPE_Device:
			Address = DESCRIPTOR_ADDRESS(DeviceDescriptor);
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			Address = DESCRIPTOR_ADDRESS(ConfigurationDescriptor);
			Size    = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String:
			switch (wValue & 0xFF)
			{
				case 0x00:
					Address = DESCRIPTOR_ADDRESS(LanguageString);
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case 0x01:
					Address = DESCRIPTOR_ADDRESS(ManufacturerString);
					Size    = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case 0x02:
					Address = DESCRIPTOR_ADDRESS(ProductString);
					Size    = pgm_read_byte(&ProductString.Header.Size);
					break;
				case 0x03:
					Address = DESCRIPTOR_ADDRESS(SerialNumberString);
					Size    = pgm_read_byte(&SerialNumberString.Header.Size);
					break;
			}
			
			break;
		case DTYPE_HID:
			Address = DESCRIPTOR_ADDRESS(ConfigurationDescriptor.HIDHID);
			Size    = sizeof(USB_Descriptor_HID_t);
			break;
		case DTYPE_Report:
			Address = DESCRIPTOR_ADDRESS(HIDReport);
			Size    = sizeof(HIDReport);
			break;
	}
	
	if (Address != NULL)
	{
		*DescriptorAddress = Address;
		*DescriptorSize    = Size;

		return true;
	}
		
	return false;
}

