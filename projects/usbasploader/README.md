# USBaspLoader

Uploaded by matrixstorm on 2013-09-12 10:39:00 (rating 0 out of 5)

## Summary

If you like USBaspLoader, please give it a 5star rating here on AVR freaks and a star on github. Thank you.


URL: <https://github.com/baerwolf/USBaspLoader>


A demo-board (tinyUSBboard) using USBaspLoader can be found under: <http://matrixstorm.com/avr/tinyusbboard/>


USBaspLoader is an USB boot loader for nearly all 8 bit AVR microcontrollers (even without hardware USB). It can be used on most AVRs with at least 2 kB of boot loader section, e.g. the popular ATMega8.


It is faster than ISP and can preserve the lifetime of the internal flash memory.  

It supports a huge variety of AVRs and implements a huge amount of features.  

One of them is the ability to provide updates without reflashing the bootloader via ISP/HVP.  

Unlike other VUSB bootloaders (i.e. BootloadHID), USBaspLoader has the advantage to be able to also read the flash, the fuse- and lockbits, and to read/write the eeprom, too.


The bootloader-firmware is flashed into the bootloader-section of the flash memory and takes control immediately after reset. If a certain hardware condition is met  

(this condition can be configured, e.g. a jumper), the boot loader waits for  

data on the USB interface and loads it into the remaining part of the flash  

memory. If the condition is NOT met, control is passed to the loaded firmware.


This boot loader requires no separate command line tool to upload the data.  

USBaspLoader emulates Thomas Fischl's USBasp programmer instead. You can thus  

use AVRDUDE to upload flash memory data (and if the option is enabled) EEPROM  

data.


Since USBaspLoader cooperates with AVRDUDE, it can be used in conjunction with  

the Arduino software to upload flash memory data.


To individualize your USBaspLoader just edit (before compiling) Makefile.inc to configure your device and frequency it uses. Depending on the remaining flash-memory in your MCUs bootloader section you also can switch on/off various features there. You also might want to edit firmware/bootloaderconfig.h to adapt layout, change the bootloader condition or influence features.


Please also checkout (and test) the new features within the testing branch:  

<https://github.com/baerwolf/USBaspLoader/commits/testing>

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
