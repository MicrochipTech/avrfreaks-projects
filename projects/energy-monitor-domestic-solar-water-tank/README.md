# Energy Monitor for Domestic Solar Water Tank

Uploaded by joachimbey on 2015-01-02 15:50:00 (rating 0 out of 5)

## Summary

This monitor runs a single wire bus with 20 nos DS1820 attached to a domestic water tank to monitor the stored solar energy (KWh) to see whether it is sufficient ( or the main heating is required). Power (KW) is computed to show an increase or drop. Data are stored to compute and display a trend over time.


An I2C LC Display shows all temperatures and total energy stored. Data are logged in a memory stick by a USB Host controller via UART for analysis. Various test modes are included also without devices on the bus. RTC and scan rate can be set with soft keys. Single devices can be read out for the addresses to update the look up table for a new set. A schematic with all interfaces as well as JTAG is included. The set of source files is complete.


Joachim Beyer  [achimbey@kabelbw.de](mailto:achimbey@kabelbw.de)

## Compilers

- AVR Studio 5

## Tags

- Complete code
- AVR Studio 5
- Atmega 32
