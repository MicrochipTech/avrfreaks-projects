
 Title  :   GCC DUAL UART DRIVER
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      30/Aug/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Info:      This code is FREE. There are no copyright issues with this code.



IMPORTANT:  Make sure that you edit the F_CPU field in the makefile to match
            the used MCU clock frequency.
            Also make sure that you edit the AVRGCC_VERSION field in the makefile
            to match the avrgcc version that you use to compile the code.
            330=winavr, 320=version 3.20, 302=version 3.02 etc.
            This is essential in order to include the correct files.
            From version 3.02 (302) and below you can use 302 as the version number.


                          GENERAL INFORMATION
            
This driver is a DUAL hardware uart driver that uses the corresponding interrupts to
transmit and receive data.

The receiver has a fifo type buffer the empties as you read data from it.
If the buffer gets filled with data then receiving stops
and a buffer overun error is issued.

I dont think there is any point keep receiving data if you dont read them.
I have tested it and i think it works ok.

                          GENERAL GUIDLINES

****************************************************************************
* ALL OTHER CONFIGURATION IS DONE IN THE uart.h FILE!                      *
****************************************************************************

****************************************************************************
* IF YOU RECOMPILE FOR A DIFFERENT AVR DELETE ALL FILES FIRST              *
****************************************************************************

****************************************************************************
* SWITCH OFF ALL UNWANTED OPTIONS IN uart.h TO SAVE CODE SPACE             *
****************************************************************************

Feel free to use it and modify it as you please.