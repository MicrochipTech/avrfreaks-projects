# FBOOT-alternative program loader for 64-bit / 32-bit Windows

Uploaded by Taketoshi Iyota on 2020-10-22 15:41:00 (rating 0 out of 5)

## Summary

I use an Atmel microcomputer (ATmega328P) to teach my students the basics of embedded systems.


I used Danni's FBOOT.EXE to write a program to this microcomputer, but since FBOOT.EXE, which is a program loader on the personal computer side, is a program works on the MSDOS that directly operates the serial port, it does Not work on 64-bit Windows. (On 32-bit Windows, the operating environment equivalent to virtual86 mode is still supported, so it barely worked.)


Currently, most of the Windows used are 64-bit Windows, so Danni's FBOOT.EXE cannot be used. Therefore, I made a new program loader that can be used instead of FBOOT.EXE. The program loader works on 64-bit / 32-bit Windows, because of operating the serial port via the Windows API.


This program is compiled by gcc on MinGW32. The transfer speed is fixed at 38400bps. The serial ports that can be used are COM1 to COM9, and if there is only one serial port, that port is automatically found and used. You can also specify the serial port to be used explicitly.


If you enter only the program file name "prog\_loader.exe" and press the enter key, simple usage examples will be displayed, so please refer to it.


It can be used without problems even when the PC is connected to a microcomputer via USB-serial conversion cable. (I also connect the PC and a microcomputer via USB)

## Links

- [fast tiny & mega UART bootloader](https://community.atmel.com/projects/fast-tiny-mega-uart-bootloader)

## Tags

- Complete code
- ATmega328P
- fboot
