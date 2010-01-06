# Frequency Counter

Uploaded by AndyPalm on 2010-01-06 03:01:00 (rating 0 out of 5)

## Summary

 asmFreqCounter4a.asm - Frequency counter with divide by 10 prescaler  

 for direct conversion shortwave receiver


 Timer1 plus an additional byte form a 24-bit counter with Timer1  

 counts externally triggered by pin T1. The Timer1 overflow ISR is  

 used to increment the upper 8 bits. 


 This version scales the count assuming an external prescaler divides  

 the signal by 10. With a clock frequency of 16 MHz the counter  

 is good for range of 0.45 to 72 MHz (before prescaling) using a  

 one half second capture interval.


 Hardware:  

 ATtiny2313 with 16.000 MHz crystal.  

 One crystal cap is fixed 22 pf, the other is a 9 to 40 pf trim cap  

 adjusted to calibrate the counter reading.  

 Five 7-segment common anode LED displays with the segments a to g  

 and decimal point lines wired in parallel. The display format  

 is set as X.XXX.X, the last digit being the hundreds Hz digit.


 AVR pin assignments:  

 PORTD0:4 - Anode lines of the five 7 segment displays,  

 D0 = lowest digit  

 PORTD5 - T1 pin for prescaler input  

 PORTB0:7 - Segments a to g and decimal point of displays


 Notes: The code handles counts of up to 8 decimal digits but  

 displays only the 5 digits needed for this application.  

 For frequencies higher than 9.9999 MHz, an additional  

 10 MHz digit can be added and the spare PORTD6 pin used for  

 its anode control (with suitable modifications to the code).  

 Due to the divide-by-ten prescaler and 0.5 second capture  

 interval, the resolution of this version is 20 Hz, so  

 the display of the 10 Hz digit would not be very useful. 


 If the receiver is to be used for CW only or is a superhet,  

 then additional code can be included to add or substract an  

 appropriate CW sidetone or intermediate frequency offset to  

 the displayed frequency.


 If the code is used as a stand-along frequency counter, one  

 can easily change to a one second capture interval by  

 setting COUNT\_IVL to 500 and eliminating the multiply by two  

 code in the main loop.


 Andy Palm  

 2009.12.15

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
