# AVR-Butterfly Application gcc-port

Uploaded by mjthomas on 2006-08-25 15:52:00 (rating 0 out of 5)

## Summary

Version 0.6.5 (**Download-Link at end of description**)


The AVR Butterfly comes with a preloaded application. The application source-code can be downloaded from the ATMEL Web-Site (AVR Butterfly - Application Rev06). It is written for the IAR EWAAVR 2.28a C-Compiler.  

This is a port of the IAR code to avr-gcc using the avr-libc. If you plan to use major parts of the ported code in own projects please make sure to keep some notes in the source-files that it is/parts are ported code based on ATMEL source to avoid copyright problems.


The Atmel code includes lots of infomartion how to access various functions. This is now available for gcc i.e.:


* LCD access with scrolling (same as STK502)
* Menu-System and State-machine which can be extended easily with own functions
* Real-Time clock/date
* Sound ("Beep-Music") Playback via PWM
* calibrated System clock from 32kHz Osc.
* Dataflash via SPI
* ADC for LDR and Temperature-Sensor
* ...

In addition information how to convert source-code for IAR to avr-gcc can be found in the code (includes, registers, working around the IAR \_flash directive with progmem.h functions, etc.) And you get two more songs, which were commented out in the ATMEL code because of size restrictions but fit in the AVR memory if compiled with avr-gcc without loosing any functions of the original code.


Please use at least the Jul. 2004 edition of WINAVR to compile and link the port version 0.6.5 code. 


[[Homepage](http://www.siwawi.arubi.uni-kl.de/avr_projects/#bf_app)]


 [[Download](http://www.siwawi.arubi.uni-kl.de/avr_projects/bf_gcc_20060128.zip)]

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
