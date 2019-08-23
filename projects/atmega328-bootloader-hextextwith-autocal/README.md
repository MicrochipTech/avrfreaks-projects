# Atmega328 Bootloader for hex&text_with autoCal

Uploaded by osbornema on 2019-08-23 16:14:00 (rating 0 out of 5)

## Summary

A bootloader for the Atmega 328 is presented for both hex and txt files. The txt file contains strings for the application to print out. As both files reside in flash the normal restrictions on the amount of space available for strings no longer apply. An auto calibration routine is also provided which sits just above the bootloader. It will only need to be launched if the default calibration factor provided with the device is not sufficiently accurate to allow it to communicate with a PC.


 


The posting includes a user guide and all source files including a short application to read the strings programmed to flash and a calibration disrupter to test the operation of the auto calibration routine.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
