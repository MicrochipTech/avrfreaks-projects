# Tiny13 Firmware Updater

Uploaded by *Claus* on 2007-10-13 14:56:00 (rating 0 out of 5)

## Summary

This is a firmware updater for the Tiny13 micro. It uses a software UART to communicate with the PC program (C# code included) though a serial or USB link.


It was made for a college project and still contains the real application since someone might get something out of reading my code :)


The updater code and the software UART takes up 448 bytes of code which doesn't leave a lot for the application but enough in many cases.


To use this updater keep the updater part (the code until the obvious divide) but replace the application code. The assembled .hex file will then hold both the updater and the application. The C# application will update only the part of the flash that is after the updater code so even if something goes wrong in your application code you just need to restart the update process. If you do make changes to the first part of the code (the updater) you will need to download the full .hex again.


If someone has any wishes or comments just leave a PM.


*claus*

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
