/* usb handling stuff for gps lap timer */

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>

#include <LUFA/Version.h>
#include <LUFA/Drivers/USB/USB.h>

#include "lap_timer.h"

/* global for reflecting current USB state */
uint8_t USBConnected = 0;


/* Type Defines: */
/** Type define for the device configuration descriptor structure. This must be defined in the
	*  application code, as the configuration descriptor contains several sub-descriptors which
	*  vary between devices, and which describe the device's usage to the host.
	*/
typedef struct
{
	USB_Descriptor_Configuration_Header_t Config;

	// Keyboard HID Interface
	USB_Descriptor_Interface_t            HID_Interface;
	USB_HID_Descriptor_HID_t              HID_KeyboardHID;
	USB_Descriptor_Endpoint_t             HID_ReportINEndpoint;
} USB_Descriptor_Configuration_t;

/* Macros: */
/** Endpoint number of the Keyboard HID reporting IN endpoint. */
#define KEYBOARD_EPNUM               1

/** Size in bytes of the Keyboard HID reporting IN endpoint. */
#define KEYBOARD_EPSIZE              8

/* Function Prototypes: */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
									const uint8_t wIndex,
									const void** const DescriptorAddress)
									ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);


/** HID class report descriptor. This is a special descriptor constructed with values from the
 *  USBIF HID class specification to describe the reports and capabilities of the HID device. This
 *  descriptor is parsed by the host and its contents used to determine what data (and in what encoding)
 *  the device will send, and what it may be sent back from the host. Refer to the HID specification for
 *  more details on HID report descriptors.
 */
const USB_Descriptor_HIDReport_Datatype_t PROGMEM KeyboardReport[] =
{
	/* Use the HID class driver's standard Keyboard report.
	 *   Max simultaneous keys: 6
	 */
	HID_DESCRIPTOR_KEYBOARD(6)
};

/** Device descriptor structure. This descriptor, located in FLASH memory, describes the overall
 *  device characteristics, including the supported USB version, control endpoint size and the
 *  number of device configurations. The descriptor is read out by the USB host when the enumeration
 *  process begins.
 */
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
	.Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

	.USBSpecification       = VERSION_BCD(01.10),
	.Class                  = USB_CSCP_NoDeviceClass,
	.SubClass               = USB_CSCP_NoDeviceSubclass,
	.Protocol               = USB_CSCP_NoDeviceProtocol,

	.Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

	.VendorID               = 0x03EB,
	.ProductID              = 0x2042,
	.ReleaseNumber          = VERSION_BCD(00.01),

	.ManufacturerStrIndex   = 0x01,
	.ProductStrIndex        = 0x02,
	.SerialNumStrIndex      = NO_DESCRIPTOR,

	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

/** Configuration descriptor structure. This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
	.Config =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

			.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
			.TotalInterfaces        = 1,

			.ConfigurationNumber    = 1,
			.ConfigurationStrIndex  = NO_DESCRIPTOR,

			.ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),

			.MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
		},

	.HID_Interface =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

			.InterfaceNumber        = 0x00,
			.AlternateSetting       = 0x00,

			.TotalEndpoints         = 1,

			.Class                  = HID_CSCP_HIDClass,
			.SubClass               = HID_CSCP_BootSubclass,
			.Protocol               = HID_CSCP_KeyboardBootProtocol,

			.InterfaceStrIndex      = NO_DESCRIPTOR
		},

	.HID_KeyboardHID =
		{
			.Header                 = {.Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID},

			.HIDSpec                = VERSION_BCD(01.11),
			.CountryCode            = 0x00,
			.TotalReportDescriptors = 1,
			.HIDReportType          = HID_DTYPE_Report,
			.HIDReportLength        = sizeof(KeyboardReport)
		},

	.HID_ReportINEndpoint =
		{
			.Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

			.EndpointAddress        = (ENDPOINT_DIR_IN | KEYBOARD_EPNUM),
			.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize           = KEYBOARD_EPSIZE,
			.PollingIntervalMS      = 0x01
		},
};

/** Language descriptor structure. This descriptor, located in FLASH memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
const USB_Descriptor_String_t PROGMEM LanguageString =
{
	.Header                 = {.Size = USB_STRING_LEN(1), .Type = DTYPE_String},

	.UnicodeString          = {LANGUAGE_ID_ENG}
};

/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ManufacturerString =
{
	.Header                 = {.Size = USB_STRING_LEN(3), .Type = DTYPE_String},
	.UnicodeString          = L"nes"
};

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const USB_Descriptor_String_t PROGMEM ProductString =
{
	.Header                 = {.Size = USB_STRING_LEN(13), .Type = DTYPE_String},
	.UnicodeString          = L"GPS Lap Timer"
};

/** This function is called by the library when in device mode, and must be overridden (see library "USB Descriptors"
 *  documentation) by the application code so that the address and size of a requested descriptor can be given
 *  to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 *  is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 *  USB host.
 */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void** const DescriptorAddress)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	const void* Address = NULL;
	uint16_t    Size    = NO_DESCRIPTOR;

	switch (DescriptorType)
	{
		case DTYPE_Device:
			Address = &DeviceDescriptor;
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			Address = &ConfigurationDescriptor;
			Size    = sizeof(USB_Descriptor_Configuration_t);
			break;
		case DTYPE_String:
			switch (DescriptorNumber)
			{
				case 0x00:
					Address = &LanguageString;
					Size    = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case 0x01:
					Address = &ManufacturerString;
					Size    = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case 0x02:
					Address = &ProductString;
					Size    = pgm_read_byte(&ProductString.Header.Size);
					break;
			}

			break;
		case HID_DTYPE_HID:
			Address = &ConfigurationDescriptor.HID_KeyboardHID;
			Size    = sizeof(USB_HID_Descriptor_HID_t);
			break;
		case HID_DTYPE_Report:
			Address = &KeyboardReport;
			Size    = sizeof(KeyboardReport);
			break;
	}

	*DescriptorAddress = Address;
	return Size;
}


