# trf24g RF module code

Uploaded by tymm on 2008-03-01 04:29:00 (rating 0 out of 5)

## Summary

*** This code should be considered obsolete. Please see my nRF2401 radio library code for newer projects


due to changes in AVR-GCC, this project will apparently no longer compile.


***


This is code for using an nRF2401-based transceiver module (example: TRF24G, available from <http://www.sparkfun.com>). 


Files:


trf24g.c -- Routines for using the device  

trf24g.h -- Header file to include for using the device  

trf24g\_hw.h -- mcu-specific tweakable bits (seperated out to make porting easier)  

spin\_delay.h -- fairly accurate uS/mS busy-loop delay routines  

rf\_serial\_test.c -- messy test example of using the comms (for example only; the UART routines it uses aren't included... really just to be used to understand how to use the interface)


(whoops, minor update -- prettied GPL disclaimer slightly, and changed a typo in a #define that won't bite unless you're using both rx channels)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Part-specific Lib.functions
- AVR32 GNU Toolchain
