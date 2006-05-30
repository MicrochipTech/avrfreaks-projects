# U(S)ART library

Uploaded by arturl on 2006-05-29 17:32:00 (rating 0 out of 5)

## Summary

This is library for using UART and USART (including those with 2 or 4 USARTs) in Atmel AVR microcontrollers.  

Its main part is written in assembler and their core size is only about 380 bytes (receiver 190 and transmitter 192). You don't need take care about any U(S)ART interrupts handling. Input and output are buffered and buffers length can be changed (independently) at compilation time to any value between 2 and 254 (default is 8). It is true library (from linker point of view) and only used (referenced) code will be included in the final FLASH image (e.g. only transmitting part). There are included „glue” functions which allow use U(S)ART as input and/or output stream for standard IO functions like fprintf, puts, fgets etc. Examples and documentation in HTML included.  

For each device there is produced separate library file, so if you want to change your program to work with another AVR you must only link it with different archive (library).

## Compilers

- AVR32 GNU Toolchain

## Tags

- General Lib.functions
- AVR32 GNU Toolchain
