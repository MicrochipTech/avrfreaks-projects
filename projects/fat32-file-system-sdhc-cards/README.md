# FAT32 File system for SDHC cards

Uploaded by Mickjc75 on 2011-10-06 01:42:00 (rating 0 out of 5)

## Summary

Hi, Here's my FAT32 implementation using an Atmega1281.


Example code allows you to:  

 Create and delete files and directories (with long file names)  

 Append to files  

 Show text files  

 Copy files  

 Play .raw sound files using a PWM output.


Connect a terminal to UART1 at 19200 baud. And press SPACE to get the console menu.


 The .raw sound files in this example must be 15.624kHz sample rate, and 8-bit unsigned. You can create these files with a wave editor like GoldWave.


 The documentation for how to use this module is in the FAT32.h header.  

This project is currently configured for an 8MHz clock, and must be run from 3V (unless you level shift to the SDHC). Simply Google "SD pinout" to get card connections.


 All of the source for the FAT module is fairly neat. However, some of the source files for my project template are a bit scraggly. The reason for this is that they have been tucked away in a linkable library for a few years, but I wanted this example to be completely compilable from source.


I'm very open to feedback and suggestions, just inbox me.


Enjoy.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
