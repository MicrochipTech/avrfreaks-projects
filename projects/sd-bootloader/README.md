# SD bootloader

Uploaded by kalbun on 2012-09-14 12:15:00 (rating 5 out of 5)

## Summary

This is a simple bootloader that:  

- reads a file from an SD card formatted as FAT16  

- writes the file into the flash memory  

- reboots the MCU


The bootloader is based on Arduino's FAT16 and SDCard libraries, but can run on any hardware, provided the SD card is connected to the SPI port.


Maximum supported card size is 2 Gb.


The file must be in pure binary format. You can prepare it by using an utility like the open-source srec\_cat


I recommend to read the description in the main file, because it also explains in more details the preparation phases.


In the future, I will consider:  

- changing FAT library, in order to use cards up to 4 Gb  

- add code to read directly the .hex format, eliminating the need for a conversion


IMPORTANT: my version of Atmel Studio 6 is 6.1.7601 service pack 1.


It seems that older version cannot open the project. In case of problems please let me know.

## Compilers

- AVR Studio 5

## Tags

- Complete code
- AVR Studio 5
