# ds1302 library in avr-gcc

Uploaded by oods_52 on 2003-07-12 22:42:00 (rating 0 out of 5)

## Summary

!!!!!!!!NEW AS OF 10/24/03!!!!!!!! I have started a web page and there is more information about this project and many others!!!!! <http://avr.oodselectronics.net>. Look under projects for the clock, or the appnotes menue for the ds1302!!!


This is a library that uses a software three wire SPI that communicates with the real time clock ds1302. In the zip file, there is the ds1302.c, which is the library itself, the ds1302\_test.c which tests the ds1302 library by reading the seconds and then sending the second over the uart of the at90sl8535. The delay.h file can delay in ms or us, the uart.c file sets up the printf command that outputs over the uart. The makefile compiles the file ds1302\_test.c, this can be changed so the library can be used with any other chip.


To use this library in your own program you need to creat two external clobal unsigned characters, b10 and bpm.


The library uses PORTB for the connection to the DS1302, the reset pin is pin B5, the clock pin is B6, and the data pin is B7. Feel free to change the pins in the library by changing the definitions, but do not change the port from PORTB. You may attemp to change the port if you know what you are doin, but i recomend keeping it on PORTB.


This library also can be modified for use with other chips that use a 3-wire bus, but make sure you know what you are doing.


I hope that many people will be helped with this library.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Part-specific Lib.functions
- WinAVR (GNU GCC)
