# cf2avr

Uploaded by mbr4 on 2005-02-21 14:21:00 (rating 0 out of 5)

## Summary

21-02-2005  

I've added files with send/recive file to/from cf card routines. (not FAT, only by sectors). Sending and reciving acros rs232 is very slow, about 5~6 kbytes/s (10~12 sectors/s)


17-02-2005  

I've added version with 1 free pin in microcontroller.


16-02-2005  

I've connected the Compact Flash card (32MB) to little at90s2313, and implemented basic protocol functions: read\_sector, write\_sector and read\_info.  

Other devices (ex. digital thermometer, dac) can be connected to latch using adresses 0x08-0xff (by chip select).  

Complete source code including EAGLE schematics, and pc receiver program.  

In future I upload schematic and code with more free pins.  

Included pc receiver program is writed in c. This program recaiv drive info and whole card data (all sectors) and write it to file (parameter). Writing is very slow, because of rs232 speed. My 32MB image file have been created about 1.5 hour. This image is 1:1 copy of card in file, and it can be readed by my program (link below) or "Disk Image Viewer" (freeware).

## Links

- [fat16reader](http://student.uci.agh.edu.pl/~mstrug/files/fat16reader.exe)

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
