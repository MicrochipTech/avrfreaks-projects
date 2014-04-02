# CRC-16 on tiny85 Flash image

Uploaded by cassidento on 2014-04-02 01:51:00 (rating 0 out of 5)

## Summary

This code walks through the tiny85 Flash image from beginning to end, except for the last 2 bytes which hold the expected value. it calculates the CRC-16, and compares it to the 16-bit value stored in the last 2 bytes of Flash. If it compares correctly, then one of two GPIO is pulled low. If it does not compare correctly, then the other of the two GPIO is pulled low. The code needs to be run once in debug mode with a breakpoint to determine what the correct CRC-16 actually is. This is then entered into the source code as the actual\_crc value. The current code operates correctly for the ATtiny85. Studio 6.1 update 2 project.

## Compilers

- AVR Studio 5

## Tags

- Complete code
- AVR Studio 5
