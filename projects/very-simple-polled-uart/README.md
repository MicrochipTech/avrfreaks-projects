# Very simple Polled UART

Uploaded by c_oflynn on 2002-11-06 21:49:00 (rating 0 out of 5)

## Summary

April 7, 2003: Added support for BOTH the UARTs (USART technically) in the AtMega128 / AtMega64, but doing this needed to move some of the stuff into the header file (so it isn't as neat)


Feb 13, 2003: Problem with AtMega128 support fixed


Dec 11, 2002: FIxed bug that Mr. Bjørnsen pointed out in the timeout function, a large value for timeout gave you a small timeout, but a large value for timeout gave you a small timeout!


If you need to use the UART of the AVR, but you don't want/need any fancy features in normal UART routines, then these are for YOU. The included routines do the following:  

1) Init UART to selected BAUD rate  

2) Wait for a char, BUT timeout if not recieved within a certain timeframe  

3) Wait forever for a char  

4) Output a char


They also don't use interrupts, mostly to make it easier to port. If you want to port the code, you will likely have to change a simple pre-processor statement that selects which AVR you are using. Plus, no interrupts means that if you use the UART interrupt service routine elsewhere, you could back-up the settings of the UART register(s), run my routines (of course making sure to disable the RX and TX interrupts), then restore the settings of the UART register(s) and use your other UART routines. Be sure to tell me if these don't work for some reason on yoru platform, i'e tested them on a few different processors now.

## Compilers

- AVR32 GNU Toolchain

## Tags

- General Lib.functions
- AVR32 GNU Toolchain
