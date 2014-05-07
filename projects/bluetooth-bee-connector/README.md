# bluetooth-bee-connector

Uploaded by ericwazhung on 2014-05-07 05:11:00 (rating 0 out of 5)

## Summary

An interface for seamlessly connecting any serial peripheral device to a computer via the BlueToothBee, as though it's wired directly to a serial port.


This was designed to install a Wacom "Penabled" digitizer in a Mac PowerBook G4, running OS X 10.5.8. The digitizer is a simple serial device, and once the BlueToothBee is selected as the serial port, the bluetooth-bee-connector automatically configures itself for things like baud-rate and dataI/O. Thus, the default serial-device driver (TabletMagic, in this case) has no problem detecting the device.


Handles things like automatically configuring the BTB, resetting it when necessary, etc. Written for an AVR, there is a PC-based test-program, and it should be easily ported to any microcontroller.


Not sure whether the difficulties I encountered were due to my Operating System, or the BTB, but after the BTB serial-port was closed, the BTB had to be reset in order for the computer to re-open it. Among other things.


It should be compatible with any OS, since the btb-connector circuitry exists on the BTB-side and its entire intent is to create an invisible interface as though the RS232-peripheral is connected straight to an RS-232 port.


[https://code.google.com/p/blueto...](https://code.google.com/p/bluetooth-bee-connector/)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
