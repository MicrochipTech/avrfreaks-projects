# Swfltek Boot Loader

Uploaded by mikericetga on 2013-06-04 20:07:00 (rating 0 out of 5)

## Summary

This boot loader was designed to fit my need for an Arduino compatible boot loader with EEPROM support. 


Though really designed for the atmega328p, it has been tested with atmega168p, 1280, and 1284p. With a little work, it should be usable on most AVRs with a hardware USART.


When compiled for devices with <= 64K of program flash, it occupies ~500 bytes. While >64K is supported, the size then exceeds 512 bytes.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
