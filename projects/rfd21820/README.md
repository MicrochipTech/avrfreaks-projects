# RFD21820

Uploaded by MarkThomas on 2012-08-23 14:09:00 (rating 0 out of 5)

## Summary

This is example code built in AVR Studio 5.1 for the new RFDigital RFD21820 radio transponders that is enough to get one going using these modules, which are pretty cool. The ones I got have an ATmega164P integrated with the radio transponder. This project simply lights up the user defined LED for 5 seconds when the user defined button on one is pressed. After 5 seconds the LED goes out and the module transmits a byte to the other module, which then lights up its LED for 5 seconds and retransmits, so the two modules light up their LEDs for 5 seconds back and forth.


They are pretty cool little modules that were programmed with my AVRISP mkII, which plugs right in to the 6 pin connector on the modules. The RFDigital website has all the info.


It took a while to get these things working, as the sample code was built with a different compiler. The support team at RFDigital was super helpful at getting the things working using AS5.1.


Sorry if this sounds like a marketing spiel, but I am pretty jazzed they are working and I have example code I can go forward from.

## Compilers

- AVR Studio 5

## Tags

- Complete code
- AVR Studio 5
