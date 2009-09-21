# Whole House Energy Monitor

Uploaded by Cliff58 on 2009-09-21 11:30:00 (rating 0 out of 5)

## Summary

This device works in conjunction with electronic digital power meters to show how much power (in kilowatts, kW) the home is using at any particular time. It also shows the cumulative number of kilowatt-hours (kWh) used since being plugged in.


It uses an Atmel AVR ATtiny461 microcontroller.


Most of the newer electronic digital power meters have a light emitting diode (LED) built in that emits a flash of infrared light for each one watt-hour of electricity used. The LED is included in the meter for use with automatic calibration equipment, but it can also be used to monitor the rate of usage of electrical energy.


This Home Power Monitor uses an infrared phototransistor to detect the flashes of light from the meter and a microcontroller to compute the power (kW) and energy (kWh) used. kW or kWh readings are shown on a 4 digit LED display.


This Power Monitor only works with electronic power meters having the infrared LED. It is not compatible with older electro-mechanical power meters that have a spinning disk. 


The ZIP file includes the source code, schematic diagram, PCB layout, bill of materials and photos.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
