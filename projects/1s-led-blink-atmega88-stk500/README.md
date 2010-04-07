# 1s LED Blink - atmega88 + STK500

Uploaded by aadishri_gs on 2010-04-07 08:35:00 (rating 0 out of 5)

## Summary

A simple LED blinker which blinks LED1 of STK500 using timer of Atmega88MC  

Here, the output pin PB1 is connected to LED1. The 8-bit timer is used and a counter is maintained to count the TOV interrupts until 1s has elapsed. This count is based n the oscillator freq. Once the counter reaches the threshold, it sends a signal to pin PB1 which lights the LED

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
