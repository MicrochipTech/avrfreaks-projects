# LCD Graphics Primitives Library

Uploaded by alenze on 2003-06-07 16:59:00 (rating 0 out of 5)

## Summary

* Updated Jun. 25: added 'display on' code, minor fixes *  

* Updated Jul. 13: character print routines will overwrite 'old' information corrrectly in 'set' mode


This library is an enhanced re-arrangement and somewhat 'generalized' compilation of modules I've written over the past two years for our designs, following Mr. Kalle's "Nokia 7110 LCD graphics library V2" example.


Currently included is a device driver for the SED136x/15xx family (w. parallel interface). Other compatible controllers are e.g. HD61202/KS0107 (8 page organization), KS0713 etc.


Functions currently included are:  

- Line (with clear/set/invert)  

- Rectangle (outline/filled w. clear/set/invert)  

- Circle (outline/filled w. clear/set/invert)  

- Ellipse (outline/filled w. clear/set/invert)  

- Polygon/Area Seed Fill(s) (pixel set only)  

- Bitmap (Icon/Picture w. clear/set/invert)  

- Single Pixel R/W (w. clear/set/invert)  

- Single Character (w. clear/set/invert)  

- ASCIIZ String (w. clear/set/invert)  

- Print BCD as ASCII (w. clear/set/invert)  

- Print HEX as ASCII (w. clear/set/invert)  

- Clear/Fill/Invert Screen  

- Write Frame Buffer to Display (device driver for 2xSED1520, 120x32 pix)  

- Three fonts (8x8 bold, 8x8 thin, 5x6 small, full IBM extended character set (code page 437)

## Compilers

- AVR Assembler

## Tags

- General Lib.functions
- AVR Assembler
