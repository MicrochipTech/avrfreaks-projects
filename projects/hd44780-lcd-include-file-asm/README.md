# HD44780 LCD include file, ASM

Uploaded by Babylon on 2007-07-05 05:06:00 (rating 0 out of 5)

## Summary

Almost all LCD's use the HD44780 controller. This file can be included in your projects to drive this controller. I think it is easy to use because you just call some simple lines of code once the file is setup correctly.


It uses 4 bit mode which means you have to sacrifice 7 I/O pins instead of 11 in 8 bit mode.


Readme file below:  

= HD44780 LCD include in ASM =  

= By Simon van Veen =


This zip contains a working beta version of a LCD driver using HD44780 in 4bit mode.  

Newer versions will include more functions and more compatability with AVR's.  

For now this one works fine.


All you need to do is:  

 - Connect LCD datapins [4-7] to pins [4-7] on an AVR port  

 - Connect LCD E, R/W, RS pins to 3 other pins on the same AVR port.  

 - include lcdinc.asm in your project  

 - Adjust definitions in lcdinc.asm  

 - Call LCDfunctions from your main .asm file  

 - An example of how to do this can be seen in LCDcontroller.asm.  

 This example connected the LCD to portD of an ATmega32.


Functions you can use:  

 - LCDINIT, Initializes LCD. Set lcdc and lcdb to select cursor/blinking.  

 - LCDTOROWX, Sets cursor to first column of row X. X = [1-4]  

 - LCDPRTCHAR, Prints character that is set in r17  

 - LCDCLEARROW, Clears current row. Cursor has to be at column 1!  

 - LCDGORX, Shifts cursor X times to the right. X = [1-20]  

 - LCDGOLX, Shifts cursor X times to the left. X = [1-20]


Functions that will be in a future update:  

 - LCDTOCOLX Sets cursor to column X in current row.  

 - LCDDDRAMADDR Sets DDRAM address in r17  

 - LCDGETCHAR Reads character at cursor position to r17  

 - Functions to easially generate your own characters  

 - LCD


IMPORTANT:  

 This script uses r16 and 17. Make sure these registers only contain temporary data.


Info:  

 I hope with this readme it is pretty self-explanatory.  

 If not, contact me via my website:  

<http://home.deds.nl/~cmon/> OR [www.cmon.nl.tt](http://www.cmon.nl.tt)


 Thanks to avrbeginners.net because of their excellent LCD driver. [but mine's better ;) ]


Disclaimer:  

 In order to use this file, read the disclaimer on my website.  

 Non of the contents in the .zip file may be used for commercial ends.

## Links

- [Find it at my site](http://home.deds.nl/~cmon/lcddrive.htm)

## Compilers

- AVR Assembler

## Tags

- General Lib.functions
- AVR Assembler
