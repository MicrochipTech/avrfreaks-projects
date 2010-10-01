# Magnetic Declination Solution

Uploaded by scottfromscott on 2010-10-01 13:55:00 (rating 0 out of 5)

## Summary

I have a Baby Orangutan robot controller with an AVR 328p microcontroller. I have an HMC6352 compass module attached and wanted to be able to adjust the compass readings for true north. There is software at the NOAA web site to do this, but I was unable to figure out how to make it work with my controller. I decided to use the NOAA software to calculate magnetic declinations for points around the globe, then wrote the software to approximate declination values in between those points. The attached C functions accomplish this within a few degrees of accuracy.


Updated September 28, 2010:  

Lookup table now supports declination values -128..127.  

Each declination still requires only one byte.  

Added development kit to create your own lookup table.


Download the 'declination function only' file to just use my table with your latitude and longitude values.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
