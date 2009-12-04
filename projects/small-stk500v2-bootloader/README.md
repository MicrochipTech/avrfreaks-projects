# Small STK500v2 bootloader

Uploaded by Kavka_ on 2009-12-04 04:09:00 (rating 0 out of 5)

## Summary

This program allows an AVR with bootloader capabilities to  

read/write FLASH/EEPROM, read fuses and lock bits,  

write some lock bits (device dependent).  

Bootloader realize subset of STK500v2 protocol, so it work with AVRStudio4 and avrdude.  

To enter Programming mode defined pin is checked. If this pin  

is signaled (pulled low), bootloader started. If not, normal execution is  

done from $0000 "reset" vector in application area.  

Designed for AVRs with bootloader support and FLASH up to 64Kb.  

Fits in 256 words (512 bytes) bootloader section.


1.0beta1 - first public release  

1.0beta2  

+ few lines of code fixed


Tested with ATmega16, ATmega8535.

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
