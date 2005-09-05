# AVR controlled USB<>CAN Bus Interface

Uploaded by Mr MIC on 2005-09-05 14:43:00 (rating 0 out of 5)

## Summary

The purpose of this project is a simple and easy to use USB <> CAN bus interface. Heart of the circuit is an Atmel AVR ATmega162 microcontroller which controls the communication between FTDI FT245BM USB to parallel converter and Philips SJA1000 Stand-alone CAN bus controller. The SJA1000 is memory mapped into memory space of the AVR, which reduces data and address line handling and increases data transfer speed. An FT245BM handles all USB communication and provides data transfer from and to PC application. An extra EEPROM can store the multi device template for the FT245BM. A Philips PCA82C250/251 works as CAN controller interface.  

Firmware is written in C, compatible to latest compiler version WinAVR.

## Links

- [Project Homepage](http://www.mictronics.de)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
