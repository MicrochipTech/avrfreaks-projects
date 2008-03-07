# AVR Flash Loader

Uploaded by zbaird on 2008-03-06 17:40:00 (rating 0 out of 5)

## Summary

This project was an entry in Circuit Cellar's WIZnet contest (won a Distinctive Excellence honorable mention). It is a Butterfly bootloader which uses the WIZ810MJ ethernet board for communication.


The bootloader operates in automatic or manual mode, and can customize the uploaded files to produce unique firmware (serial numbers, strings, etc.) in every burn.


In manual mode the user may read, write, and verify EEPROM and non-protected flash (below 0x1C00 where the bootloader starts), edit the memory image, and load/save Intel hex or ASCII dump files to disk. In automatic mode the file(s) to be burned and an optional customization script file are specified when the server software is run, then the user simply connects the Butterfly to the ethernet cable for programming with no further intervention.


This is more of a proof of concept project rather than a practical application. It shows one method of interfacing the WIZnet board in SPI mode, and a TCP connection to server software. The Butterfly code is in assembly language, and the server code is in VB6 using the MS Winsock control.


A breakout board is used for the WIZ810MJ to overcome the 2mm header spacing, but otherwise there is no additional hardware involved, just a wiring harness.


Files are a slightly modified version of the contest entry, with abstract, full documentation, all code (server and client), pictures and diagrams, etc.

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
