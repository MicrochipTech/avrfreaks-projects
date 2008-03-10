# AVR DDS signal generator V2.0

Uploaded by mindaugasu on 2008-03-10 13:16:00 (rating 0 out of 5)

## Summary

AVR DDS signal generator V2.0 is a firmware based DDS signal generator which uses slightly modified Jesper’s DDS algorithm adapted to AVR-GCC C program as in-line ASM. Signal generator has two outputs - one for DDS signal and another for high speed [1..8MHz] square signal - which may be used for reliving microcontrollers with wrong fuse settings or for other purposes as well. High speed (HS) signal is direct output from Atmega16 OC1A(PD5) pin. DDS output is used for all other signals that are generated via R2R resistor network and is adjusted via LM358N offset and amplitude regulating circuits. Offset and amplitude can be regulated by two potentiometers. Offset can be regulated in range +5V..-5V while amplitude in range 0..10V. DDS frequency range is from 0 to 65534Hz that is more than enough for testing audio circuits and other tasks.

## Links

- [AVR DDS signal generator V2.0](http://www.scienceprog.com/avr-dds-signal-generator-v20/)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
