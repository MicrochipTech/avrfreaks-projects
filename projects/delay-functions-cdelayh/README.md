# Delay functions cdelay.h

Uploaded by alank2 on 2017-11-02 01:19:00 (rating 4 out of 5)

## Summary

Here is the cdelay.h I use for most of my projects.  It requires defining the symbol F\_CPU with the number of cpu cycles, but then you can do delays in ns, us, ms, or s.


It will always select the next higher delay.


The \_approx functions may add up to 5 more cycles of delay at the savings of flash.

## Compilers

- Atmel Studio 7

## Tags

- Complete code
- Atmel Studio 7
