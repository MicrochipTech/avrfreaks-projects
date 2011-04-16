# Arduino loader for AVR Studio 4

Uploaded by o5o5o5 on 2011-04-16 10:15:00 (rating 0 out of 5)

## Summary

Arduino Bootloader for AVR Studio 4


This software is a bootloader compatible with AVR Studio 4 for  

Arduino UNO/Diecimila/Duemilanove with ATmega168/328. Project  

is based on well known Peter Fleury STK500 v2 bootloader.


To start bootloader connect PIN12 and GND and press RESET on  

Arduino. L led will show that bootloader is started. Then  

connect from AVR Studio, select STK500 or AVR-ISP and enjoy.


To burn bootloader you can use any AVR programmer.  

Don't forget to set BOOTRST fuse and BOOTSZ to 1024 words.


Author: Yuri A. Nikiforov   

Tested with Arduino UNO (ATmega328P)  

Date: 2011/04/16


CHANGE LOG:  

v0.3:  

- traced bug with hw/sw version reporting to AVR Studio


v0.2:  

- merged ATmega168 and ATmega328 projects into one folder


v0.1:  

- initial release

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
