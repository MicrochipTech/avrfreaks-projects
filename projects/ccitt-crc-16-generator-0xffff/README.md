# CCITT CRC-16 Generator (0xFFFF)

Uploaded by Holmes on 2007-01-08 07:21:00 (rating 0 out of 5)

## Summary

An AVR assembler translation of a C program originally developed by Ashley Roll of Digital Nemesis Corp.  

This implementation has a small lookup table size (32 bytes),  

smallish code size 150-200 bytes and processes a 1024 bit  

data block in 582us (20MHz).  

By convention, its initial CRC value is set to 0xFFFF.  

No optimising has been carried out.  

The CRC generating assembler code is in 3 routines which perform:  

initialisation, nibble extraction/preparation & CRC generation.  

It is certainly ideal for my XMODEM requirements and I must admit that I admire the simplicity of Ashley's algorithm.  

It has been tested against Ashley Roll's original test results  

and also against an online CRC calculator for the same test data.  

Test vector: "123456789" (without the quotes); resulting CRC: 0x29B1  

The assembler code documentation contains references to web sites for the original C code and to the online CRC calculator which I used in testing.

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
