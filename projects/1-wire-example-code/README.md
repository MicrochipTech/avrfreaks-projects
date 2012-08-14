# 1-wire example code

Uploaded by SpacedCowboy on 2012-08-14 16:40:00 (rating 0 out of 5)

## Summary

I spent a little bit of time over the weekend porting the Atmel AVR318 code to gcc. 


It appears to work just fine, requires a single digital i/o (I didn't port the USART version, I like using USARTS for other things ) and is happily scanning, finding, and using the DS18B20 temperature sensor connected to the AVR. 


It's pretty self-contained - just 1wire.{c,h} for all the common 1-wire code. The example has main.c and uart.{c,h} as well and displays the temperature every 3 secs over a serial line. In general, though, just #include "1wire.h", add 1wire.c to the Makefile, and you're done. 


Enjoy,  

Simon

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
