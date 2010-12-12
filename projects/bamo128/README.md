# BAMO128

Uploaded by bh2009 on 2010-12-12 10:30:00 (rating 0 out of 5)

## Summary

Bamo128 is a resident monitor program for AVR8 microcontrollers. The monitor is written in assembler (GNU toolchain: avr-as) and communicates with the host-PC over a serial interface with an appropriate terminal program (advantageously minikermit from <http://cs.ba-berlin.de> or <http://code.google.com/p/minikermit> - minikermit contains a arduino compatible bootloader interface!)  

With bamo128 you can:  

- visit/modify/copy the different memories in an AVR8 microcontroller  

- burn flash program memory with a bootloader and execute (also in  

step-mode and with break-points) asm- and C- programs  

- visit and modify cpu-state (flags, regs,...)  

- disassemble flash instructions.  

Useful monitor functions can be linked to external asm- C-programs with a linker script.  

For this functionality Bamo128 needs 4 KWords (8K Bytes) of flash memory advantageously in the boot section of the microcontroller and about 256 bytes ram for monitor variables (MONRAM).  

In version 05 we use bamo128 at boards with the atmega128 (CharonII with 32K external RAM and the Mica2 without external RAM) and the atmega1280 on the ArduinoMega board.  

You can simple burn the monitor in a naked microcontroller with your programmer. See the Readme-file in the released packet. You can download the latest stable version from  

<http://cs.ba-berlin.de> (there is an error on avrfreaks-site, I can not append an URL or files) or from <http://code.google.com/p/bamo128>.

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
