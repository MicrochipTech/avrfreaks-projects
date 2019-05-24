# Code for an Atmega 328 bootloader

Uploaded by osbornema on 2019-05-24 12:48:00 (rating 0 out of 5)

## Summary

A C boot-loader program is presented for the Atmega 328 device. Its design is based on that of the in-Circuit programmer published by osbornema in "[https://community.atmel.com/proj...](https://community.atmel.com/projects)"


on 22 February 2019.


 


The hard ware routines required to clock data and commands between programmer and target have been replaced by assembly routines designed to implement the LPM (load program memory)


and SPM (store program memory) commands. Prototypes for these subroutines are given in the Atmega 328 data sheet.


 


This posting includes a user guide for the boot-loader and two zip files.  The first contains the code, a makefile  and all C modules required to ensure that it compiles.  The second contains several


programs that use the maths library functions and are included as test files for the boot-loader.  No hard ware details are included. However once it has been programmed with the boot-loader code


the Atmega 328 only requires a connection to a PC and an external reset circuit.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
