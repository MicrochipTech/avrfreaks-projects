# avr-edid-spoofer

Uploaded by ericwazhung on 2014-05-07 05:16:00 (rating 0 out of 5)

## Summary

This essentially turns an AVR ATtiny45 (or any AVR, with slight modification) into an EDID chip for displays which don't have them, or for creating your own timings, etc. 


It can also make use of the i2c DDC bus for other communication purposes. (Currently, three outputs are used to "fade" three-color LEDs across the rainbow. The EDID's DDC bus can be used to send commands to control their brightness).


Automatically generates EDID data-values based on timings as usually viewed/modified in a computer: H/V Front Porch, H/V Sync Width, H/V Back Porch (rather than "H/V Blank Period" as its stored in the data-array). The checksum is automatically generated, as well, upon boot.


Contains extensive notes re: EDID parameters, etc.


[https://code.google.com/p/avr-ed...](https://code.google.com/p/avr-edid-spoofer/)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
