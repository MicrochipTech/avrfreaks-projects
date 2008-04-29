# Onewire lock system

Uploaded by LoneTech_ on 2008-04-29 14:38:00 (rating 0 out of 5)

## Summary

I and a fellow student developed this program, and the corresponding hardware, to handle locks for some cupboards in the university.  

As students come and go, the function to wipe the permitted keys in their absence was required, and the interface was to be kept simple. Registered keys are to unlock all doors until a button is pressed, and there are management functions to add or remove single keys as well as reset the whole list.


The hardware is built around an ATtiny84, which was total overkill except for the I/O lines. As we wanted to control up to 8 solenoids, we used a serial shift register, which is always latched (transparent). This is used when unlocking to pull hard with the solenoids with some transistors; one extra transistor is used with a PWM to apply a lower current to keep them open.

## Links

- [Source code](http://donkey.dyndns.org/~yann/onewire.zip)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
