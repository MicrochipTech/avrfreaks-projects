# ISAAC based RNG

Uploaded by c_oflynn on 2002-11-15 02:01:00 (rating 0 out of 5)

## Summary

This pseudo-random number generator makes random numbers that are rather random, based on ISAAC ([http://burtleburtle.net/bob/rand...](http://burtleburtle.net/bob/rand/isaacafa.html) is website). It will generate a float type between two ranges you set. It is easy to change though to make any type of number between any range (currently range is -127 to +127 roughly). I tested the core random number generator on the DIEHARD series of tests, and it passed them all very well.


WARNING: This thing consumes HUGE amounts of RAM and FLASH, for example I don't think it will fit onto an AT90S8515, you will need at least AtMega. The AVR isn't exactly designed for working with 32 bit numbers and type doubles...


If you have comments on this generator, please e-mail me as I will be interested in your comments (especially if you happen to run some statistical tests or something on it!) [c\_oflynn@yahoo.com](mailto:c_oflynn@yahoo.com) . I plan to eventually perform a spectral test on this thing...


Also note that Bob Jenkins the the guy who actually came up with this RNG, I just added a bit of code onto the front-end of it as I thought it might be of use to people here...

## Compilers

- AVR32 GNU Toolchain

## Tags

- General Lib.functions
- AVR32 GNU Toolchain
