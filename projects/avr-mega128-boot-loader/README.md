# AVR Mega128 boot loader

Uploaded by admin on 2002-03-08 20:42:00 (rating 0 out of 5)

## Summary

I have commercial product with Mega128 which requires boot loader.  

I made one ( the ones I found were not for Mega128 with its 128kB nor for industrial use).  

My boot loader:  

- works in binary mode ( for fast sending)  

- supports full 128kb ( minus boot area)  

- uses 65ms during startup to enter boot mode or execute the application ( just serial communication is needed to enter the lader)  

- uses one flash page to store information about application suitability to run


The PC part of the loader is WIN32 console application.

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
