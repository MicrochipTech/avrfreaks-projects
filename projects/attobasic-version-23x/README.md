# AttoBASIC Version 2.3x

Uploaded by ksvitale on 2013-10-13 21:03:00 (rating 1 out of 5)

## Summary

2013-1013: Corrected AT90USB1286 bug. New files posted, no version change. Sorry for the inconvenience.


2013-1012: Bug alert! AT90USB1286, all clock speeds; use of THEN statement causes hardware stack corruption resulting in software lockup with USB serial I/O or software reset upon malfunction. All other builds are unaffected. 


2013-1008: This update includes support for;  

- The NORDIC nRF24L01(+) transceiver  

- The DALLAS 1-Wire bus.  

- The AT90USB1286 and ATmega2560. This includes support for both PJRC's TEENSY2.0++ and ARDUINO Mega2560 boards.  

- Nested GOTO and GOSUB to 4 levels deep.  

- DATA, READ and RESTORE commands to embed data into programs, transfer multi-byte data packets from nRF24L01(+) or 1-Wire devices and read them back.  

- EEPROM file system to save and load programs from on-chip EEPROM.  

- Commands to read/write the on-chip EEPROM.  

- New commands for bit, nibble and shifting of data.  

- Lots of bugs squashed!


Review the "AttoBASIC Revision History" document for details.


Version 2.30 is unreleased.


Project History:  

In 2011, Dick Cappels' AttoBASIC received a face-lift and was ported to the Mega88/168 and Mega32U4. Version 2.0 added additional and improved support for on-chip peripherals but there was no support for the Mega32U4 to use the USB as its serial I/O.


Version 2.1 added PJRC.COM's USB Serial I/O (via Virtual Comm Port) to AttoBASIC to support the ATmega32U4 used on products such as ADAFRUIT's Mega32U4 breakout board. Support for the Arduinos using the ATmega168 and ATmega328 was added to provide folks with a means to use an interpretive language on their Arduino's. Boot-loader, support was also added


Version 2.2 fixes a few bugs and adds a few more features. Mainly support for access to an external EEPROM attached to the SPI port so it can be used as a data file. 


Version 2.22 added support for DHT11/22 low-cost humidity and temperature sensors.


Having data file support led to a natural extension to use AttoBASIC as the heart of a programmable data recorder (see AVR Data Recorder project).


The AttoBASIC home page is hosted on Dick Cappels' web page at [http://cappels.org/dproj/AttoBas...](http://cappels.org/dproj/AttoBasic_Home/AttoBasic_Home.html). One will find earlier versions of AttoBASIC for the AT90S2313 and the ATTiny2313 as well as for the AT90S8515 and ATMega163. Be sure to visit as Dick hosts many other projects, some AVR-based and others not.


In the project ZIP file, one will find pre-built HEX files for the Mega88/168/328, Mega2560, AT90USB1286 and Mega32U4 at clock speeds of 4, 8, 16 and 20MHz. Builds are available with and without the STK500v2, OptiBoot or LUFA bootloaders as well as builds for the AT90USB1286 and Mega32U4 using UART or USB for serial I/O and the Mega328 as a data recorder. The builds without the boot-loader can be uploaded to the target platform, ARDUINO (using avrdude) or TEENSY2.0++ (using PJRC's "Teensy Loader").


AVRFREAKS file upload has file size limitation problems. The original project file is 4.6MB but I had to break it into three (3) separate ZIP files:


1 -> AttoBASIC V2.31 Source Code  

2 -> AttoBASIC V2.31 Pre-compiled HEX Files  

3 -> AttoBASIC V2.31 Support Files - Bootloaders and USB Serial I/O support for Mega32U4 and AT90USB1286.


Create a folder for AttoBASIC. Unzip file #1, #2 and #3 into the AttoBASIC. The resulting folders structure within the AttoBASIC folder should look like this:


\_Applications/  

\_Bootloaders/  

\_USB\_Drivers/  

\_USB\_Serial/  

AVR\_Specific\_Builds/  

Include/ 


Have fun!


Peace,  

Scott

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
