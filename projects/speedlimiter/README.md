# speedLimiter

Uploaded by rasmus.renzi on 2006-07-02 19:33:00 (rating 0 out of 5)

## Summary

Code is now complete.  

The limiter measures the time between ignitions and calculates the RPM.  

When actual RPM value is above set limit, the limiter cuts the ignition.  

The limiter has a display interface, 2*16 line intelligent ASCII display, and three push buttons for user input.  

The limiter controls the ignition with an insulated gate bipolar transistor (IGBT).  

On every ignition interrupt the limiter opens or closes the IGBT to either fire a spark or start dwelling the ignition coil.  

The limiter reads the input signal from the mechanical distributor’s ignition switch, which in this case is disconnected from the ignition coil and instead connected to battery voltage via a pull-up resistor.

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
