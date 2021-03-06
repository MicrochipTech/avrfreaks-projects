# FreeRTOS port to CodeVisionAVR 2

Uploaded by admin on 2009-03-20 18:56:00 (rating 0 out of 5)

## Summary

Target chip: ATmega128 running at 8MHz  

on STK500+STK501 boards


In order to compile the demo, open in CodeVisionAVR the  

project file \Demo\AVR\_ATMega128\_CVAVR\FreeRTOSDemo.prj  

and do Project|Build All  

After the succesful build, press the Program Chip button in the Information window.


Setup:


The ATmega128 chip must pe placed in the ZIF socket of the STK501 addapter board.  

The STK501 addapter board must be connected to the EXPAND0 and EXPAND1 connectors of the STK500 board.


STK500 Jumpers:  

 VTARGET: ON  

 AREF: ON  

 RESET: OFF  

 XTAL1: ON  

 OSCSEL: 2-3  

 BSEL2: OFF  

 PJUMP: OFF  

 DATAFLASH: OFF


An 8MHz crystal must be placed in the CRYSTAL socket


Make the following connections:  

STK500 ISP6PIN - STK501 SPROG  

STK500 PORTC - STK500 LEDS  

STK501 PORTE.0 (header pin1) - STK500 RS232 SPARE/RXD  

STK501 PORTE.1 (header pin2) - STK500 RS232 SPARE/TXD


STK500 RSR232 SPARE: connect pin 2 with pin 3 (loop-back connector)


STK500 RSR232 CTRL: connect to the PC COM port


In CodeVisionAVR Settings|Programmer menu select Programmer type: STK500 and the COM port connected to STK500 RS232 CTRL 


More details about the demo at: <http://www.freertos.org/>  

Please note that the original demo uses PORTB for LED connections.  

In the CodeVisionAVR demo PORTC is used instead.


Please note that for older versions of CVAVR V2,  

you will have to do some small changes in the program.  

We have tested the port ONLY with the current CodeVisionAVR V2.04.0


Enjoy!


[www.hpinfotech.com](http://www.hpinfotech.com)

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code
- CodeVisionAVR-Standard
