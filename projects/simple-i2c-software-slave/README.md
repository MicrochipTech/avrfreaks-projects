# Simple I2C Software Slave

Uploaded by kinkajou on 2012-04-13 17:53:00 (rating 5 out of 5)

## Summary

"C" realization of I2C (TWI) slave protocol on any AVR device without I2C hardware module, using only one hardware interrupt (easily can be changed to pin-polling scheme).  

No assembler code, except \_delay\_us() from <avr/delay.h>


Engine + sample program provided.


Tested in real system (shortwave tranceiver) with several boards based on ATtiny84 running on internal RC clock at 8MHz, SCL speed up to 50 kHz (may be higher - my board use very weak pull-ups on both lines) achieved.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
