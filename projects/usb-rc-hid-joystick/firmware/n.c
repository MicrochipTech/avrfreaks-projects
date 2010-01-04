#include "common.h"
//
// Execution starts here
//
int main(void)
{
    wdInit();                       // initialize watchdog timer
    usbInit();                      // initialize USB driver
    inDecoderInit();                // initialize input decoder
    usbDeviceConnect();             // connect USB device to USB bus
    sei();                          // required by USB driver and some interfaces

    while (1)
    {
        wdReset();                  // reset watchdog timer
        inDecoderPoll();            // poll for input data
        outSendData();              // prepare data for USB Interrupt In endpoint
        usbPoll();                  // process USB requests

#if BOOT_SUPPORT_ENABLED
        if (bootRequest())          // jump to boot loader if switch is pressed
        {
            usbDeviceDisconnect();  // disconnect USB device from USB bus
            inDecoderStop();        // stop input decoder interrupts
            bootJump();             // jump to boot loader
        }
#endif
    }
}
