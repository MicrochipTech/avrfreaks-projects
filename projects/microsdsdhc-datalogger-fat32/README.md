# microSD/SDHC Datalogger (FAT32)

Uploaded by DHARMANI on 2011-05-11 05:02:00 (rating 5 out of 5)

## Summary

Hi friends,


aim of this project is to present a way to store a large quantity of data into microSD card in files with FAT32 format. Here, ATmega32 is used for data collection and for microSD interface. The data is received from in-build 8-channel ADC of ATmega32. One channel is used for reading temperature from LM35 sensor and remaining channels are used for simply reading voltages and storing them.


This project can be used to interface 8 different sensors with ADC of ATmega32, similar to the LM35 used here. The data is stored in CSV (comma separated values) format with date & time stamp, which can be read using a PC/Laptop with Microsoft Excel or other compatible software.


The project contains RTC interface (for date and time storage), RS232 (for connection with PC) and a microSD module. Here, the hyper-terminal connection is required only for setting RTC date and time. Once the date/time are set, the RS232 connection is not required anymore for normal data-logging operation (It can be used for debugging purpose if there is a problem).


A complete operation of the circuit, schematic in PDF and EAGLE format and AVRStudio (winAVR) project source code is available for download at the webpage link given below:


[microSD ATmega32 Datalogger](http://www.dharmanitech.com/2011/05/microsd-atmega32-datalogger.html)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
