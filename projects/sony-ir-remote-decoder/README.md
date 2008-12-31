# Sony IR Remote Decoder

Uploaded by AndyPalm on 2008-12-30 18:59:00 (rating 0 out of 5)

## Summary

 cTVRemote.c - Decode Sony IR remote control


 Operation: Measure counts between falling and rising edges on  

 TIMER1 input capture pin connected to IR demodulator.  

 Reads 12 bit Sony IR remote control codes. Displays  

 results on two sets of LEDs.


 Hardware: ATtiny2313 on breadboard with 8 MHz external xtal.


 Port B connected to command bit LEDs (PB0:PB6).  

 Port D connected to device bit LEDs (PD0:PD4).  

 Pulse input from IR demodulator connected to ICP/PD6.  

 This line has external 10 Kohm pullup resistor.


 Notes: The Sony code consists of a start pulse of length 2.4 ms  

 followed by 12 pulses of length either 0.6 ms for zero or  

 1.2 ms for one. There is 0.6 ms between pulses. The first  

 seven bits give a command code, LSB sent first. The last  

 five bits give a device code, also LSB sent first.


 The pulse line from the 38 kHz IR demodulator (Vishay Part  

 TSOP2238) is normally high at Vdd and the line is pulled  

 low for a pulse. 


 Andrew Palm  

 2008.12.28

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
