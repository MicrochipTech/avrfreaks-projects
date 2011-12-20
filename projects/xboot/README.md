# XBoot

Uploaded by alex.forencich on 2011-12-20 05:19:00 (rating 0 out of 5)

## Summary

XBoot is an extensible, modular bootloader for the ATMEL AVR processor series, compatible with both AVR ATMEGA and AVR XMEGA devices with sufficient memory. It is compatible with the AVR109 (butterfly) bootloader protocol with a few XMEGA specific extensions for access to the user and production signature rows. XBoot includes several advanced features including multiple serial busses and an API providing the ability for the running application to update itself without the need for external programming hardware. Many bootloaders only support RS232 for programming from a PC, but XBoot's modularity allows it to support the exact same set of commands over any hardware serial port. Currently, IÂ²C, RS485, and parallel FIFO support have been incorporated. This allows for easy in-system reconfiguration of XBoot equipped chips with little additional time investment. Also, XBoot includes support for IÂ²C address autonegotiation for when multiple, identically configured processors sit on the same IÂ²C bus. 


[https://github.com/alexforencich...](https://github.com/alexforencich/xboot)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
