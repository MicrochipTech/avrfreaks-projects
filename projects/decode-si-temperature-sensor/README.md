# Decode SI temperature sensor

Uploaded by jremington on 2012-04-04 18:21:00 (rating 0 out of 5)

## Summary

This code, functional on an ATtiny26, works with a 433 MHz receiver to decode the weather station remotes marketed by Sharper Image. No model number is apparent on either the sender or desktop receiver (which includes a WWVB "atomic clock"), so the code may be general for Sharper Image remotes. The RF encoding scheme is simple PWM, forming a 28 bit message with a CRC4 checksum, unlike the more popular Oregon Scientific or LaCrosse weather station sensors. See comments in the code for an explanation of the coding/decoding algorithm.


The ATtiny26 outputs the code word and the temperature value via a software UART to a TTL-serial 20x2 LCD display.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
