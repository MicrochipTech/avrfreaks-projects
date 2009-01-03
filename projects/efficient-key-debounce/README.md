# Efficient key debounce

Uploaded by danni on 2009-01-02 19:33:00 (rating 5 out of 5)

## Summary

Efficient key debounce (bulletproof)


Almost every application need input keys to use it.  

Most beginners are thinking, that debounce was easy or unneeded, but this was untrue.  

A right working debouncing routine was essential!


Following an efficient solution, which avoid all disadvantages from "simple" solutions.  

It debounce one up to eight keys with the same code.  

It sample every input four times and accept a change only, if all four samples are equal.  

Thus it was also useful to suppress noise, e.g. from connections over long lines.  

Furthermore it detect the key press event, means the transition from the released to the pressed state.  

This event was stored, so that if the software was busy, the key press wasnt lost.


To understand the working the demo code display on LED7..3 the internal counter  

state from key 0. Also the debouncing interval was expanded to one second to see the working.


The second example toggle every LED7..0 on pressing KEY7..0.  

The debouncing interval was set to 10ms, which was a good value.  

But its not critical to use other intervals (2ms .. 50ms).


The examples are tested on the STK500 with the ATmega48.  

Please watch, that KEY7,6 must be connected to XTAL1,2 for the ATmega48.


Also you should use a modern WINAVR version, which support atomic access.  

The atomic statement was needed to not interrupt the get\_key\_press function,  

otherwise the key press event may be lost.  

For compilers without atomic access the CLI/SEI instructions must be used.


Peter

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
