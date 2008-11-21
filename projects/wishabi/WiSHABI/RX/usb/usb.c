#include "usb.h"

/////////////////////////////////////////////////////////////////////////////////////////
//Defines usbHidReportDescriptor - the form of input buffer array to be sent to
//udbSetInterrupt.  The array will take the following form:
/////////////////////////////////////////////////////////////////////////////////////////
//For Keyboard Buffer (uC to PC)/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
//[ 1 , B1 , B2 , B3 , B4 , B5 , B6 , B7]
//  |   |    |_________________________|
//  |   |                 |
//  |   |                 |____Keyboard buttons (0x00 to 0x65)
//  |   |______________________Modifier Condition Where Bit0:Left Ctrl
//  |                                              Bit1:Left Shift
//  |                                              Bit2:Left Alt
//  |                                              Bit3:Left GUI (Windows Key)
//  |                                              Bit4:Right Ctrl
//  |                                              Bit5:Right Shift
//  |                                              Bit6:Right Alt
//  |                                              Bit7:Right GUI (Windows Key)
//  |__________________________Report ID = 1 for keyboard (2 for mouse)
/////////////////////////////////////////////////////////////////////////////////////////
//For Keyboard Buffer (PC to uC)/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
//[ 1 , B1]
//  |   |
//  |   |
//  |   |
//  |   |______________________PC LED Conditions Where Bit0:Num Lock LED
//  |                                                  Bit1:Caps Lock LED
//  |                                                  Bit2:Scroll Lock LED
//  |                                                  Bit3:Compose LED
//  |                                                  Bit4:Kana LED
//  |                                                  Bit5:N/A
//  |                                                  Bit6:N/A
//  |                                                  Bit7:N/A
//  |__________________________Report ID = 1 for keyboard (2 for mouse)
/////////////////////////////////////////////////////////////////////////////////////////
//For Mouse Buffer///////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
//[ 2 , B1 , B2 , B3 ]
//  |   |    |    |
//  |   |    |    |_______Cursor movement along the Y-Axis (-127 to 127)
//  |   |    |____________Cursor movement along the X-Axis (-127 to 127)
//  |   |_________________Mouse Button Conditions where Bit0:Button One (Left Click)
//  |                                                   Bit1:Button Two (Right Click)
//  |                                                   Bit2:Button Three (Middle Click)
//  |                                                   Bit3:N/A
//  |                                                   Bit4:N/A
//  |                                                   Bit5:N/A
//  |                                                   Bit6:N/A
//  |                                                   Bit7:N/A
//  |_____________________Report ID = 2 for mouse (1 for keyboard)
/////////////////////////////////////////////////////////////////////////////////////////


