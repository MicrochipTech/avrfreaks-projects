# MOD Player for AVR 8-bit ATmega

Uploaded by TSLabs on 2010-06-24 05:04:00 (rating 5 out of 5)

## Summary

Minimum requirements for hardware:  

- MCU: ATmega AVR  

- Fclk: 16 MHz  

- Flash: 8kB  

- SRAM: 1kB  

- 2 x 8-bit counter/PWM  

- 1 x 16-bit counter


Specifications for player routine:  

- 4 channel 31 samples modules support  

- volume setting 0-64 for each channel  

- 2 stereo channels 8-bit PWM output  

- software linear interpolation  

- selectable output samplerate up to 48kHz  

- most used effect commands supported


The basic idea was to create an all-in-one implementation  

without use of external components. On-board PWM's are used  

as DAC's, the module is kept in Program Flash.  

Tha player is written in pure Assembler.  

The current release includes conditional compilation  

for ATmega16 and ATmega128 (those author has in presence),  

but is also designed for very easy compilation for any other ATmega type.  

Also, there is a technological clearance for use of external memory,  

RS232 interface for communicational purposes etc.


The schematics for this project is very simple. You just power the chip, supply the 16MHz clock to it and have LEFT and RIGHT audio from 8-PWM's. All specific pin numbers depend on chip, that you selected.


WARNING  

Some obscure bugs in the player routine found!  

I will fix them later (I plan to attach external memory and flash to the player unit and then will tenaciously search for bugs).  

If you found any buggy module or have a question - write me on e-mail:  

at2313 (D0G) gmail (D0T) com

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
