# Nokia GLCD Library

Uploaded by Negah on 2004-04-13 22:30:00 (rating 0 out of 5)

## Summary

This graphic library supports Nokia 6610, 6100, 7250, 7250i and combaitble color displays trough SPI. The sources are written in assembler and supports Rectangles, Lines, Ellipses, Bitmaps, proportional and fixed multi color Fonts with compression and includes a full featured Font Designer for MS Windows. The GLib is a full link library and the gcc-linker have the chance to remove unused functions. The minimal codesize needed are 600 bytes and 2800 bytes with all features used (plain C need 4600 bytes). All graphic operations supports Background, Foreground, Border- and Filling-colors with transparency. The 256 and 65536 color modes of PCF8833 are supported. Included is a LFSR-SG (linear feedback shift register shrinking generator) as replacement for the inbuild gcc rand() function. This RNG is stronger, faster and far more resource friendly. It have a period of 2**63-2 and can be used for cryptography.


Actual version is 2.1.


best regards, Hagen

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
