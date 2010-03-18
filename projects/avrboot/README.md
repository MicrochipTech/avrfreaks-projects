# AVRBoot

Uploaded by ptrxtrnd on 2010-03-18 06:18:00 (rating 0 out of 5)

## Summary

AVRBoot is a bootloader for AVR targets. The bootloader is a small application located in the boot section of the target MCU and can be used for software downloads in the field. 


At power up, the bootloader is started and waits for a programming request on the UART. If no request has been received within a certain time, the application already in the target is started.


The current version is tested on ATmega16 only.


The bootloader is accompanied by a PC console program AVR Prog, that communicates with the bootloader during the software download.


Instructions and screenshots are available on the project homepage: [http://sites.google.com/site/coc...](http://sites.google.com/site/cocoosorg/)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
