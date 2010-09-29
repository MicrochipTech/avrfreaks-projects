# Sun Position Algorithm

Uploaded by scottfromscott on 2010-09-28 18:53:00 (rating 0 out of 5)

## Summary

The attached C function uses an elegant, and fairly short algorithm for approximating the sun's position and could be used for a PV panel solar tracker. I will use it with an Atmega 328p, but this code is hardware independant. 


Updated September 28, 2010


Replaced algorithm with SPA Algorithm published by Blanco, Muriel, et. al.  

Inputs (available from a GPS reciever): UTC month, day, year, hour, minutes, seconds, latitude, longitude  

Outputs: sun altitude, azimuth

## Compilers

- AVR Studio 4

## Tags

- General Lib.functions
- AVR Studio 4
