# AVR Basic Interpreter

Uploaded by madmaxo@hotmail.com on 2006-09-26 21:35:00 (rating 0 out of 5)

## Summary

This is an experimental project aimed to learn how to implement an Interpreter for AVR Atmega microprocessors.


BASIC was choosen because of ease implementation and learning.


It is based on Micro-Basic from Dave Dunfield:  

* Copyright 1982-2000 Dave Dunfield  

* All rights reserved.  

*  

* Permission granted for personal (non-commercial) use only.


MICRO-C code has been ported to WinAVR and it still an alpha version. Primary target would be Atmega16/32/64/128. Tested on Atmega128 OLIMEX board.


Currently the BASIC source code must be typed on a VT100 Terminal program via serial (RS232) connection. So it is not possible to save the code, only interpret and run.


Current Status:  

- Not all functions are implemented or tested.  

- Most functions that relates to file operation won't work for obvious reasons.  

- Functions to access PORTS, PINS and DDR are not fully implemented.


Future plans:  

- implement i2c routines to save BASIC code on external serial eeprom  

- implement I/O functions to access PORTS, PINS and DDRS  

- implement ADC functions  

- implement Timers functions  

- implement Interrupt functions


Requirements:  

- Atmega16/32/64/128 with RS232 port. This version was tested with OLIMEX MT-128 board. Should work on other 16/32/64 boards.  

- Using UART1 for serial connection.  

- PC terminal with local ECHO enabled.  

- Free RS232 port 9600/8/N/1  

- WinAVR 3.4.6 and Procyon AVRlib Version 20051006


How to build:  

- Unzip the file to a folder  

- Edit the Makefile for the appropiate AtmegaXXX  

- From command prompt type: "make"  

- If ok generates .HEX .COF and other files


Archive contents:  

- BASIC.COM = compiled PC BASIC interpreter  

- BASIC.C = original Mikro-C source code  

- MBASIC.C = AVR source code  

- MBASIC.HEX = compiled source code for ATMEGA128  

- README.TXT = this file  

- BLKJACK.BAS = example does work  

- HILOW.BAS = example works ok  

- global.h = global definitions  

- Makefile = Makefile  

- RENUMBER.C = RENUMBER source code  

- RENUMBER.COM = compiled PC RENUMBER tool


History:  

- Version 0.3a Fixes and updates [50 KB] (Sep 25, 2006):  

 - Now while running a script CTRL+C will stop execution  

 - New ADCSET and ADCREAD functions not tested but should work  

 - New CLS functions to clear screen requires VT100 terminal  

 - Fixed bug with functions such as IN(, ADCREAD(, etc  

 - Planning to implement SAVE and OPEN functions with simple file system


- AVR BASIC Interpreter [47.22 KB] (Sep 22, 2006), some fixes:  

 - inclusion of PORTG, DDRG, PING  

 - inclusion of PINS definition variable  

 - updated IN(PINX) function  

 - updated OUT and DDRSET functions to reflect PORTG 


- AVR BASIC Interpreter [32.22 KB] (Sep 22, 2006), original post


Have fun ! And remember that this is still an ALPHA version !


PS: This is my first WinAVR (AVR-GCC) project. So many things are new for me. Code is actually very lazzy and buggy. I'm still trying to remember/learn how to edit Makefile's.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
