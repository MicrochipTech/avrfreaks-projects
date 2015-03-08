# Simple, generalized USART setup with i/o (printf() etc.), without ASF, for the UC3B series

Uploaded by tominsf on 2015-03-07 19:22:00 (rating 0 out of 5)

## Summary

For those who use the standalone toolchain with a makefile, without ASF, this header will conveniently set up any of the three USARTs in the AT32UC3B series, automatically connecting the correct port pins to TXD and RXD.


 


It also implements sr\_printf(), sr\_putchar(), sr\_getchar(), and sr\_kbhit(), which work the same as the equivalents in the usual C stdio.h for transfer to and from stdout.


 


One to three USARTs can be set up for simultaneous use, with a #define determining which USART should be used for sr\_printf() and the other i/o functions.


 


It should be easy to modify usart\_io.h to work with other AT32UC3 parts (A, C, D etc.).


 


 


The source file needs two #defines:


 


1. frequency of PBA bus: e.g #define F\_PBA 48000000L (note the L for long)


 


2. which USART will be used for the i/o functions in this header: e.g. #define PRINT\_USART 0 (or 1 or 2)


 


Followed by #include "usart\_io.h"


 


 


usage:


 


uint8\_t setup\_usart(usart\_no, baud, parity, bits, stop, pin\_fcn)


 


   usart\_no: 0, 1 or 2


 


   baud: standard rate between 300 and 115200


 


   parity: N, E, or O (none, even, odd)


 


   bits: 5, 6, 7, or 8


 


   stop: 1 or 2 stop bits


 


   pin\_fcn: which function (A, B, C, D) and thus which pins to connect USART to


 


 


error handling is minimal: usart\_io.h normally returns 0, but returns 99 if an attempt is made to connect to a function (A, B, C, D) that the USART isn't defined for


 


 


sr\_printf() works just like printf()


 


sr\_putchar(), sr\_getchar(), sr\_kbhit() also work like their namesakes


 


 


I've included a simple example source file to show proper usage. It will send three lines of text, then echo back any character sent (e.g. from the keyboard).


 


Note that this has not been tested for all possible combinations of USART and pin functions, so let me know if something doesn't work.


 


 


![schematic](https://www.avrfreaks.net/sites/default/files/usart_io_example.jpg "example schematic")

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
- AT32UC3B1256
- USART
- serial
- UC3
- printf
