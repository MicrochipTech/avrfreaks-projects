# PCB_111000_UNO part 1: PCB 111000_1 & UNO_bootloaderForHex&textV6

Uploaded by osbornema on 2020-01-16 20:31:00 (rating 0 out of 5)

## Summary

Updated 20/06/2020 to “UNO\_bootloader\_for\_hex&text\_V6”.


 


A new plug-in pcb for the UNO to be known as PCB 111000\_1 is introduced here. The aim is to provide new hardware that will enable a novice to explore the C programming language when applied to the Atmega 328.


The pcb contains an 8 digit display, user switches and an Atmega 328. This hosts a driver for the display, a programmer, an I2C master and routines for other functions including timers, random number generators and arithmetic. This device is of course in addition to the one provided on the UNO.


The intention is that PCB 111000\_UNO should behave as a single product rather than two products (i.e. the UNO and PCB 111000\_1) simply plugged together. For this reason a new bootloader for the UNO is proposed.


Of course the UNO, pre-loaded with the Arduino bootloader already provides an excellent platform for the novice programmer. However it could be argued that the UNO is primarily a prototyping platform.   And it is sufficiently flexible to enable development, not only  of new hardware such as PCB 111000\_1, but also new software as is being proposed here.


PCB 111000\_UNO is very easy to assemble and a range of projects have been developed which it is hoped will be instructive as well entertaining (at least in some cases). With the benefit of hindsight many projects have been discarded. These either appeared over complicated or rather dull.


The attachments include:  

A picture of a prototype system  

An introduction to the new bootloader  

The new code “UNO\_bootloader\_for\_hex&text\_V6” as a zip file  

Two test applications neither of which require the presence of PCB 111000\_1.


 


Future postings will give


Additional details required to complete PCB\_111000\_UNO i.e. to build and integrate PCB 111000\_1 (also known as PCB\_A) with the UNO.


A selection of user projects that can be developed using Arduino, WinAVR or Atmel studio.


Eagle circuit and pcb design files (but not Gerber or drill drawings).

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
