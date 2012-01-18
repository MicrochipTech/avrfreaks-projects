# Adjust intensity LED by potmeter (ASM)

Uploaded by RES on 2012-01-18 13:19:00 (rating 0 out of 5)

## Summary

This code converts an applied voltage (potentiometer) on the ADC of an ATtiny26 AVR microcontroller to a PWM signal for an LED. Refresh rate = approx. 56 Hz Resolution = 4 bit (16 intensity levels) Pulse-Pause ratio = 1:12000 Duty-cycle = T / 2 - P Range Pulse Width = 0 to T / 2 (0 = off, T/2 = bright) Connect a 10k - 50k potmeter to ADC0 pin PA0 and an LED + 75 Ohm to PA1.

## Compilers

- wavrasm

## Tags

- Complete code
- wavrasm
- ATtiny26
- led potentiometer adjustment
