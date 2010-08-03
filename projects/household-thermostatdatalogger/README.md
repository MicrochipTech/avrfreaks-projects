# Household thermostat&datalogger

Uploaded by tloimu on 2010-08-02 21:13:00 (rating 0 out of 5)

## Summary

Measures temperature from 8 sensors locating inside and outside of the house and also taped and pasted to heating pipes. Currently, readings from one of the sensor is used for controlling the water pump from my 2000 liter boiler towards the central heating furnice and radiators.


The temperature sensors are DS18B20 on a single 1-wire bus powered by parasite power. Distance to the central processing unit varies from 50cm to 10 meters. The heat transfer to the house is controlled by turning on and off a regular hot water circulating pump via a single optoisolated triac component driven by a output pin from AVR.


Temperatures are taken about every 5 seconds and a log is written to a MicroSD-card (via SPI). Due memory constraings in ATmega168, I had to skip all file systems niceties and take the brute force approach of writing directly to the flash memory disk. Transfer of the log to PC is made simply by taking the SD-card once a month or two from the AVR-board to PC with a regular SD-card-to-USB adapter. To actually read the data from the disk in Windows required me to make a simple card reader program that reads the raw data from data blocks into file on a plain text file (rows of tab-separated columns) on PC.


To view the data graphically in PC, I wrote a generic graphical tabular data viewer application for it. Decided to make it in Java since there is a nice ZUI library (zoomable user interface) called Piccolo for it. The viewer is a simple generic application capable of reading large amounts of tabular data and displaying it on a window and allowing zooming into and out of it and taking selected samples to clipboard for further analysis. The amount of data produced by the system during months and years of running is outrageous - well exceeding the capabilities of Excel or other equivalent spreadsheet applications. Thus, the need for a simple home brewn viewer.


List of strategic components:  

 - AVR ATmega 168  

 - DS1820 (DS18B20) for temperature measurements  

 - An ancient 16 character LED-matrix display and control unit (salvaged in 90s from an age old Olivetti PC-server reject) for the user interface  

 - S202S02 triac for controlling a single regular water circulating pump running at 230V AC  

 - MicroSD-card interface for data storage for logging purposes  

 - A self-made program for Windows for reading logs from the MicroSD-card via USB-adapter  

 - Various code libraries for AVR - mostly stolen from other people's projects (mainly for 1W and SD-card)  

 - Self-made Java application for viewing and analysing the temperature logs (Excel was ok for development phase, until started getting tens of megabytes of data...)  

 - A common 5V power adapter for the AVR board. The AVR-board has a regulator for added safety.


A standard disclaimer:


If you're considering the above configuration, please notice that the triac connects to a water pump that operates at 230V AC voltages. If you are not a qualified electric engineer for working on such circuits, you should get one for doing that connection for you. There are options available e.g. getting an electrician to install an equivalent triac/relay to your central electric cabinet's DIN-rail that provide a low-volatage interface that you are allowed to operate with from that on towards the AVR.


Experiences:


The system has been running since autumn 2008 and has proven to be surprisingly reliable and accurate in controlling the house hold temperature. The problems encountered so far have been 95% related to the MicroSD-card interface (a missing or damaged sample sample row). The 4% has been with the display and control unit's electical characteristics (occasional garbage on screen for a second or two). During 1 1/2 years, has there been one incident where the system has not completely recovered from electricity blackout - and boy have we had many due living in a country side with electric wires in air.


Development tools:


The code is developed in AvrStudio 4.17 with WinAVR 2009 March edition.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
