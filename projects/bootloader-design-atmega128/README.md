# Bootloader design for ATmega128

Uploaded by Lalbert on 2007-05-22 05:40:00 (rating 0 out of 5)

## Summary

This note describes the implementation of a stand-alone bootloader program which resides in a dedicated block of flash memory (4k words at 0xF000) in the ATmega128 AVR processor. The bootloader provides a convenient method to update the main application firmware, in-system, from a host computer via a serial connection (on-chip UART or USB peripheral chip).


The documentation package (zip file) includes example source code files and a customized linker command file (bootldr.xcl). The source code does not comprise a complete bootloader program. However, it provides ATmega128-specific flash memory programming functions ("spm" module) together with a loader command processor ("loader" module). Device I/O functions need to be added to suit the developer's chosen comm's interface.

## Compilers

- IAR Embedded Workbench for AVR

## Tags

- Part-specific Lib.functions
- IAR Embedded Workbench for AVR
