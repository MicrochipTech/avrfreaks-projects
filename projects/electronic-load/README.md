# Electronic Load

Uploaded by Murus on 2006-03-26 09:15:00 (rating 0 out of 5)

## Summary

This is an electronic load which can be driven with up to 200W. (max. 20V and 10A)  

Therefore, i regulate an IGBT. This regulation is totally analog, the ATmega8 only controls the Display and shows me the temperature of the heatsink, measures the current and the voltage of the source and it controls the fan, if it is necessary.  

You will find a schematic of the analog IGBT-regulation in the zip-file. It is built with zwo OpAmps and a few filters, otherwise, it would oscillate.  

The control-circuit can be operated with 14-37V.  

Theoretically, the IGBT can handle a power dissipation of 400W, but I don`t think my cooling can handle that....

## Compilers

- BASCOM-AVR

## Tags

- Complete code
- BASCOM-AVR
