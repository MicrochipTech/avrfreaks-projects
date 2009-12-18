# HD44780 'Hello World' - simple!

Uploaded by Zora53 on 2009-12-18 00:05:00 (rating 0 out of 5)

## Summary

This program is intended to be a very simple introduction to interfacing a microcontroller to a 2x16 character HD44780-based LCD (although it may work on bigger ones too, like 2x20 or 2x40). I used an ATmega8515L and the STK500 development board, but the code is so simple, that it can easily be ported to a number of other AVR uC's. The data transmission mode is configured to be 8-bit, so you will need a total of 10 general purpose I/O pins (8 data + RS + E).


The code is written in assembly and does the following:  

- properly initializes the LCD according to Initialization by Instruction outlined in the HD44780 datasheet  

- writes 'Hello World' on the display  

- if you are using an STK500 or can connect a push button to the external interrupts, prints characters in response to a button press  

The code also allows you to configure some basic display features, such as whether the cursor is visible and whether it blinks or not.


All you have to do is download lcd\_driver.asm and lcd\_44780\_driver.inc, create a new project and compile the code. Take a look at the first few lines of lcd\_44780\_driver.inc to configure the ports to your liking. I used PORTA as an output port to drive the lcd data pins and pins PD4 and PD5 of PORTD to control the RS and E pins on the lcd. Since this is a very simple program, I don't use the R/W pin, which is not essential for printing to the screen - you can safely ground it. And you really do want to connect that potentiometer to adjust the screen contrast.


Helpful documents:  

The official HD44780 datasheet - [http://dipmicro.com/store/datash...](http://dipmicro.com/store/datasheets/HD44780.pdf)  

"How to control a HD44780-based Character-LCD" - [http://home.iae.nl/users/pouweha...](http://home.iae.nl/users/pouweha/lcd/lcd0.shtml#hd44780)

## Compilers

- AVR Studio 4

## Tags

- General Lib.functions
- AVR Studio 4
