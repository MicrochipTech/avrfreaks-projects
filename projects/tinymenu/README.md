# TinyMenu

Uploaded by tymm on 2005-08-03 15:53:00 (rating 0 out of 5)

## Summary

Simple menuing routines -- not so much optimized for run-time efficiency currently (does whole screen redraws for each update), but base code compiles to around 600 bytes and allows for a simple loop to control full navigation through a hierarchy of menus.


tinymenu\_hw.h contains hardware-specific defines (which display interface routines to use, so it can be customized for different LCD's, etc).


Also included is joystick.h (very simple joystic interfacing -- I use a simple joystick w/pullup resistors for each switch), spin\_delay.h (simple fairly accurate routines for uS/mS busy-wait loops), and a quick example app (LCD routines not included; you'll have to tweak for that to use it, or just look at it for example usage).


This makes putting together menus really quite simple.


-- updated 1:45 pm Nov 9


LGPL headers added, slight modifications (can configure to fill unused menu area with spaces instead of clearing display; could be useful if using graphics display & don't want it all wiped). Somewhat better docs. Etc.


NOTE: spin\_delay.h #includes a stddef.h -- this is where it expects to find a #define for MCU\_CLK (in hz -- e.g. #define MCU\_CLK 4000000 for a 4mHz clock)


-- updated Aug 3 2005


Bugs fixed in scrolling up & down for menus that don't fit in given space.  

Fixes to example code. Hope to add decent docs soon.

## Compilers

- AVR32 GNU Toolchain

## Tags

- General Lib.functions
- AVR32 GNU Toolchain
