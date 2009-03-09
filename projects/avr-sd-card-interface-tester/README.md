# AVR SD Card Interface Tester

Uploaded by transcendentnb2 on 2009-03-08 23:48:00 (rating 0 out of 5)

## Summary

This project provides a serial terminal interface to execute a set of operations on the SD card. This uses avr-libc and is somewhat bloated. The intent is to create a somewhat verbose user interface that is helpful for debugging and learning. It has a small set of commands for reading, writing, and erasing data on the SD card.


The SD card is assumed to be attached to the SPI line, with SS connected to the card's CS. It has only been tested on the ATmega16.


Sorry for the large zip file, but it does have documentation and call graphs :)


Example usage:  

 `SD Card Tester v0.2.0  

> help  

Available commands:  

 get  

 read  

 write  

 erase  

 scan  

 sdinit  

 help  

> sdinit  

Initializing SD Card...  

Card found  

CID: 02 54 4D 53 44 35 31 32 28 81 60 D2 8F 00 72 55  

CSD: 00 2D 00 32 1B 59 83 D0 FE FB 7F 80 16 40 00 AD  

> read 0 0x20  

addr: 00000000, size: 00000020  

0x00000000: 68 69 20 74 68 65 72 65 FF FF FF FF FF FF FF FF hi there........  

0x00000010: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

> write 0 Hello World!  

addr: 00000000, size: 0000000C  

> read 0 0x20  

addr: 00000000, size: 00000020  

0x00000000: 48 65 6C 6C 6F 20 57 6F 72 6C 64 21 FF FF FF FF Hello World!....  

0x00000010: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

> scan 0 0x1000  

0x00000000: 48 65 6C 6C 6F 20 57 6F 72 6C 64 21 FF FF FF FF Hello World!....  

0x00000010: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................`


0x00000800: 53 6F 6D 65 20 68 69 64 64 65 6E 20 64 61 74 61 Some hidden data  

0x00000810: 2E 2E 2E FF FF FF FF FF FF FF FF FF FF FF FF FF ................


> erase 0 0x100  

Erasing addr: 00000000, size: 00000100  

Type "yes" to confirm > yes  

> read 0 0x100  

addr: 00000000, size: 00000100  

0x00000000: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

0x00000010: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

0x00000020: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

0x00000030: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

0x00000040: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

0x00000050: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

0x00000060: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

0x00000070: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

0x00000080: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

0x00000090: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

0x000000A0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

0x000000B0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

0x000000C0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

0x000000D0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

0x000000E0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

0x000000F0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF ................  

>

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
