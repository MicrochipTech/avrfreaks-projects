# Interrupt Driven Software UART for Tiny15 (1200bps)

Uploaded by shadowbane on 2005-10-06 01:00:00 (rating 0 out of 5)

## Summary

NOTE: See AVR305 & AVR304 for a faster & smaller software UART.. (I have a port of AVR305 to the tiny15 I've attached as another file to this project.)


Having a bit of time on my hands, I ported Mr Hristea's Half duplex software UART to the Tiny15.


At 117 words (with sample app), its significantly shorter than his code. Also, it is written to require NO SRAM, and only 3 high registers (6 other registers). It has a 3 byte receive buffer.


If you are trying to go higher than 1200 baud, I expect you'll need to deal with 2 things. The first is the hard coded magic numbers from Mr. Hristea's code (56 & 80). I assume that it'll require an oscilloscope to correct them. The second is what I believe is a bit of a bug (which if I fix, the code no longer works at 1200 baud, and without an oscilloscope, Im at a loss). On a receive the code waits for a full start bit length before sampling the RX pin. I believe that means that its sampling at near the same time the data is coming in. So it would become unstable at high speeds. Any input on these two issues would be appreciated. I did note that the original C code appeared to TX the start bit for the wrong length of time (refer to his code to see what I mean).


#defines exist to strip the transmitter or receiver, as well as reduce the stack usage to just the one entry required by the interrupts.


The sample main: routine contains 2 ifdefs. Id take them out if you want the code to be more readable. I didnt because I think the functionality they demo is important.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Part-specific Lib.functions
- WinAVR (GNU GCC)
