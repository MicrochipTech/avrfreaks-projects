# Barcode Chip

Uploaded by dvanhorn on 2002-07-28 19:11:00 (rating 0 out of 5)

## Summary

This is a complete implementation of a single-chip barcode decoder, on the AVR2343.  

It needs no external components, other than a barcode pen with TTL output, and a power supply.  

It is designed to be a peripheral for a host system, to offload the complexity and interrupts.  

The host interface is four pins, /RESET, /CS, clock and data.


It decodes UPC, EAN-13, EAN-8, and UPC-E. Two or five digit supplementals are supported in all symbologies, and reading can happen in either direction. 


The code could be modded to work with a laser scanner if needed, but it is designed for hand-scanned barcode pens.


Good reference material: The Bar Code Book by Roger C. Palmer I prefer the first edition, but it's hard to find.


Also included is the BA1FB programmer, so all you need is this file, the AVR assembler, and six wires from your parallel port to program the chips.


The code is highly commented, and highly compressed. The original implementation didn't do half of this, and was over 1500 words. This version has a few words free in the 2343.

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
