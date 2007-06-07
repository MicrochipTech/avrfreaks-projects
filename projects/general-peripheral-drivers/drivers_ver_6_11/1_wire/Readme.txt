/****************************************************************************
 Title  :   DS18X20 FUNCTIONS library 
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      21/Jan/2003
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Info:      This code is FREE. There are no copyright issues with this code.


*****************************************************************************/

IMPORTANT:  Make sure that you edit the F_CPU field in the makefile to match
            the used MCU clock frequency or define it in the makefile.


This code is a DS18X20 1 wire temperature sensor driver 
The code is compatible with the varius releases of gcc (tested from ver 3 and up)
and it is implemented as close to the DS18X20 datasheet as possible.
The driver presented here works fine to my knowledge.
I have tested it using 1, 3 and 5 ds18b20 devices connected to the same 1 wire bus.
I didn't had a single communication error.
The rom codes are CRC8 verified from the one wire driver.
 
The test program itself is a fine thermometer application.

The operation of the test program follows the plug and play (pray) concept.
It will search the 1 wire bus for ds18x20 devices and if any are found
they are stored in the eeprom.The eeprom is written only in the below cases:
1) After flashing the code 
2) If a change is sensed in the 1 wire bus. The code will sense
   if something is damaged, added, or removed and any change in the bus in general
   and readapt the settings and the eeprom contents.

Feel free to modify it if you wish but give me a note if you wish
so i can find out if anything is wrong.

I hope it can help someone.
Chris.
