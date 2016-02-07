# Modbus RTU Slave Library for the avr

Uploaded by mbrugg on 2016-02-07 13:55:00 (rating 5 out of 5)

## Summary

Hello everyone,


in a recent project I needed Modbus capabilities. I was not satisfied with the libs out there; so I wrote my own:


 


The following function codes have been implemented:


* Read Coils
* Read Discrete Inputs
* Read Holding Registers
* Read Input Registers
* Force Single Coil
* Force Single Register
* Force Multiple Coils
* Force Multiple Registers

 


General features are:


* Mostly interrupt driven
* includes example project for Atmega88
* uses <2,9k flash with all functions used
* uses <1,5k flash with coils/discretes only
* supports single and multiple device addresses => can be used to implement gateway devices
* can be used in half duplex RS485 networks and in RS232

 


I hope It's useful for someone out there.

## Links

- [https://sourceforge.net/projects/yambsiavr/](https://sourceforge.net/projects/yambsiavr/)

## Compilers

- Atmel Studio 6

## Tags

- General Lib.functions
- Atmel Studio 6
- #atmega88
- #Modbus #RTU
