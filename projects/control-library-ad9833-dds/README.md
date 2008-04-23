# Control library for AD9833  DDS

Uploaded by krazatchu on 2008-04-22 23:39:00 (rating 0 out of 5)

## Summary

This is a library for the Analog Devices DDS part AD9833.


The AD9833 is a serial programmable DDS with 28bit resolution.  

With a 25 MHz master, it can output from 0 to 12.5 MHz.  

It has sinusoidal/triangular/square wave outputs.  

It's incredibly small (M-SOP package).  

It has an onboard 10 bit DAC.


Connections are as follows:  

MOSI ==> SDATA  

SCK ==> SCLK  

PB0 ==> FSYNC


Implemented:  

1) an initilize function for SPI with frame sync  

2) an initilize function for the AD9833  

3) a function to write a 16bit word to the AD9833  

4) a function to calculate the frequency and select the register


Not Implemented:  

5) a function to calculate the phase setting  

6) a function to select the output type  

7) ???


That which is not implemented can be achieved using function 3...


Merry Christmas~~

## Compilers

- AVR32 GNU Toolchain

## Tags

- General Lib.functions
- AVR32 GNU Toolchain
