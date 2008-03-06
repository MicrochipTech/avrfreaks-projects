# Terminalator II - The SQL

Uploaded by zbaird on 2008-03-06 00:21:00 (rating 0 out of 5)

## Summary

This project was an entry in Circuit Cellar's WIZnet contest (won a Distinctive Excellence honorable mention). It uses a Butterfly and the WIZ810MJ ethernet board to connect a dumb terminal to a MS Access database. The user types SQL statements at the terminal which are passed (mostly uncensored) to the database, with results displayed on the terminal.


This is a proof of concept project rather than a practical application. It shows one method of interfacing the WIZnet board in SPI mode, and a TCP connection to server software. The Butterfly code is in assembly language, and the server code is in VB6 using the MS Winsock control.


A breakout board is used for the WIZ810MJ to overcome the 2mm header spacing, but otherwise there is no additional hardware involved, just a wiring harness.


Files are a slightly modified version of the contest entry, with abstract, full documentation, all code (server and client), pictures and diagrams, etc.

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
