# ST7565 assembly LCD routines

Uploaded by stewarttodd on 2012-12-19 05:56:00 (rating 5 out of 5)

## Summary

The ST7565 128 x 64 serial (SPI) low cost graphic screens are great for text/status display. Generally used bit-mapped into system ram which takes 1024 bytes for the screen.  

These routines only use 5 bytes of system ram but limit you to text and box-drawing characters.  

This implementation uses bit-banged SPI.  

It saves on system ram but the basic font takes up (256 x5) bytes. Limited double sized number support & +-.,  

You get 8 rows of 21 chars and in its basic use, the screen wraps horizontally to the next line and vertically back to the top.  

Supports positioning (row 0..7, coll 0..20) and is straightforward to use.  

Normal (black on clear) or inverted (clear on black) text.  

The sample project shows most of what it can do. Just define your I/O pins in the ST7565.asm file and the demo cycles through.  

Some pics of the display in the zipfile. I got mine at adafruit.  

Enjoy....

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
