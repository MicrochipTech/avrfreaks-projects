# Ultrasonic Resonant Tracking

Uploaded by krazatchu on 2008-06-12 08:00:00 (rating 0 out of 5)

## Summary

This is a resonant driver for an Ultrasonic transducer..


It measures the phase angle between Current and Voltage and adjusts the frequency for maximum power transfer...


Input:  

It measures the zero crossing of phases with an LM393 comparator...  

Optional Precision AC to DC convertor for measuring power... (4558)  

Input comes from a current sense resistor in series to ground with the transducer...


Signal Output:  

AD9833 generating sine wave, control thru SPI...  

LM833 acting as Offset and Gain control...


Data Output:  

RS232 @ 56k ...


Core:  

It uses an ATMega8-16 @ 16Mhz for capture and control...


Notes:  

This circuit requires an amplifier to drive the transducer...  

It has been tested with a 200w welding transducer @ 28KHz..


Future modifications:  

The AD9833 may be replaced with a PWM output for driving a half bridge...


Happy birthday!!


EDIT (2008/6/12):  

This project reqires uart.c and uart.h ...  

The uart library is from Peter Fleury v 1.7.2.5 2005/08/14 ...

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
