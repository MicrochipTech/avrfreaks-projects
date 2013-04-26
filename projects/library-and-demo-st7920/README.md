# Library and demo for the ST7920

Uploaded by iolair on 2013-04-26 02:18:00 (rating 5 out of 5)

## Summary

Library and demo for the ST7920 128x64 display. This display is widely available (e.g., [http://www.ebay.com/itm/12864-12...](http://www.ebay.com/itm/12864-128x64-LCD-Display-module-ST7920-blue-backlight-/251001400559?pt=LH_DefaultDomain_0&hash=item3a70d96cef)) on ebay or dealextreme. Cost is wonderfully cheap for a 128x64 display, usually around $10-$15. I've connected an Atmega8 to it, it can do text, graphing, whatever (although at a relatively slow-speed - you could make an oscilloscope out of it it you don't mind a 1-second sweep time! The modules data pins (D0-D7) are attached to PORTB, the RS, RW and E lines are attached to PORTC pins 0-2. Reset goes to Atmega8 reset pin 1. Use 5 volts on Vcc and backlight anode. PSB to plus to use an 8-pin data bus, although the ST7920 can do SPI as well. Use an 8MHz internal clock on the Atmega8. I'm using it for graphing temperature over time.


Peter Barnes  

AA1YY

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
