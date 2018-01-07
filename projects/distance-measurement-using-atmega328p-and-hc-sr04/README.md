# Distance measurement using ATMEGA328p and HC-SR04

Uploaded by abytom87 on 2018-01-06 17:28:00 (rating 1 out of 5)

## Summary

Distance measurement using HC-SR04 and ATMEGA328p. In this project i have measured distance in centimeters, with the help of HC-SR04 Ultrasound sensor, ATMEGA328p micro-controller, LCD Display via I2C bus.


Principle:


Timer2 of ATMEGA328p is used to generate a Trigger pulse of 20uS, the ultrasonic module sends out a 8cycle burst of 40khz which hits the object surface and returns back to raise an echo pulse. The pulse-width of this pulse is proportional to the distance between the module and Object.


Input capture module of the ATMEGA was used to capture the time between rising and falling edges of the echo pulse. The prescaler of this unit was chosen, such that the resolution of pulse-width is 16uS.


The display in use is LCD1604 which has an integrated PC8574 chip that converts serial data (via I2C bus) into parallel stream of bits for the LCD


 


 


Hardware used:


* Arduino-UNO
* HC-SR04 Ultrasonic sensor
* LCD Display with integrated PCf8574T chip enabling serial (I2C from uC) to parallel conversion.
* 2.2k Pull-up resistors for I2C bus.

SW and Tool-chain:


* Software was written entirely in C,
* Developed in Eclipse CDT,
* AVR-GCC compiler was used,
* AVRDUDE was used to program the hex onto the target,
* Operating System - Ubuntu(Linux)

 


![](https://community.atmel.com/sites/default/files/1_68.jpg)


 


 


![](https://community.atmel.com/sites/default/files/2_42.jpg)


 


 


![](https://community.atmel.com/sites/default/files/3_23.jpg)

## Links

- [Distance measurement using ATMEGA328p and HC-SR04](https://github.com/abtom87/Distance-measurement-)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
- #Atmega #avr
- #avr #atmega328p #hc-sr04
