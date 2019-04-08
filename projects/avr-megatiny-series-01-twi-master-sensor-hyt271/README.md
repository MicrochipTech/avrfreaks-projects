# AVR Mega/Tiny Series-0/1 TWI Master (Sensor: HYT271)

Uploaded by GermanFranz on 2019-04-08 14:30:00 (rating 0 out of 5)

## Summary

This project contains all the code required for a simple interrupt- based TWI-Master implementation.  

I2C slave of this example is a single HYT271 humidity / temperature sensor.


The timed subroutines (be part of the main program or a cyclic timer interrupt) must be called at a certain time interval:   

Sensor takes some wakeup and response time.  


Measurement cycle starts with MR measurement request command, via DF command new data (4 Bytes) will be fetched later.


For details have look the HYT271 datasheet.


Any communication errors will be 'quick and dirty' responded via TWI disable, then the user have to try new request cycle again.

## Compilers

- AVR Assembler

## Tags

- Part-specific Lib.functions
- AVR Assembler
- ATmega4809
- ATtiny1614
- ATtiny1616
- ATtiny1617
- HYT271
- megaavr 0-series
- tinyavr 1-series
- ATmega4808
- TWI
- I2C Master
