# Retro Games Console (Open)

Uploaded by autismuk on 2001-08-29 09:37:00 (rating 0 out of 5)

## Summary

This project is a colour video games console of low resolution (upto 160 x 192 pixels, 8 colours) based around AVR RISC technology. The design is built for simplicity of construction and consists of an 8515 or upward compatible MCU clocked at 8Mhz, a 32k x 8 SRAM chip which functions as video RAM, a 74244 chip and a few resistors. It connects to a TV via a SCART socket, or to an RGB monitor, and as such for NTSC TV or PAL TV without SCART will require an encoder chip such as MC1378. There will be a greyscale version which does not require any extra hardware beyond the resistor network.


The core OS and video interrupt routines is in assembly, and drives the video independently, providing screen drawing routines and sprite routines. the games will be written in 'C' or a Forth variant currently being developed.


Hardware design : 100% completed  

Operating System: 80% completed  

FORTH Compiler: 60% completed  

Some "Launch" Games : 0% completed.

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
