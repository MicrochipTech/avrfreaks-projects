# Float to String (ASCII)

Uploaded by c_oflynn on 2002-11-23 14:51:00 (rating 0 out of 5)

## Summary

note: This routine is a bit useless now, as avr-libc has built-in routines which do pretty much the same thing, but just a little better....


nov 24, 2002: fixed bug in software that misplaced decimal point


This function converts a type Float to a string. It has a few options, and only requires the stdlib.h, it does not use math.h, and in fact can be set up to use no floating point operations at all. It is an all-in-one routine, you don't have to do anything fancy ;)


It takes a toll in accuracy when doing large exponents though, as you will find out if you test this routine, which I advise you to do. Also it doesn't perform rounding (which is a good idea), so you may want to add that.


Although specifically designed to use AVR-GCC, it looks fairly portable although you might have to change one line. However, it DOES rely on the bit-layout of the type float, so the target compiler must be the same bit-layout!


Note: in AVR-GCC the call to ulta (unsigned long to ascii) takes about half of the total time it takes to execute the entire function! So if you need faster speed, try replacing that function.

## Compilers

- AVR32 GNU Toolchain

## Tags

- General Lib.functions
- AVR32 GNU Toolchain
