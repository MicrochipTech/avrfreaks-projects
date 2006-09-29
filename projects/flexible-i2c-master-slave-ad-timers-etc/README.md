# Flexible I2C Master-Slave with A/D, Timers, etc.

Uploaded by rc_campbell on 2006-09-28 19:43:00 (rating 0 out of 5)

## Summary

This project is an attempt to figure out a completely reliable master/slave I2C driver for the AVR microcontrollers. The Atmel application notes refer to this being possible, but only implement, rather poorly, separate master and slave examples.


I have put together a set of AVR code using the TWI that implements a fairly general purpose and flexible I2C interface capable of master and slave transactions. This code is a conglomeration of stuff used in real products, and while I’m happy to share this and for others to play with it and use it, I have a job and may not be able to do consulting on other people’s applications.


On the master side, simple register based functions can be called for the simple transaction types such as byte and word reads and writes, or the API to the ISR can be used directly using the “polled” API routines as an example.


The Slave interface implements some GIR registers (Read only Status), A/D conversion registers (Read only), Control Registers (R/W with special actions, flags, and side effects), and some RAM (R/W). These types of behaviors are implemented as examples to attempt to bribe the user community to implement general purpose I2C interface features in AVRs and therefore pressure Atmel to help us figure out how to make it reliable.


A serial port is used to implement a very simple monitor program that allows reading and writing over the I2C bus as well as printing some values.


The code is all written in assembly for AVR Studio 4 as I could just not deal with the C compiler issues surrounding the low level hardware and interface stuff. I understand this is not an optimal approach, but code is code and I don’t care very much what I write in. Don’t be too intimidated, it’s not that tough.


This code will run fine on an ATMega128 on the STK500 + 501 board, but may take a little porting for other parts.


A document file included in the archive details the code and functions.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
