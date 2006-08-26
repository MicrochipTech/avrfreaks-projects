# delay_x

Uploaded by heinrichs.hj on 2006-08-26 15:13:00 (rating 0 out of 5)

## Summary

Allthough "Busy Wait" is somewhat wasting CPU-Power, I sometimes find it the most efficient way for creating delays.


The include file "delay\_x.h" provides four macros:


\_delay\_ns(double ns)  

\_delay\_us(double us)  

\_delay\_ms(double ms)  

\_delay\_s(double s)  

\_delay\_cycles(double c)


===> The generated delays:


1. are cycle accurate  

2. are code size optimized (NOPs,byte loops, 16bit loops, 32bit loops)  

3. can range from 1 cycle upto 536 seconds (e.g. @8MHz)


Required Environment:  

1. The arguments have to be compile time constants.  

2. Use of compile optimization (2,3,s)  

3. F\_CPU must be defined with the clock frequency


====================


Together with "delay\_x.h" comes a test program "test.c" that I used for verifying the generated delays with Timer1 of an AT90S2313.

## Compilers

- AVR32 GNU Toolchain

## Tags

- General Lib.functions
- AVR32 GNU Toolchain
