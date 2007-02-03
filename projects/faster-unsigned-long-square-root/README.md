# faster unsigned long square root

Uploaded by jodis188 on 2007-02-02 17:56:00 (rating 0 out of 5)

## Summary

Algorithm only with shift, compare and substract which give squareroot and remainder allowing to get the closest value. Longer time it take is for big number like 0xFFFFFFFF which need 584 cycles. Algorithm had been given to me by EdPugh (Thanks !) as a C file for DOS. Here is also a slightly modified version which run with WinAVR.With Assembler version, be carefull that result of "sqrt" is given on WORD type format and for number >= 0xFFFF0001, result of function "arrondi" is FFFF + 1 = .... 0 !

## Compilers

- AVR Assembler

## Tags

- General Lib.functions
- AVR Assembler
