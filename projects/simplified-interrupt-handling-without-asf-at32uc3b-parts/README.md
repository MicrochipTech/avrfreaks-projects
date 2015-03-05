# Simplified interrupt handling without ASF for AT32UC3B parts

Uploaded by tominsf on 2015-03-05 04:34:00 (rating 0 out of 5)

## Summary

I've come up with a quite simple method for using interrupts - without the ASF - for the AT32UC3B series.


See my post here for the description:


[https://www.avrfreaks.net/forum/s...](https://www.avrfreaks.net/forum/simplified-interrupt-handling-without-asf-at32uc3b-parts)


The important bit is the header file, but I've included C code for a simple example that runs four concurrent interrupts.  This should run on an EVK1101, but I've included the schematic of the circuit I actually used to test it.


Note that this is still experimenta/developmental and there are certainly no guarantees.  But it's been working nicely so far.  Note also, this is for AT32UC3B parts ONLY, although I suspect it should be easy to extend to the other UC3 parts.


To make this work:


1. At top of the main source file there should be one define per interrupt group used, e.g.:


#define INTCB2


#define INTCB14


2. #include “intcb.h” just below the #defines - this is the header file that contains the c function that sets things up


3. In the source file, there should be one ISR function per interrupt used, named as shown, without prototypes (they're in the header). These functions look exactly like any other C function:


void intcb2(void)


{


     [body of function]


}


void intcb14(void)


{  


     [body of function]


}


There should be no 'return from interrupt' statement; this is taken care of in the machine code call routine installed in RAM above the EVBA table. The user is responsible for determining, if necessary, which line of the particular interrupt group has triggered, and for clearing the interrupt. The ISR function can call other functions, but as it is an interrupt, it should be kept as brief in time as possible .


4. In function main(), setup\_intcb() should be called near the top (setup\_intcb() is in the header file) .


5. The process of setting up each peripheral used should include selecting (as needed) the interrupt priority level and enabling the individual interrupt for the peripheral .


6. In main() or other functions, call as needed the macros START\_GLOB\_INTS and STOP\_GLOB\_INTS, which do what they say.


 


 


Schematic:


![schematic](https://www.avrfreaks.net/sites/default/files/intcb_example.3.jpg "intcb_example schematic")

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
