# 32x10_character serial terminal

Uploaded by iolair on 2013-05-08 04:44:00 (rating 5 out of 5)

## Summary

High-density (320 characters at a time) serial display using ST7920-based 12864 display. For ATMEGA8 running on internal 8MHz clock. Serial data from any source is buffered through a MAX-232 converter (schematics everywhere on the net) and applied to pin PD2 of the Atmega8. Other wiring is explained at the top of the 'main.c' file.  

I obtained the 12864 display from EBay for about $10, and was disappointed with it's native character display - 4 lines of 16 characters. With 126x64 pixels, I knew it could do better than this!

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
