# Linux driver example

Uploaded by Jawn on 2009-04-09 08:11:00 (rating 0 out of 5)

## Summary

Abstract:


Writing a device driver is the easiest way to access the hardware on the AP7000 directly without modifying the linux kernel. This project provides a simple working example of a linux driver and a userspace application that interfaces with it.


Usage:


Download and build buildroot from the Atmel website. Once buildroot is built change the buildroot path in both makefiles to point to your buildroot path. The makefiles should give you a spitest.ko and a spitest.


Load the files onto an ngw100 and install the driver with the command


> insmod spitest.ko


Test the ioctl interface by running the spitest application


> ./spitest

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
