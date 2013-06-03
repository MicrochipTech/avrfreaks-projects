# attiny24/45/85 serial bootloader

Uploaded by flenser on 2013-06-03 15:02:00 (rating 0 out of 5)

## Summary

This is my port of the t2313bl v0.8 bootloader by Arne Rossius to the attiny25/45/85.  

Arne's bootloader is at [http://elektronik-kompendium.de/...](http://elektronik-kompendium.de/public/arnerossius/schaltungen/avr/tn2313bl.htm). This page is in German but google gives a good translation and his bootloader code is in English.


My port includes these changes:  

- It uses a modified version of the AVR305 Software UART.  

- I use the watchdog timer to exit the bootloader and jump to the application.  

- I've added support for the AVR910 blockmode commands. Using these makes the bootloader a little bigger but up to 6x faster so i's a great option for the attiny45/85. Using the blockmode commands at "ludicrous" speed (aka 230400 baud using the internal oscillator at 8MHz) makes programming impressively fast on these small memory attinys.  

- You can still compile using just the bytemode commands to get the smallest possible bootloader for the attiny25.


v1.0 Initial release  

v1.1 Disable the WDT in the initialization section. This needs to be done before jumping to the appln.

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
