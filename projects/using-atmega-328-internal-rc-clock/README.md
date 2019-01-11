# Using the Atmega 328 internal RC clock

Uploaded by osbornema on 2019-01-10 19:30:00 (rating 0 out of 5)

## Summary

The Atmega family of devices come with an internal 8MHz RC clock. This is ideal for many users. It simplifies pcb layout and enables use of plug-in prototyping boards. Generally speaking the accuracy of this clock is good enough to enable communication with a PC using the UART and a USB bridge.


However there does appear to be a slow but steady stream of devices for which communication with a PC is severely restricted. As a last resort this problem can be overcome by adjusting the baud rate settings until communication is possible. But in anticipation of this problem register OSCCAL is supplied in which users can save a calibration number which will automatically be used to calibrate the RC clock.


 


The attachments give more details about how this calibration number can be obtained plus typical plots of frequency accuracy against OSCCAL and listings of the programs used to explore the frequency accuracy of the RC clock in more detail.


 


Note: The USART port of the Atmega 328 was connected to a PC via an FT230XS USB bridge.  The PC was running a terminal program at a baud rate of 57.6K. Other settings were No handshaking, no parity, 1 stop bit and 8 data bits.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
