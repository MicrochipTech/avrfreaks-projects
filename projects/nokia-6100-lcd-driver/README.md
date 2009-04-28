# Nokia 6100 LCD driver

Uploaded by dstahlke on 2009-04-27 17:46:00 (rating 0 out of 5)

## Summary

This driver works with the new Nokia 6100 LCDs from SparkFun that use a chip that is apparently slightly incompatible with the standard Epson chips. These have a red tab on the protective cover of the LCD and have a green PCB. The driver should also work with Philips displays but this has not been tested.


The code has routines for basic pixel and rectangle drawing, as well as support for fonts and rasters. No diagonal line or circle code is included. There is also a module that supports hardware scrolling.


This is easily portable to any MCU since the MCU-specific stuff is consolidated into two functions. Code is included for ATmega168 and (heresy?) dsPIC.


<http://www.stahlke.org/dan/nokialcd>

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
