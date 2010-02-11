# Simple Button Debouncing

Uploaded by danielson on 2010-02-11 15:52:00 (rating 5 out of 5)

## Summary

90% of AVR projects will require button debouncing (from the book of made up numbers), and as I was trying to figure out where to start on writing some firmware, I decided to write some code for handling the button debouncing.


What I ended up with is likely more bloated than most solutions, but it's also more flexible and easier to use. This 'library' should be pretty portable across nearly all AVRS. All it requires is a single 8-bit timer (using TIMER0 in the included files) (and SRAM, we're talking C here)


The files included will compile for an Atmega2560 running at 8Mhz with CLKDIV8 enabled. 


Features:  

 Debouncing independent of PORT.. switches don't all have to be attached to a single port


 Debounce non I/O ports if you like.. it's just as simple


 You can signal state changes through the main loop execution path  

 or via interrupts (i.e. pin change interrupt, etc)


 Individually controllable debouncing delay, or stick to a global one


 It's the set-it-and-forget-it approach. Just setup which pins you want to debounce and use callbacks to detect debounced state changes.


 Each individual pin callback can be configured to either respond asynchronously (called from timer ISR) or synchronously using callSignaledHandlers() function


 Only 4*n + 1 bytes of global variables necessary for 'n' debounced pins


Cons:  

 Overhead can be larger for a large number of pins compared with debouncing an entire port at once for instance.


 Uses timer0


 Not entirely portable either, code assumes your avr has an 8-bit  

TIMER0 and that its register names/bits are identical. Changing this will only require altering a couple lines of code.


Note: can be compiled for an atmega2560 @ 8Mhz w/CLKDIV8 enabled out of the box, PINB for switches/PIND for LEDs

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
