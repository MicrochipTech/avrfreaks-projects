# Eversmith - AVRSMS

Uploaded by mjthomas on 2003-09-23 19:27:00 (rating 0 out of 5)

## Summary

Eversmith (AVRSMS) Version 0.56a


Firmware for ATmegas to send and receive short messages (SMS) in a GSM Network with a GSM Mobile Phone/Cell Phone/Handy connected to the AVRs serial port. The firmware is completely written in AVR-Assembler Code. (You may call it: YAAS - Yet Another AVR SMS Software)


Features:  

* Sends and receives Short Message in SMS PDU-Format. PDU creation and decoding is done on the fly (from UART to Memory), so only one memory area is needed that can be shared.  

* Switches output pins on receive of SMS  

* Send system status via SMS on demand  

* Sends SMS if input pins where enabled  

* Menu-based configuration via Terminal-Software  

* Configuration values are kept in EEPROM storage  

* Debouncing of input keys  

* Buffered UART


For more details see manual.pdf that comes with the project-archive.

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
