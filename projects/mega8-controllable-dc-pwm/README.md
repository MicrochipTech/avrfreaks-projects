# Mega8 Controllable D/C PWM

Uploaded by slipnfall on 2006-08-29 00:07:00 (rating 0 out of 5)

## Summary

My first project with an AVR Mega8: intended to familiarize myself with PWM, interrupts, and general AVR assembly.


A fairly simple program to control the duty cycle of the 16bit Timer1, configured as fast PWM. Starts by outputting a 10kHz, 50% PWM on OC1A. Two pushbuttons connected to the two external interrupts allow the duty cycle to be increased or decreased. The ISR's simply increment or decrement an 'OutLevel' variable, which ranges from 1 to 10, or 10% to 100% duty cycle output. The main loop looks for the difference between the 'LastLevel' and 'OutLevel' to determine if the OCR1A register needs updated.


There is a very basic 'range validation' check that needs some finesse, but it works OK for now. The result is that you can get closer to 100% than you can 0%. 


Source is well commented and should be for the most part, self-explainatory!


Rev1.1 adds ramping between level/DC changes

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
