# UART Module

Uploaded by BillK on 2001-10-23 15:04:00 (rating 0 out of 5)

## Summary

The C source implements a set of routines which allow the use of the UART(s) present on the various versions of the AVR micros to be operated in polled and/or interrupt mode. The operation of the UART has been fixed at 8 bit and Double Speed is an option for the ATmega161 & 163


Note: The apparent use of floating point in the .h file allows the preprocessor and compiler to calculate accurate BAUD rate divisor(s) for any crystal frequency. It does not however cause ANY floating point code to be included in the compiled code.

## Compilers

- AVR32 GNU Toolchain

## Tags

- General Lib.functions
- AVR32 GNU Toolchain
