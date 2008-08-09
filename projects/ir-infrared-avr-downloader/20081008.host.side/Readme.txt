AVR IR Downloader - Host Side [20080813]

++++++++++++++++++++++++++++++++++++++++

AVR IR Downloader is one of final assignments at Electrical Engineering Brawijaya University of Malang, Indonesia.
The basic idea was came from our lecturer at campus, Ir. Nanang Sulistyanto.
If this project was successfully made, it will be used to program our automatic machine's uC during the testing schedule.
Actually we are using Atmel AVR series. Automatic machines are part of entire machines we have for Robocon in Indonesia (KRI).
And now we had just made it.... Thanks to our Mighty Allah SWT, Muhammad SAW our prophet, UB Robotics crews and
all Electrical Engineering Brawijaya University of Malang academicians.

The host-side consist of host-program for Win32 PC using Libusb-Win32, device based on ATmega8
using firmware-only USB driver by Objective Development and infrared hardware unit.
Host program was compiled using Lazarus Freepascal. Device firmware was compiled using WinAVR (AVR-GCC).
The implementation uses custom device class, required simple inf file and libusb driver on PC.
Full source code is provided as is without any support.

Infrared data transmission uses USART facility and Timer2 for 38 kHz modulation signal generator.
Baudrate 2400bps is based on the IRM-8510N data reception ability which is 400us for the shortest length logic signal.
With a single infrared led, both host and target side can wirelessly communicate at about 6,5 meters. Final board uses
6 infrared led, can handle 15 meters length approximately. Data packet size was limited 128 byte length because of ATmega8
SRAM was limited at 1 kbyte size. During development phase we planned to increase till 1~2 kbyte length but we avoided it.
Actually it can be increased to optimize the transfer process using a bigger SRAM AVR series. All transmission packets were
implemented using CRC16 for error detection.

Host-Side Features are:
- Device firmware is implemented as custom class device.
- Atmel AVR uC memory operations: read, write and erase.
- Memory supported: flash, eeprom and fusebits (extended also).
- Error detection during transmission through infrared implements CRC16.
- Data packet size 128 byte length, can be increased in future development.
- Can handle data transmission at about 15 meters max.
- usbpoll cannot be run while USART data transmission. USART handling is the highest priority routine to maintain infrared
  communication consistency between host and target side.
- Developed for all Atmel AVR series but still partially tested (different SCK frequency of specific AVR series depend on
  the target-side system).
- Maximum target memory operation can handle is 32640 byte (32 kbyte). It depend on the packet id max. which is 255 with
  data packet size 128 byte length.

Note:
This project (not including usbdrv) is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License (GPL2) as published by the Free Software Foundation.
A copy of the GPL version 2 can be found in the appendix of this document.
This project was intended for open source and not intended for production. It was built for educational purpose.
So many things left and many ideas for future development. But we don't have a spare time to implement it. All of you
may use this code or our ideas freely, by give us some credit "it is partially based on avrirdownloader by Hendy Eka and
Arief Mardhi at Electrical Engineering Brawijaya University of Malang, Indonesia".
We publish this project with the reason, we are not alone in this world. We just a human being which are social creatures.
We aren't perfect, we need you....

Thanks in advance.
Thanks for all inspirational project before till we can realize this.

Host-Side by Hendy Eka Hardana <hendyeka.hardana@gmail.com>
avrirdownloader by Hendy Eka and Arief Mardhi at Electrical Engineering Brawijaya University of Malang, Indonesia.
