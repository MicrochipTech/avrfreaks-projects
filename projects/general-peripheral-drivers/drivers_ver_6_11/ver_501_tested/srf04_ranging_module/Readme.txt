/****************************************************************************
 Title  :   SRF04 RANGING MODULE FUNCTIONS library 
 Author:    Chris Efstathiou hendrix@otenet.gr
 Date:      20/Aug/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Info:      This code is FREE. There are no copyright issues with this code.

*****************************************************************************/

IMPORTANT!  Make sure that you edit the F_CPU field in the makefile to match
            the used MCU clock frequency.
            Also make sure that you edit the AVRGCC_VERSION field in the makefile
            to match the avrgcc version that you use to compile the code.
            330=winavr, 320=version 3.20, 302=version 3.02 etc.
            This is essential in order to include the correct files.
            From version 3.02 (302) and below you can use 302 as the version number.

I use it to drive the SRF04 ultrasonic ranging module
The configuration is done from within srf04.h file and it's quite simple.
You can connect up to 4 SRF04 UNITS.



Chris.
