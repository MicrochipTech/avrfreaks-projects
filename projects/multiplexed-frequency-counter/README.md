# Multiplexed Frequency Counter

Uploaded by angmar on 2006-06-01 14:51:00 (rating 0 out of 5)

## Summary

In research areas some applications uses crystal oscillators as a sensor that the response of frequency is linked with some property. This project is a simple hardware and software to monitoring the difference of frequency by one selected oscillator and a reference oscillator. The project is very simple and it is possible to adapt the hardware and software to different applications.


A AT90S2313 microcontroller performs three different operations: select an oscillator, count pulses and display results.


The selection of the oscillator takes place using two I/O lines from 2313 connected to a 74HC153 (dual 4-input data selector/multiplexer). In this case, only one 4-input multiplexer was used and the other one was disable. The pulse counter was obtained using the 2313 configured as a 16-bits counter. The difference of frequency is performed by a 74HCT74 (dual D flip-flop with set and reset). The difference is displayed using a HD44780 LCD compatible. The crystal oscillators was make using the 74HCT04 (hex inverter) and quartz crystals. The reference oscillator was a clock oscillator (XO) with the frequency very close with the selectionable oscillators.

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
