# Atmel DFU programmer for win32

Uploaded by o5o5o5 on 2011-09-06 13:39:00 (rating 0 out of 5)

## Summary

DFU-programmer is an implementation of the Device Firmware Upgrade class  

USB driver that enables firmware upgrades for various USB enabled (with the  

correct bootloader) Atmel chips. This program was created because the  

Atmel "FLIP" program for flashing devices does not support flashing via USB  

on Linux, and because standard DFU loaders do not work for Atmel's chips.


You can get sources from [http://dfu-programmer.sourceforg...](http://dfu-programmer.sourceforge.net/)


Example usage:  

1. Erase micro  

dfu-programmer atmega8u2 erase


2. Write user program  

dfu-programmer atmega8u2 flash program.hex


3. Start user program  

dfu-programmer atmega8u2 start
