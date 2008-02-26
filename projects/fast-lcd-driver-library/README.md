# Fast LCD driver library

Uploaded by barrelshifter on 2008-02-26 15:49:00 (rating 0 out of 5)

## Summary

This modified library is optimized for speed for displaying on 128x64 displays or larger. The original code was written by [www.holger-klabunde.de](http://www.holger-klabunde.de) at 11.10.2003. My work uses a portion of the SRAM as a "video" buffer to speed up the functions (circle, text, rectangle). The LCD will be updated only when needed by the update-function. See code sample (display.c).  

Requires ATMegas with a minimum of 2kB SRAM (1kB RAM will be used as "video" RAM). Pls see notes in the code.  

Happy hacking!

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
