# fast tiny & mega UART bootloader

Uploaded by danni on 2009-03-23 07:58:00 (rating 0 out of 5)

## Summary

After every power on reset, the bootloader wait for about 0.3 seconds for a keyword to enter the bootloader, otherwise the application was started. So it is also possible to use the reset pin as IO on small Attiny or ATmega.


There is no need for a crystal, since autobauding was done prior checking the keyword.  

On frequencies below 8MHz, programming at 115200Baud may fail and you should try a lower baudrate. Baudrates down to 1200baud can be selected. There is a small download program included, which can easy be executed on the end of the make file.  

"fboot.exe -?" tells the usage.  

It must be started prior powering or resetting the AVR.


There are two different programming algorithm implemented:  

1.  

The ATtiny and ATmega without bootstart fuses need to place the RJMP instruction at first. This was replaced by the RJMP to the bootloader and recalculated to be placed one word below the bootloader.  

Especially care was taken against accidentally lockout. Thus erasing was done backward with erasing the page with the bootloader jump at last. So if power fails unfortunately at this moment, all pages are erased to 0xFFFF, which was interpreted like a NOP and thus the bootloader was entered anyway.


2.  

On Atmega equal and above 8kB the bootstart fuse must be set to enter the bootloader.  

For faster download a buffersize bigger than the pagesize was used.


Following devices are tested:


AVR---------user size---programming time  

---------------------------------------------  

ATtiny13----606 Byte----0.77 seconds  

ATtiny2313--1630 Byte---2.86 seconds  

ATtiny45----3646 Byte---3.52 seconds  

ATmega8-----7680 Byte---1.81 seconds  

ATmega16----15872 Byte--3.35 seconds  

ATmega32----32256 Byte--5.33 seconds  

ATmega644---64512 Byte--8.19 seconds  

ATmega2561--261120 Byte--32.75 seconds


Now updated to Version V2.1  

An API-call was implemented to program a Flash page from the application.  

The example "apicall.c" was written for AVR-GCC (WINAVR).  

Error correction for ATmega32 (wrong start address)


Linux version available.


Peter

## Links

- [Linux version from Ilya Goldberg](https://www.avrfreaks.net/index.php?module=Freaks%20Academy&func=viewItem&item_id=1927&item_type=project)

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
