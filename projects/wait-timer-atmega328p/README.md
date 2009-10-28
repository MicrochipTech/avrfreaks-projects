# Wait Timer for ATmega328P

Uploaded by ricalvarez on 2009-10-28 15:13:00 (rating 0 out of 5)

## Summary

I'm a n00b. I have a background in OO for enterprise applications and have been doing n-tier work in Java for at least a decade, maybe longer. This is the result of my early dabblings into AVR for motion control.


I know, it's probably already out there - but sometimes re-inventing the wheel is the best way to learn things for me.


I needed a way to get a ms-precise wait mechanism in order to space pulses out far enough apart to avoid my stepper just sitting there buzzing instead of actually turning.


This wait mechanism allows me to select my frequency and scaler appropriately at compile time without having to rewrite any of the code as long as I can use Timer/Counter1 and the appropriate pre-processor flags.

## Compilers

- AVR Studio 4

## Tags

- Part-specific Lib.functions
- AVR Studio 4
