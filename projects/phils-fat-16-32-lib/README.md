# Phils fat 16 32 lib

Uploaded by philmalphalink2000 on 2005-04-30 23:05:00 (rating 0 out of 5)

## Summary

This library contains not only the basic read and write functions, but many other functions to help getting an embedded AVR FAT16/32 application up and running using BASCOM-AVR.  

It is able to mount a drives first partition whether it has a partition table or not, and access any directory.  

The user of the library does not need to be concerned about FAT details, buffer status, and the type of drive, as it is all taken care of by the libraries.  

Only short file names (8.3) are supported.  

The mass storage device drivers are separate libraries, and currently have drivers for an ATA hard disk or compatible connected to general I/O ports and one for when it is memory mapped in XRAM space. The ATA hard disk library will also put the drive to sleep after a pre-determined time of no disk access.  

A hardware driver for a MMC / SD card connected to the hardware SPI has also been written.  

The real time clock may use either the hardware based DS1307 or use the software based BASCOM-AVR internal soft clock

## Compilers

- BASCOM-AVR

## Tags

- General Lib.functions
- BASCOM-AVR
