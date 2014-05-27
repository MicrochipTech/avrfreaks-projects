# AVR Console-commandline

Uploaded by cprhodin on 2014-05-27 04:19:00 (rating 0 out of 5)

## Summary

AVR Console with Ragel command interpreter and three commands implemented.


Target device(s): ATmega328P Arduino Nano


Resources: USART0, pins, interrupts 3.8KB ROM 127B RAM


Toolchain: gcc version 4.7.2 (AVR\_8\_bit\_GNU\_Toolchain\_3.4.2\_939) avr-libc-1.8.0


Programming tool: AVRISP mkII


How to build (Linux 64-bit system):


$cd librb


$make


$cd ..


$make


$make burn # Flash an Arduino Nano with avrdude.


How to run (Linux 64-bit system): The default baud rate is 250K. The baud rate is set by the preprocessor variable "BAUD".


$setserial /dev/ttyUSB0 spd\_cust divisor 96 # make 38.4K into 250K


$minicom -D /dev/ttyUSB0 -b 38400


Type "help" for a list of commands.


Complete source code can be downloaded from Github.  Use the command: git clone -b console-commandline [https://github.com/cprhodin/avr-...](https://github.com/cprhodin/avr-console.git)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
