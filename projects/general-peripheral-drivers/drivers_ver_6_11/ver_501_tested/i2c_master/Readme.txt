/****************************************************************************
 Title  :   I2C FUNCTIONS library 
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      8/Sep/2002
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


I searched for a I2C driver but after trying varius codes
i decided to write my own.
The reason was that it was difficult to find code compatible with
the varius releases of gcc and also working as close by the book as possible.
The driver presented here works fine to my knowledge.
For usage help have a look at the srf08 driver (srf08.c)
This code follows as close as possible the I2C FAQ found in the Internet.
(a copy is provided in this archive)

Feel free to modify it if you wish but give me a note if you wish
so i know what i did wrong.

I hope it can help someone.
Chris.
