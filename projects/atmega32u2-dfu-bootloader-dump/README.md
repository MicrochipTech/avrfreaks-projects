# ATmega32U2 DFU bootloader dump

Uploaded by o5o5o5 on 2011-09-06 13:37:00 (rating 0 out of 5)

## Summary

Atmel sells AVR microcontrollers with USB interface preprogrammed with their own bootloader. For some reason neither the binary image nor sources for this bootloader are supplied officially. Microcontrollers come read locked and there is no possibility to read bootloader through ISP by external programmer. Although fuse settings allow to read bootloader section from user program. Here you can get HEX-file of original bootloader extracted from ATmega32U2 with marking:


ATMEL (logo)  

MEGA32U2  

-AU  

0927 084-4 


Now you don't need to fear erasing your flash, as you can restore bootloader by yourself at any time.
