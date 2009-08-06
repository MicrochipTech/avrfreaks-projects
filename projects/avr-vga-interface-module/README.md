# AVR VGA Interface Module

Uploaded by khaled2005 on 2009-08-06 16:09:00 (rating 0 out of 5)

## Summary

It is a small Board that can be connected directly to a color VGA monitor to interface it like a normal LCD module.  

It can print colored characters, draw lines, Rectangles, and other.


the resolution is 640*480, with 256 color


Board interface is 8 data bit, RS, Enable, Busy. like the ones found in small LCD modules.


The board contains AVR microcontroller (ATmega162V) which is used as a command handler, it recieves a command from a host (another microcontroller) and then executes it.


The Board contains a video buffer memory 512 Kbyte and a DAC.


Most of the Video generation logic is made using Altera CPLDs, 4 small CPLDs (44 pin) are used in this project.


There is 4 fonts available, Font Bitmaps are stored in the flash memory as data.

## Compilers

- wavrasm

## Tags

- Complete code with HW
- wavrasm
