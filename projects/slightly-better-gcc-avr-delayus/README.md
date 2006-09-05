# Slightly better gcc-avr delay_us

Uploaded by Zathras on 2006-09-04 18:33:00 (rating 0 out of 5)

## Summary

It's simple enough. Just #include "delay2.h", and feed \_delay\_us\_2() a constant. It should be exact down to 1 clock(a), rounded down. This won't compile without some degree of -O.


(a) It needs r24/25 for larger sleeps, you may pay anywhere from nothing to 2 pushs/pops for that. Small sleeps are done as NOPs and RJMP+0s. Heck, just read it, it's only a few lines.


It was broken, should work now. An rjmp +0 goes to the head of the current function, but rjmp .+0 does the right thing.

## Links

- [The file](http://members.cox.net/pen_sq/delay2.h)

## Compilers

- AVR32 GNU Toolchain

## Tags

- General Lib.functions
- AVR32 GNU Toolchain
