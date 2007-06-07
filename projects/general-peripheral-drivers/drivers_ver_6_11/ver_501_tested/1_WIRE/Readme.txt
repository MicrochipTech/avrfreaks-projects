/****************************************************************************
 Title  :   1 WIRE FUNCTIONS library 
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      21/Jan/2003
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Info:      This code is FREE. There are no copyright issues with this code.


*****************************************************************************/

IMPORTANT:  Make sure that you edit the F_CPU field in the makefile to match
            the used MCU clock frequency.
            Also make sure that you edit the AVRGCC_VERSION field in the makefile
            to match the avrgcc version that you use to compile the code.
            330=winavr, 320=version 3.20, 302=version 3.02 etc.
            This is essential in order to include the correct files.
            From version 3.02 (302) and below you can use 302 as the version number.


I searched for a 1 WIRE driver but found almost nothing for Gcc
so i decided to write my own.
The code presented here is compatible with the varius releases of gcc
For usage help have a look at the DS18X20 driver (ds1820.c)
This code follows as close as possible the 1 wire protocoll.
The rom codes that are found using the supplied ow_search_rom() function are
CRC8 verified before they are passed to the rest of the program.

Feel free to modify it if you wish but give me a note if you wish
so i know what i did wrong.

I hope it can help someone.
Chris.
