# File generator

Uploaded by daqq on 2007-01-19 22:43:00 (rating 0 out of 5)

## Summary

For testing of a device I created a simple utility to generate a file FULL of one character you specify. I needed a file full of 0x00 and I didn't want to go CTRL+C etc. So, here's 5minutes of work.  

You specify the output file name , the LENGHT (0-really big) and the CHAR number (0-255). You use the command line parameters.  

For instance: to create a file 6840 bytes long, named out.txt and full of 'A' you'd execute from a command line this:


useless\_file\_maker out.txt 6840 65


Walla! It's done.  

Share freely, but I doubt you even want it your selves. 


Compiled using gcc and the DevC++ IDE. 


Enjoy.


David 'daqq' Gustafik

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
