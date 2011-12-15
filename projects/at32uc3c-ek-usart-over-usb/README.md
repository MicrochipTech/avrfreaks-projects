# AT32UC3C-EK USART Over USB

Uploaded by drift on 2011-12-14 18:48:00 (rating 0 out of 5)

## Summary

This project demonstrates how to use the USB\_VCP port on the AT32UC3C-EK as a virtual serial port. The UC3C micro sends data over UART to the UC3B micro. The UC3B micro converts the UART to USB using the USB CDC device class. A PC attached to the USB\_VCP port can open a COM port attached to that USB device and treat it as RS-232 input.


Attached are the source code and the USB driver file (at32uc3b\_cdc.inf). I have edited the .inf file to use PID 2310 instead of the typical Atmel PID of 2307 to make the driver work for this development board.


This project was created using AVR Studio 5. Due to upload size restrictions only the source code is attached. To get a working project simply create an AVR Sudio 5 project using the AT32UC3C-EK board template and replace the source code files with the ones attached.

## Compilers

- AVR Studio 5

## Tags

- Complete code
- AVR Studio 5
