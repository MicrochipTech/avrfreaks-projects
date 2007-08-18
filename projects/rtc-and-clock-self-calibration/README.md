# RTC and clock self-calibration

Uploaded by ltdor on 2007-08-17 23:00:00 (rating 0 out of 5)

## Summary

This program running on an ATMEGA8 exhibits:  

 1. a fast reliable self-calibration procedure of the internal RC  

 oscillator when using a 32768Hz crystal with any supply voltage  

 2. a routine written in assembler for determining the MCU speed  

 in the above conditions (\_Get\_MCU\_Speed)  

 2. a way to achieve a high serial baud rate of 115200bps when the MCU  

 is clocked at very low speeds for preserving battery power  

 3. a RTC interrupt routine written in assembler which doesn't affect  

 any register and takes only 67 words from the flash memory.  

 The RTC keeps the right time and date from 2001 to 2099(\_isr\_T2OVF).

## Compilers

- BASCOM-AVR

## Tags

- Complete code
- BASCOM-AVR
