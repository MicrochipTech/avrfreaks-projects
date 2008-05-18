# WinLCDBitMap Version 1.0

Uploaded by C@PPER on 2008-05-18 16:47:00 (rating 0 out of 5)

## Summary

This Delphi program will load a color bitmap file, convert it to a monochrome bitmap image based on color choices for foreground and background, then generate a char array for use in your C program for a 240 x 128 LCD display.


Sample output structure:


static const unsigned char g\_graphic[3840] = {  

0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,...  

};


The only issue I know of is the last data point also includes a comma after it (before the closing bracket) that needs to be edited out before it will compile.

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
