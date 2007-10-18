# metronome

Uploaded by KijanLodz on 2007-10-17 20:18:00 (rating 0 out of 5)

## Summary

This program is for ATMega 8 microcontroler to create metronome. Program allows you to control tempo from 60bpm top 240bpm. Moreover you can set meter from 1/4 to 4/4. Microcontroler "steers" 4 7segmented led. You can use 74ls47 bcd driver to control them but i don't checked this. For now is controling by PORTC but there is one problem if you want to program with ISP you will have little problem, a verse "g" will not by polarized. You have to change a c file in that way that an array znaki will be in proper combination to polarized proper verse by a 74ls47 BCD led driver. I use a common cathode led so in schematics I use an pnp transistor to polarized prpoerly common cathode 7 segmented led

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
