# ATmega328P ASF external interrupt and timer driver

Uploaded by sourstar on 2015-04-06 18:11:00 (rating 0 out of 5)

## Summary

I started playing with ATmega328p-XMINI some days ago and I noticed that ASF


does not have an external interrupt driver for this processor, so I wrote it.


I did not tested all the possible external interrupts, however I'm pretty sure it's compliant with the datasheet.


 


Then I started writing also a driver for the timers.


The driver allows the full configuration of all the timers,


however I tested it only for normal mode (yet).


 


The project simply start/stop LED0 toggle any time SW0 is pressed.


SW0 is monitored via external interrupt driver, then LED0 is toggled via timer driver.


The project is very simple and has the only scope to show the drivers' usage.

## Compilers

- Atmel Studio 6

## Tags

- Complete code
- Atmel Studio 6
