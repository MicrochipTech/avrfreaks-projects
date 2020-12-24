# Simple 7-segment demo with ATmega4809 Curiosity Nano board

Uploaded by CmdrZin on 2020-12-24 06:12:00 (rating 0 out of 5)

## Summary

This is a simple demo with the ATmega4809 Curiosity Nano development board, but can easily be ported to other devices.   


The demo consists of a the board and a common anode 7-segment LED plus current limiting resistors. All of this can easily fit on a protoboard.


![Schematic 7-segment display demo](https://community.atmel.com/sites/default/files/Test7segment.png "7-segment demo schematic")


The example code shows how to perform timed events without using delay() and how to use FLASH (program memory) to store patterns that never change. This saves RAM space.

## Compilers

- Atmel Studio 7

## Tags

- Complete code with HW
- Atmel Studio 7
