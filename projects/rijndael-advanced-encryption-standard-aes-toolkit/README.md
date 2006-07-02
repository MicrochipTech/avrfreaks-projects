# Rijndael / Advanced Encryption Standard (AES) Toolkit

Uploaded by Mike B on 2006-07-02 14:44:00 (rating 0 out of 5)

## Summary

This toolkit contains three different implementations of complete Rijndael / Advanced Encryption Standard (AES) programs. There is an implementation oriented towards FLASH memory usage, an implementation oriented towards SRAM memory usage and an implementation oriented towards a smaller overall memory footprint. Each implementation has key generation, encryption and decryption source code. The SRAM and small memory usage versions have source code for generating tables used by Rijndael / AES. The target processor is an ATMEL AT90CAN128. There are notes included on porting to other AVR processors. These programs can also be used for AVR boot loaders, MAC algorithms, hash functions, stream ciphers, pseudo-random number generators, etc.


ATMEL assembler version 2.1 was used. This is the assembler version that was first included in AVRstudio version 4.12 Service Pack 1 and later. Earlier versions of the assembler do not support this source code. There is a readme.txt file with additional information and the source code is commented. As documented, I wrote this code from scratch and I do not place any restrictions on private or commercial use (please see the source code comments for complete information). My intention is to make a contribution to this really great community here at AVRfreaks.


There were no bugs reported for version 1.0. The 1.1 release optimized the shiftrows and invshiftrows code for a slightly smaller program memory size and a very minor increase in execution speed. I discovered AVRstudio 4.12 service pack 1 or later is required to get the correct assembler version, so I updated the above notice.

## Compilers

- AVR Assembler

## Tags

- General Lib.functions
- AVR Assembler