/** Buffer to hold the previously generated Keyboard HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevKeyboardHIDReportBuffer[sizeof(USB_KeyboardReport_Data_t)];

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Keyboard_HID_Interface =
 	{
		.Config =
			{
				.InterfaceNumber              = 0,

				.ReportINEndpointNumber       = KEYBOARD_EPNUM,
				.ReportINEndpointSize         = KEYBOARD_EPSIZE,
				.ReportINEndpointDoubleBank   = false,

				.PrevReportINBuffer           = PrevKeyboardHIDReportBuffer,
				.PrevReportINBufferSize       = sizeof(PrevKeyboardHIDReportBuffer),
			},
    };


void InitUSB (void)
{
	USB_Init();
}

static void AwaitStringTransmission (void)
{
	do
	{
		HID_Device_USBTask(&Keyboard_HID_Interface);
		USB_USBTask();
	} while (*PrintBuffer != '\0' && USBConnected);
}

char *FormatTime (float Time)
{
	uint32_t IntTime = (uint32_t)Time;
	char *Buffer = PrintBuffer + strlen (PrintBuffer);
	
	sprintf_P (Buffer, PSTR("%02u%02u:%02u"),
		(unsigned)(IntTime / 3600UL),
		(unsigned)(IntTime / 60UL) % 60,
		(unsigned)(IntTime % 60UL));
	
	return Buffer + strlen (Buffer);
}

void ServiceUSB (void)
{
	uint8_t i;
	uint32_t IntTime;
	
	HID_Device_USBTask(&Keyboard_HID_Interface);
	USB_USBTask();
	
	if (!USBConnected)
		return;

	LCDClear ();
	LCDCommand (LCDAddress (0, 0));
	LCDString_P (PSTR("USB connected"));
	HID_Device_USBTask(&Keyboard_HID_Interface);
	USB_USBTask();
	LCDCommand (LCDAddress (0, 2));
	LCDString_P (PSTR("Press BLACK to"));
	HID_Device_USBTask(&Keyboard_HID_Interface);
	USB_USBTask();
	LCDCommand (LCDAddress (0, 3));
	LCDString_P (PSTR("export summary.."));

	do
	{
		do
		{
			HID_Device_USBTask(&Keyboard_HID_Interface);
			USB_USBTask();
		} while ((PIND & _BV(PD5)) && USBConnected);
		
		for (i = NoLaps; i; i--)
		{
			if (LapTimes[i-1].Time < TopSpeedTime)
				break;
		}		
		sprintf_P (PrintBuffer, PSTR("Top speed, %.2f,km/h"), (double)TopSpeed);
		AwaitStringTransmission ();
		sprintf_P (PrintBuffer, PSTR(", at, "));
		sprintf_P (FormatTime (TopSpeedTime), PSTR(", lap, %d\n"), i);
		AwaitStringTransmission ();
		
		sprintf_P (PrintBuffer, PSTR("Total distance,  %.3f,km\n"),
				   (double)(CumulativeDistance / 1000.0));
		AwaitStringTransmission ();
		
		IntTime = (uint32_t)(CumulativeRunTime * 10.0);
		sprintf_P (PrintBuffer, PSTR("Total run time,  %d:%02d.%d\n"), 
				   (int)(IntTime / 600),
				   (int)(IntTime % 600) / 10,
				   (int)(IntTime % 10));
		AwaitStringTransmission ();
		
		IntTime = (uint32_t)(CumulativeStoppedTime * 10.0);
		sprintf_P (PrintBuffer, PSTR("Total rest time, %d:%02d.%d\n"),
				   (int)(IntTime / 600),
				   (int)(IntTime % 600) / 10,
				   (int)(IntTime % 10));
		AwaitStringTransmission ();
		
		sprintf_P (PrintBuffer, PSTR("\nLAP  , SPLIT   , TIME\n"));
		AwaitStringTransmission ();
		
		IntTime = (uint32_t)(LapTimes[0].Time * 10.0);
		sprintf_P (PrintBuffer, PSTR("Start,         , "));
		sprintf_P (FormatTime (LapTimes[0].Time), PSTR("\n"));
		AwaitStringTransmission ();
		
		for (i = 1; i < NoLaps; i++)
		{
			IntTime = (uint32_t)((LapTimes[i].Time - LapTimes[i-1].Time) * 100.0);

			sprintf_P (PrintBuffer, PSTR("%5d, %2d:%02d.%02d, "),
					i,
					(int)(IntTime / 6000),
					(int)(IntTime % 6000) / 100,
					(int)(IntTime % 100));
			sprintf_P (FormatTime (LapTimes[i].Time), PSTR("\n"));
			AwaitStringTransmission ();
		}
		
		/*if (!CharaToPrint && *Ptr != '\0')
		{
			CharaToPrint = *(Ptr++);
		}*/
		
		do
		{
			HID_Device_USBTask(&Keyboard_HID_Interface);
			USB_USBTask();
		} while (!(PIND & _BV(PD5)) && USBConnected);

	} while (USBConnected);
}

