# 16-bit division by constants (3-23)

Uploaded by alenze on 2006-03-24 14:23:00 (rating 0 out of 5)

## Summary

ASM library for divisions of 16-bit numbers by constants (range 3-23) using scaled reciprocal multiplication.  

HW-multiplier ("mul" instruction) required.  

Functions typically need ~36 cycles (w. remainder) or ~27 cycles (w/o rem.)  

2003-Feb-12: added /18, fixed errors  

2003-Feb-15: added /20, added "C-style" function with macro to perform a constants division with all divisors from 2 to 20 from one module  

2003-Feb-24: added /21-/23 (stand-alone & module)  

2002-Feb-24: everything speeded up by ~20% (thanks to J-H Reichert)  

2006-Mar-24: added /60 for RTC related apps., fixed bug (Q>>4)in several modules

## Compilers

- AVR Assembler

## Tags

- General Lib.functions
- AVR Assembler
