# TWI (I2C) Example using Attiny 26 as Slave

Uploaded by Huey on 2004-04-25 21:57:00 (rating 0 out of 5)

## Summary

This example shows how to use the Attiny26 as a slave for I2C. I used a mega16 as a master programmed with i2c.test from [http://hubbard.engr.scu.edu/embe...](http://hubbard.engr.scu.edu/embedded/avr/avrlib) Received data is inverted and displayed on PORTA of the tiny26. If no data is present PORTA will scroll back and forth. The data received is sent back when a Master requests data from the tiny26 slave. The entire I2C software is interrupt driven thus keeping the processor free for other tasks.

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
