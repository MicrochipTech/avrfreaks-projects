# Butterfly data recorder

Uploaded by jglando on 2009-11-08 06:40:00 (rating 0 out of 5)

## Summary

A complete application derived from the Butterly demo code base. This application adds a Record function and NV (flash) Memory dump function to the menu and deletes unneeded entries (music,name, etc..) and unused code to free up program memory. It's a first project for me so feedback on bugs welcome. I have not included documentation yet. The main file to read would be the record.c code. Other changes include adding a recordInterval counter to the RTC.c code and two routines added to the Dataflash.c file. I use TeraTerm to capture the memory dump off the USART (be sure to set TeraTerm to binary and enable logging to a file) then process the data with the included Excel reader. Enjoy.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
