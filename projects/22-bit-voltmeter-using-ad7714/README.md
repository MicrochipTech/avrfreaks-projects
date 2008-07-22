# 22-bit voltmeter using AD7714

Uploaded by dstahlke on 2008-07-22 05:14:00 (rating 0 out of 5)

## Summary

This project contains reusable code for interfacing with an AD7714 22.5-bit ADC chip. The reading process may optionally be done via the INT0 interrupt which is triggered when the ADC's DRDY pin goes low.


There is also a driver for an LCD panel operating in 4-bit mode driven from the SPI port using a 74HC595 shift register. The LCD code was based upon a driver written by G. Burger for the PIC microcontroller.


A pair of rotary switches are used to navigate menus which allow setting of most parameters on the ADC chip.


A Ruby script is included for visualizing the measurements on the computer strip-chart style (Linux only).

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
