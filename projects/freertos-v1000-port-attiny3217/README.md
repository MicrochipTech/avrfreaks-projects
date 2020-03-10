# FreeRTOS v10.0.0 port to ATtiny3217

Uploaded by heel on 2020-03-10 00:21:00 (rating 0 out of 5)

## Summary

Attached is a port of FreeRTOS v10.0.0 to Attiny3217. Whole project for Atmel Studio 7 (Version: 7.0.2397 ) is attached. Note that it need a series of additional packages to compile, but the tool should tell that.


 


The port is based on the application note: [http://ww1.microchip.com/downloa...](http://ww1.microchip.com/downloads/en/Appnotes/FreeRTOS-on-megaAVR0series-DS00003007A.pdf) which guides to build a START project, but for a ATmega4809.


Simply changing processor in this code to ATTiny3217 is not enough as they have different IO and peripheral addresses. Notably the base address of the TCB0 is different. TCB0's base address is hard coded into the port.c in the form of an assembly statement to handle the tick timer. Once this address is updated to the correct value it seems to work fine. Not all features of FreeRTOS have been verified (Note 1)  but the example using tasks, vTaskDelay and Queue works fine. One thing to watch out for is the line


#define configTOTAL\_HEAP\_SIZE 0x700


in FreeRTOSConfig.h this has not been optimized if this is a safe and optimum value (abs max is 0x800=2k that is for sure)


It has been tested on ATtiny3217 Xplained Pro which is quite handy with USB debugger on board and all pins to connectors.


 


Enjoy.


 


Note 1: I suppose there are test suites which can be run to verify the port more carefully, but this will be for another time.

## Compilers

- Atmel Studio 7

## Tags

- Complete code
- Atmel Studio 7
- ATtiny3217
- FreeRTOS
