# AVR 8515 LCD with serial Interface

Uploaded by ralcocer on 2004-11-17 09:00:00 (rating 0 out of 5)

## Summary

A serial interface for the Truly MTC-C162DPLY-2N  

2 line X 16 char LCD display  

Original posted at <http://projects.cappels.org/>  

With this arrangement, the Truly display has a 4 bit parallel interface. The AT90S8515 controller converts incoming serial data to the appropriate 4 bit parallel commands and data. Incoming characters are written to the bottom line of the display and when a displayable character, that is a character that is not a control character, is received after a linefeed is received, the bottom line is copied to the top line, leaving a blank line on the bottom with the cursor set to the first column of the bottom line.

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
