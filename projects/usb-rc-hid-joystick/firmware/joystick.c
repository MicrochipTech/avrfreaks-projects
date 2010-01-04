//=============================================================================
// Standard USB HID-compatible game controller (joystick) with buttons.
//
// $Id: out_joystick_btn.c,v 1.1.2.1 2006/08/10 20:58:23 SOY Exp $
//=============================================================================

#include "common.h"

#if OUT_JOYSTICK_BTN

//=============================================================================
// Local variables

// USB report buffer
static uchar usbReport[8];          // cannot use more than 8 bytes for low-speed USB device

//-----------------------------------------------------------------------------
// Standard 8-axes joystick HID report descriptor with buttons

#if (MAX_CHANNELS != 8)
#error  "You must update the usbHidReportDescriptor[] if (MAX_CHANNELS != 8)"
#endif

#define N_REPORT_IDS    2           // total number of Report IDs

PROGMEM char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] =
{
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x15, 0x00,                    // LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              // LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    // REPORT_SIZE (8)
    0x09, 0x04,                    // USAGE (Joystick)
    0xa1, 0x01,                    // COLLECTION (Application)

    // Report 1: X, Y, Z, Rx axes (5 bytes: 1 byte ReportID + 4 bytes axes)
    0x85, 0x01,                    //   REPORT_ID (1)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x82,                    //     INPUT (Data,Var,Abs,Vol)
    0xc0,                          //   END_COLLECTION
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x09, 0x32,                    //     USAGE (Z)
    0x09, 0x33,                    //     USAGE (Rx)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x82,                    //     INPUT (Data,Var,Abs,Vol)
    0xc0,                          //   END_COLLECTION

    // Report 2: Ry, Rz, Slider, Dial axes + 8 buttons (6 bytes: 1 byte ReportID + 4 bytes axes + 1 byte buttons)
    0x85, 0x02,                    //   REPORT_ID (2)
    0x09, 0x34,                    //   USAGE (Ry)
    0x09, 0x35,                    //   USAGE (Rz)
    0x09, 0x36,                    //   USAGE (Slider)
    0x09, 0x37,                    //   USAGE (Dial)
    0x95, 0x04,                    //   REPORT_COUNT (4)
    0x81, 0x82,                    //   INPUT (Data,Var,Abs,Vol)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x05, 0x09,                    //   USAGE_PAGE (Button)
    0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
    0x29, 0x08,                    //   USAGE_MAXIMUM (Button 8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)

    0xc0                           // END_COLLECTION
};

//-----------------------------------------------------------------------------

//
// Build the USB report data using channel data in uS and ReportID
// as an argument. If ReportID is 0 then select it automatically.
//
// Calculates each report byte using the following:
//   - source data should be 732..2268 uS
//   - it should result in 0..255 byte values
// Buttons reflect channel values as discrete controls.
//
static uchar usbBuildReport(uchar id)
{
    static uchar lastReport = 0;    // last ReportID sent

    // choose the ReportID to build: use specified or select next
    if (id)
        lastReport = id;
    else
        lastReport = (lastReport % N_REPORT_IDS) + 1;

    // preserve the ReportID
    uchar *rp = &usbReport[0];
    *rp++ = lastReport;

    // build the report requested
    int data;
    int i;

    switch (lastReport)
    {
        case 1:
            for (i = 0; i < 4; i++)
            {
                // ensure atomic operation
                cli();
                data = channelData[i];
                sei();

                // bounds check and correction
                if (data <  (1500-6*128)) data = (1500-6*128);
                if (data >= (1500+6*128)) data = (1500+6*128)-1;

                // storing channel
                *rp++ = (uchar)((data - (1500-6*128)) / 6);
            }

            // return size of Report 1
            return (1+4);

        case 2:
            for (i = 4; i < 8; i++)
            {
                // ensure atomic operation
                cli();
                data = channelData[i];
                sei();

                // bounds check and correction
                if (data <  (1500-6*128)) data = (1500-6*128);
                if (data >= (1500+6*128)) data = (1500+6*128)-1;

                // storing channel
                *rp++ = (uchar)((data - (1500-6*128)) / 6);
            }

            // prepare buttons
            int *dp = &channelData[0];
            char buttons = 0x00;
            char mask    = 0x01;

            for (i = 0; i < 8; i++)
            {
                // ensure atomic operation
                cli();
                data = *dp++;
                sei();

                if (data > 1500) buttons |= mask;
                mask <<= 1;
            }

            *rp = buttons;

            // return size of Report 2
            return (1+4+1);
    }

    return 0;
}

//-----------------------------------------------------------------------------

//
// USB setup request processing
//
uchar usbFunctionSetup(uchar data[8])
{
    usbRequest_t *rq = (void *)data;

    if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS)
    {
        // Return the report if host requests it without USB interrupt
        if (rq->bRequest == USBRQ_HID_GET_REPORT)
        {
            // wValue: ReportType (highbyte), ReportID (lowbyte)
            // build report requested
            usbMsgPtr = usbReport;
            return usbBuildReport(rq->wValue.bytes[0]);
        }
    }
    return 0;
}

//-----------------------------------------------------------------------------

//
// Check if the USB Interrupt In point buffer is empty and return
// the data buffer for the following host request via USB interrupt
//
void outSendData(void)
{
    if (usbInterruptIsReady())
    {
        // fill in the report buffer and return the
        // data pointer (Report ID is selected automatically)
        usbSetInterrupt(usbReport, usbBuildReport(0));
    }
}

#endif  // OUT_JOYSTICK_BTN
