# Stepper motor drive for telescope

Uploaded by rih on 2016-05-24 17:22:00 (rating 0 out of 5)

## Summary

The drive box described controls an equatorially-mounted telescope with worm drives on the RA and Declination axes. Each worm drive is fitted with a bipolar stepper motor with a planetary gearbox.


The box also reads a pair of 5000 pulse-per-revolution incremental encoders.


The box is controlled via the RS232 port. A hand controller is also supported.


The box is built around an Atmel AVR ATMEGA168 microcontroller running at 16MHz.


In the absence of commands from the PC, the box runs the RA stepper motor at sidereal rate plus a fixed periodic error correction stored in the flash memory.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
- ATmega168
- stepper motor controller encoder telescope
