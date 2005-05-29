# FT232 Modul (USB <> RS232(TTL) converter)

Uploaded by tuttut on 2005-05-28 17:37:00 (rating 0 out of 5)

## Summary

This modul provide a RS232 communication port with TTL/CMOS signals at the one side and USB on the other side. You can easily connect any AVR controller with a UART to this modul to communicate with it over USB. FTDI provides a Virtual COM port (VCP) driver which causes the USB device to appear as an additional COM port available to the PC. Application software can access the USB device in the same way as it would access a standard COM port. You can get more information about the FT232BM chip at [http://www.ftdichip.com/Products...](http://www.ftdichip.com/Products/FT232BM.htm) .


For easier self-etching the modul is designed on a single layer board - 6 bridges on top are the price for that.  

Sorry, but there is no source code (and even no AVR) in this project, because you can use it for any AVR project which make use of an UART. Nevertheless it is quite usefull when dealing with AVRs and you are running out of serial ports. I put it here because it doesn't fit elsewhere. There is no vendor for this modul so you have to build it on your own.

## Links

- [Link: More information about FT232BM chip from FTDI](http://www.ftdichip.com/Products/FT232BM.htm)

## Tags

- Complete code with HW
