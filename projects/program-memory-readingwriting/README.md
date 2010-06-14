# Program Memory Reading/Writing

Uploaded by drift on 2010-06-13 18:28:00 (rating 0 out of 5)

## Summary

This project uses the boot.h and pgmspace.h routines to read and write to program memory (the flash memory). These routines are useful for data logging or large constant parameter storage. To use this code you must have the UART libraries from Peter Fleury (<http://jump.to/fleury>) and you must create a .bootloader section in your makefile. If you are using AVR Studio you can easily make the .bootloader section by selecting Project -> Configuration Options -> Memory Settings, then add a new memory segment to the flash named .bootloader with the same address shown in the fuse programming for the bootloader memory.


*THERE ARE NO PROTECTIONS INCLUDED IN THESE ROUTINES TO PREVENT OVERWRITING OF PROGRAM CODE


**THE FLASH MEMORY ONLY HAS 10,000 GUARANTEED WRITE CYCLES (flash can be read unlimited times, though)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
