# AVD-DDSv2

Uploaded by ksvitale on 2006-07-09 01:16:00 (rating 0 out of 5)

## Summary

2006-Jun-30  

Description: Battery-operated, compact signal/waveform generator  

Background: I needed a small, battery operated signal generator to use as a signal source for a more complex PLL project I am working on. I found Jesper's "MiniDDS" but had difficulty using his Windoze-based frequency setting program as I cound not set the baud rate to anything other than 115K. Jesper lost the code in a hard drive crash a few years ago so getting the source from him was nill. My 2nd intention was to allow communications with the unit using *ANY* OS (including PALM-OS) that provided a dumb-terminal (TTY/ASCII) interface to a serial port. I also wanted to use a faster processor (TINY2313) and a more standard frequency crystal (10 or 20MHz). I wanted to use a MAX856 boost-regulator to run from 2-AAA cells at 3V instead of the original 9V. The intention was to leave the AVR in POWER-DOWN mode and raise RS-232 DTR to awake and run, drop DTR to sleep again. I re-coded pretty much everything except for a few of the low-level serial routines and the main accumulator loop. Internally, the code keeps track of frequency in packed-BCD format and does a conversion to the proper 24-bit accumulator values if a frequency change request occurs. The majority of the code is the 6-digit packed-BCD add, subtract and accumulator update routines. Four wavetables are held in program memory (1K total) I had only 1K of code space left for actual code (there is currently room for 9 more instructions left). I moved all the ASCII message strings into EEPROM (all 128 bytes used!). I have supplied an EXCEL spread sheet which can be used to create other wave tables with various # of samples and bit weights as well as EXCEL spreedsheet for figuring out 8/16/24 bit counter values and baud rate using xtal freqs of your choice. I have also provided ramp and reverse-ramp wavetables. Be aware, since I did not get the sleep routines to work properly, this code runs all the time. Also, the TINY2313's have not come in yet so the part used is a AT90S2313. If/when I get the sleep routines to function, I'll post the code. Shared this with Jesper as well. I might add that the schematic shows an OPAMP buffer on the output but I have not yet built and tested that portion of the circuitry.


2006-Jul-08  

Update: Updated schematic and code. Power-down mode functioning correctly. Measured 40ma run current, 700ua "sleep" current.  

Note: MAX856 can start up with 2.0V so using rechargable cells is permissable. Using 10MHz crystal yields 0 to 500KHz operation (unloaded square wave out).  

Recommendation: For improved accuracy, measure actual running crystal frequency (at 5V) and enter into the code defs before assembling and programming AVR.  

----------------------------------------------------------------------------------


Peace and blessings,  

Scott, in the vicinity of the land known to men as central florida within the political border known to men as the united States of America (without THE UNITED STATES, INC.)  

!all rights explicitly reserved without recourse!

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