void StopUSB (void)
{
	USB_Disable ();
}

#if 0
/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware()
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Hardware Initialization */
	Joystick_Init();
	LEDs_Init();
	Buttons_Init();
	USB_Init();
}
#endif

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	USBConnected = 1;
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	USBConnected = 0;
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Keyboard_HID_Interface);

	USB_Device_EnableSOFEvents();
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	HID_Device_ProcessControlRequest(&Keyboard_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&Keyboard_HID_Interface);
}

/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent)
 *
 *  \return Boolean true to force the sending of the report, false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize)
{
	USB_KeyboardReport_Data_t* KeyboardReport = (USB_KeyboardReport_Data_t*)ReportData;

	static char *Ptr = PrintBuffer;
	static uint8_t KeyPress = 0;
	
	if (*Ptr)
	{
		if (*Ptr >= 'A' && *Ptr <= 'Z')
		{
			KeyboardReport -> Modifier = HID_KEYBOARD_MODIFIER_LEFTSHIFT;
			KeyboardReport -> KeyCode[0] = HID_KEYBOARD_SC_A + *Ptr - 'A';
		}
		else if (*Ptr >= 'a' && *Ptr <= 'z')
			KeyboardReport -> KeyCode[0] = HID_KEYBOARD_SC_A + *Ptr - 'a';
		else if (*Ptr >= '1' && *Ptr <= '9')
			KeyboardReport -> KeyCode[0] = HID_KEYBOARD_SC_1_AND_EXCLAMATION + *Ptr - '1';
		else if (*Ptr == '0')
			KeyboardReport -> KeyCode[0] = HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS;
		else if (*Ptr == '.')
			KeyboardReport -> KeyCode[0] = HID_KEYBOARD_SC_DOT_AND_GREATER_THAN_SIGN;
		else if (*Ptr == ',')
			KeyboardReport -> KeyCode[0] = HID_KEYBOARD_SC_COMMA_AND_LESS_THAN_SIGN;
		else if (*Ptr == '/')
			KeyboardReport -> KeyCode[0] = HID_KEYBOARD_SC_SLASH_AND_QUESTION_MARK;
		else if (*Ptr == ':')
		{
			KeyboardReport -> Modifier = HID_KEYBOARD_MODIFIER_LEFTSHIFT;
			KeyboardReport -> KeyCode[0] = HID_KEYBOARD_SC_SEMICOLON_AND_COLON;
		}
		else if (*Ptr == ' ')
			KeyboardReport -> KeyCode[0] = HID_KEYBOARD_SC_SPACE;
		else if (*Ptr == '\n')
			KeyboardReport -> KeyCode[0] = HID_KEYBOARD_SC_ENTER;
		
		if (KeyboardReport -> KeyCode[0] == KeyPress)
		{
			/* deal with key raise between repeated characters */
			KeyPress = KeyboardReport -> KeyCode[0] = 0;
		}
		else
		{
			KeyPress = KeyboardReport -> KeyCode[0];
			Ptr++;
		}
		
		if (*Ptr == '\0')
		{
			Ptr = PrintBuffer;
			*PrintBuffer = '\0';
		}
		*ReportSize = sizeof(USB_KeyboardReport_Data_t);
		return true;
	}
	*ReportSize = sizeof(USB_KeyboardReport_Data_t);
	return false;
}

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
	uint8_t* LEDReport = (uint8_t*)ReportData;

// 	if (*LEDReport & HID_KEYBOARD_LED_NUMLOCK)
// 	  LEDMask |= LEDS_LED1;
// 
// 	if (*LEDReport & HID_KEYBOARD_LED_CAPSLOCK)
// 	  LEDMask |= LEDS_LED3;
// 
// 	if (*LEDReport & HID_KEYBOARD_LED_SCROLLLOCK)
// 	  LEDMask |= LEDS_LED4;
// 
// 	LEDs_SetAllLEDs(LEDMask);
}

