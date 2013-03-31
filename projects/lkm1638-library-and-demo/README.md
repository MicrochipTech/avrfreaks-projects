# LKM1638 library and demo

Uploaded by iolair on 2013-03-30 18:48:00 (rating 0 out of 5)

## Summary

 Deal Extreme ([www.dx.com](http://www.dx.com)) has a LED module on sale for $7 which is extremely useful in AVR projects needing a display or for debugging. It has 8 seven-segment numeric displays, 8 2-color leds, and 8 pushbutton switches. It uses a TM1638 display chip which multiplexes the various leds for low power usage, and can be cascaded and addressed on the same 3-wire bus using different SPI addresses. Coolest of all, you can interface it with only three pins plus power (5V) thanks to it's SPI interface, a protocol that many AVR chips support natively. You can find it at [http://dx.com/p/8x-digital-tube-...](http://dx.com/p/8x-digital-tube-8x-key-8x-double-color-led-module-81873).  

 There are a few Arduino libraries out there for this device but nothing ready-to-use for us bare-bones types. Here's my attempt at a library for this unit, with a demo program written for Atmega8 which demonstrates the various functions.


 Peter Barnes  

 AA1YY


NOTE: I've had no luck trying to upload a zip of the source files for this via the AVRFreaks project uploading, keeps failing. You also can try downloading it from the 5th post here:  

[https://www.avrfreaks.net/index.p...](https://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=130554)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Part-specific Lib.functions
- WinAVR (GNU GCC)
