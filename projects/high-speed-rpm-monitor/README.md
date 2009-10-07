# High Speed RPM Monitor

Uploaded by davidrs on 2009-10-07 00:12:00 (rating 0 out of 5)

## Summary

There are many motor speed monitors on the net that look good, but I decided to write my own so I could adapt it to several conditions.  

This one is based on the Mega16 running at 8 mhz with an external crystal and an optical sensor monitoring a slotted disk attached to the motor shaft. The system requires 2 interrupts from the motor to determine the motor speed. The first starts Timer 1, (1 mhz counter with overflow interrupt enabled), and sets up the needed registers. The second interrupt stops Timer 1 and tells Main its time to calculate the RPM. My test results ranged from 5 to 100,000, however higher speeds are possible with accurate results. My target is approximately 30,000 rpm spindle speed.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