char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] PROGMEM =
{
// Keyboard
//	-------------------------------------------------------------------------
	0x05, 0x01,//	USAGE_PAGE (Generic Desktop)							|
	0x09, 0x06,//	USAGE (Keyboard)										|
	0xa1, 0x01,//	COLLECTION (Application)								|
//	-------------------------------------------------------------------------
	0x85, 0x01,//		Report Id (1)										|Byte 0 - Report ID = 1 for Keyboard
//	-------------------------------------------------------------------------
	0x05, 0x07,//		USAGE_PAGE (Keyboard)								|
	0x19, 0xe0,//		USAGE_MINIMUM (Keyboard LeftControl)				|Defines byte 1 form when byte
	0x29, 0xe7,//		USAGE_MAXIMUM (Keyboard Right GUI)					|0 is 1 (keyboard) for data reception.
	0x15, 0x00,//		LOGICAL_MINIMUM (0)									|Represents the state of each of the 8
	0x25, 0x01,//		LOGICAL_MAXIMUM (1)									|modifiers (ctrls, alts,
	0x75, 0x01,//		REPORT_SIZE (1)										|shifts, and guis)
	0x95, 0x08,//		REPORT_COUNT (8)									|
	0x81, 0x02,//		INPUT (Data,Var,Abs)								|
//	-------------------------------------------------------------------------
	0x95, 0x05,//		REPORT_COUNT (5)									|
	0x75, 0x01,//		REPORT_SIZE (1)										|Defines byte 1 form when byte
	0x05, 0x08,//		USAGE_PAGE (LEDs)									|0 is 1 (keyboard) for data transmission.
	0x19, 0x01,//		USAGE_MINIMUM (Num Lock)							|Represents the status of the PC's LEDs
	0x29, 0x05,//		USAGE_MAXIMUM (Kana)								|(Caps Lock, Num Lock etc).  5-bits.
	0x91, 0x02,//		OUTPUT (Data,Var,Abs)								|
//	-------------------------------------------------------------------------
	0x95, 0x01,//		REPORT_COUNT (1)									|
	0x75, 0x03,//		REPORT_SIZE (3)										|3-bits to pad previous section to a byte.
	0x91, 0x03,//		OUTPUT (Cnst,Var,Abs)								|
//	-------------------------------------------------------------------------
	0x95, 0x06,//		REPORT_COUNT (6)									|
	0x75, 0x08,//		REPORT_SIZE (8)										|Defines bytes 2-7 form when byte
	0x15, 0x00,//		LOGICAL_MINIMUM (0)									|0 is 1 (keyboard). Represents
	0x25, 0x65,//		LOGICAL_MAXIMUM (101)								|any of the regular keyboard
	0x05, 0x07,//		USAGE_PAGE (Keyboard)								|characters
	0x19, 0x00,//		USAGE_MINIMUM (Reserved (no event indicated))		|
	0x29, 0x65,//		USAGE_MAXIMUM (Keyboard Application)				|
	0x81, 0x00,//		INPUT (Data,Ary,Abs)								|
	0xc0,//			END_COLLECTION  										|
//	-------------------------------------------------------------------------

// Mouse 
//	-------------------------------------------------------------------------
	0x05, 0x01,//	Usage Page (Generic Desktop),							|
	0x09, 0x02,//	Usage (Mouse),											|Defines byte 1 form when byte
	0xA1, 0x01,//	Collection (Application),								|0 is 2 (mouse). Represents
	0x09, 0x01,//		Usage (Pointer)										|the condition of three mouse
	0xA1, 0x00,//		Collection (Physical),								|buttons (thus only 3 bits,
	0x85, 0x02,//			Report Id (2).									|
	0x05, 0x09,//			Usage Page (Buttons),							|remaining 5 bits padded as
	0x19, 0x01,//			Usage Minimum (01),								|constants by the second
	0x29, 0x03,//			Usage Maximun (03),								|"Input" line.
	0x15, 0x00,//			Logical Minimum (0),							|
	0x25, 0x01,//			Logical Maximum (1),							|
	0x95, 0x03,//			Report Count (3),								|
	0x75, 0x01,//			Report Size (1),								|
	0x81, 0x02,//			Input (Data, Variable, Absolute), ;3 button bits|
	0x95, 0x01,//			Report Count (1),								|
	0x75, 0x05,//			Report Size (5),								|
	0x81, 0x01,//			Input (Constant), ;5 bit padding				|
//	-------------------------------------------------------------------------
	0x05, 0x01,//			Usage Page (Generic Desktop),					|
	0x09, 0x30,//			Usage (X),										|Defines form of bytes 2
	0x09, 0x31,//			Usage (Y),										|and 3 when byte 0 is 2
	0x15, 0x81,//			Logical Minimum (-127),							|(mouse). Represents the motion
	0x25, 0x7F,//			Logical Maximum (127),							|increment of the cursor in the
	0x75, 0x08,//			Report Size (8),								|x-axis (byte 2) or y axis
	0x95, 0x02,//			Report Count (2),								|(byte 3).
	0x81, 0x06,//			Input (Data, Var, Rel),2 position bytes (X & Y)	|
//	-------------------------------------------------------------------------
	0xC0,//				End Collection,										|
	0xC0,//			End Collection											|
//	-------------------------------------------------------------------------
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Definitions for the following in "usb.h".
uchar inputBuffer1[8] = { 1, 0,0,0,0,0,0,0, };
uchar inputBuffer2[4] = { 2, 0,0,0, };
static uchar idleRate = 0;
static uchar protocolVer=1;
uchar tx_kbd = 0;
uchar tx_mouse = 0;
uchar expectReport=0;
uchar LEDstate=0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////
uchar usbFunctionSetup(uchar data[8])
{
	usbRequest_t *rq = (void *)data;
	usbMsgPtr = inputBuffer1;
	if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS)// class request type 
	{
		if(rq->bRequest == USBRQ_HID_GET_REPORT)
		{
			if (rq->wValue.bytes[0] == 1)
			{
				usbMsgPtr = inputBuffer1;
				return sizeof(inputBuffer1);
			}
			else if (rq->wValue.bytes[0] == 2)
			{
				usbMsgPtr = inputBuffer2;
				return sizeof(inputBuffer2);
			}
			return 0;
		}
		else if(rq->bRequest == USBRQ_HID_SET_REPORT)
		{
			if (rq->wLength.word == 2)// We expect two byte reports.  B1: Report ID, B2: Status of Keyboard LEDs
			{
				expectReport=1;
				return 0xFF; // Call usbFunctionWrite with data 
			}
		}
		else if(rq->bRequest == USBRQ_HID_GET_IDLE)
		{
			usbMsgPtr = &idleRate;
			return 1;
		}
		else if(rq->bRequest == USBRQ_HID_SET_IDLE)
		{
			idleRate = rq->wValue.bytes[1];
		}
		else if(rq->bRequest == USBRQ_HID_GET_PROTOCOL)
		{
			if (rq->wValue.bytes[1] < 1)
			{
				protocolVer = rq->wValue.bytes[1];
			}
		}
		else if(rq->bRequest == USBRQ_HID_SET_PROTOCOL)
		{
			usbMsgPtr = &protocolVer;
			return 1;
		}
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
uchar usbFunctionWrite(uchar *data, uchar len)
{
	if ((expectReport)&&(len==2))
	{
		LEDstate=data[1]; // Get the state of all 5 LEDs (data[0] is the report id)
/*		if (LEDstate&(1<<LED_CAPS))// If CAPS lock is OFF
		{
			LED_PORT = (LED_PORT | (1<<LED_CAPS_LOCK)); //Turn on caps lock led
		}
		else// If CAPS lock is ON
		{
			LED_PORT = (LED_PORT & ~(1<<LED_CAPS_LOCK)); //Turn off caps lock led
		}*/
		expectReport=0;
		return 1;
	}
	expectReport=0;
	return 0x01;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void usbReset(void)
{
	uchar i, j, k;

	for (k = 0; k < 8; k++)
	{
		// on even iterations, output SE0
		if (!(k & 1))
		{
			PORTD &= ~USBMASK;	// no pullups on USB pins
			DDRD |= USBMASK;	// output SE0 for USB reset
		}
		else
		{
			DDRD &= ~USBMASK;	// set USB data as inputs
		}

		// delay
		j = 0;
		while (--j)// USB Reset by device only required on Watchdog Reset
		{
			i = 0;
			while (--i)// delay >10ms for USB reset
			{
				nop();
				nop();
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void hid_clear(void)
{
	inputBuffer2[1] = 0;	// mouse buttons
	inputBuffer2[2] = 0;	// X axis delta
	inputBuffer2[3] = 0;	// Y axis delta

	inputBuffer1[1] = 0;	// modifiers
	inputBuffer1[2] = 0;	// key
	inputBuffer1[3] = 0;	// key
	inputBuffer1[4] = 0;	// key
	inputBuffer1[5] = 0;	// key
	inputBuffer1[6] = 0;	// key
	inputBuffer1[7] = 0;	// key
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void send_packets(void)
{
	/* send pending packets */
	if (usbInterruptIsReady())
	{
		if (tx_kbd)
		{
			usbSetInterrupt(inputBuffer1, sizeof(inputBuffer1));	// send kbd event
			tx_kbd = 0;
		}
		else if (tx_mouse)
		{
			usbSetInterrupt(inputBuffer2, sizeof(inputBuffer2));	// send mouse event
			tx_mouse = 0;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
