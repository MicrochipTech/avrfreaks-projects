# XTV - XMega TV output

Uploaded by condemned on 2012-06-24 20:55:00 (rating 0 out of 5)

## Summary

XTV - XMega TV output


Black and White TV output for XMegas!


This code provides simple TV output for you to build upon.


* PAL or NTSC composite video output  

* Bitmap: up to 320x240  

* Text: up to 40 x 25 characters  

* Black and White  

* Interrupt driven  

* Just needs 2 resistors!*


[![](https://www.batsocks.co.uk/img/XMega/XTV/XTV_Sample1_320.jpg)](https://www.batsocks.co.uk/readme/XMegaExamples.htm)  

*Sample Text Output*


[![](https://www.batsocks.co.uk/img/XMega/XTV/XTV2_Demo_320.gif)](https://www.batsocks.co.uk/readme/XMegaExamples.htm)  

*Sample 160 x 120 Bitmap Output*


*Bitmap Video examples*


 


*Text Video example*


 


Because the TV signal is generated in background, you're free to do whatever you like in the main loop. The XTV code just displays what you put in the character-array / bitmap.


The code is available to download on the [Batsocks XMega Examples](https://www.batsocks.co.uk/readme/XMegaExamples.htm) page.


*An external 16Mhz crystal is optional, but does make the display rock-solid.


Technical


Peripherals used:  

2 Timer/counters  

1 USART  

1 DMA channel (optional)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
