# Function Gen on Scope Clock

Uploaded by jrseattle on 2008-10-23 11:30:00 (rating 0 out of 5)

## Summary

Updated October 17, 2008


DDS based signal generator using the Dutchtronix AVR Oscilloscope Clock.


The Dutchtronix AVR Oscilloscope clock is a small Atmega168 based board which displays a pretty analog (and numeric) clock on an oscilloscope in XY mode; it works best on analog scopes (anything 2 Mhz or better) but also on high end digital scopes. You can find more details at [www.dutchtronix.com/ScopeClockH2...](http://www.dutchtronix.com/ScopeClockH2-0.htm). Note that the current version, Hardware 2.0 (firmware 3.0) is not being produced any more. An updated version, Hardware 3.1, will be available soon ([www.oscilloscopeclock.com](http://www.oscilloscopeclock.com)).


This oscilloscope clock has a built-in boot loader, intended to allow users to upload new firmware versions, but it means that anyone can load new programs in this gadget using just a serial cable. The clock application software itself is available on the website so you can always reload the original. If something bad happens, you can send in the Atmega168 for reprogramming.


I decided to port the mini-DDS generator from Jesper Hansen ([http://www.myplace.nu/avr/minidd...](http://www.myplace.nu/avr/minidds/index.htm)) to this clock because it has a built-in DAC (the TLC7528 from Texas Instruments, a dual 8-bit parallel DAC). The only complication was the fact that the DAC is not connected to one port on the Atmega168 but on 2 different ports (PORTB and PORTC, both bits 0..3). The reason for this design is the fact that there is NO complete port available on the 28-pin DIL Atmega168 when using features like the serial port (PD0, PD1), the crystal (PB6-PB7) and the reset pin (PC6).


As a result, the assembly code uses 11 cycles, as opposed to the original 9 cycles to generate a waveform sample value. Because the clock runs at 20 Mhz (Jesper's hardware runs at about 11 Mhz), the maximum frequency which will generate a decent waveform is still higher than the original from Jesper. The frequency of the DDS generator is a percentage of the "Adder Value" or accumulator, a 24-bit entity. In Jesper's generator, this value is 7.324%, in this implementation, it's 10.837%. My experiments show that a sine waveform becomes distorted around 200 KHz.


Jesper also provides a small windows application, called DDSControl.exe, to interact with the signal generator ([http://www.myplace.nu/avr/minidd...](http://www.myplace.nu/avr/minidds/ddscontrol.zip)). Once you figure out how to use it, it's very nice. The trick is to configure it properly by right clicking the title bar and select "preferences". Set the "DDS Resolution" to 24 bits, set the "Synthesizer Frequency" to 1818181 (20000000/11) and select the actual com port being used.  

I recently (October 2008) modified the generator to match DDSControl better. The arrows now work more intuitively: right arrows mean Higher Frequency, left arrows mean Lower Frequency. The amount of change per arrow is now in frequency (1Hz, 100Hz, 10000Hz) as opposed to the original accumulator value.


The source code also contains a conditional called "ASCIILINK". It is turned off by default but when turned on, provides an ascii interface to the PC so you can use a simple terminal program (Hyperlink, Brey's Terminal) at 19,600 bps to interface using the following commands:


1: sine output  

2: sawtooth output  

3: triangle output  

4: square output  

-: up 1 Hz  

d: up 100 Hz  

D: up 10,000 Hz  

+: down 1 Hz  

u: down 100 Hz  

U: down 10,000 Hz  

s: Set adder value using 6 hexadecimal upper case digits


Note that the direction commands have been reversed to match the DDSControl.exe program.


The mini-DDS generator will return the current 24-bit accumulator in hexadecimal. DDSControl.exe uses this value to compute and display the current frequency.


Use of the mini-DDS generator on the Dutchtronix AVR Oscilloscope clock could not be simpler: connect a scope probe or counter input or whatever needs the signal to the CH1 turret terminal and connect a serial cable to a serial port on the PC.


Downloading the new application in the Oscilloscope clock is done by turning the power to the board on while keep the button switch pushed down  

(the led will blink quickly). Use avrprog.exe to download the new application.


Use the new application using either the DDSControl.exe program from Jesper's website (if ASCIILINK is 0) or use a terminal program ASCIILINK is 1). When using a scope, you can see the waveform change as you enter the commands.


There is also a conditional DACLATCH, default OFF. Because the Atmega168 uses 2 nibbles to transfer a byte value to the DAC, there are 2 cycles in each iteration where the byte value isn't quite accurate. When DACLATCH is enabled, the clock board will more accurately generate the new voltage levels. Unfortunately, the generated waveforms are not as good; they are more blocky. The original inaccuracies are actually helping to smooth out the waveforms.


Thanks to Erik R. for bringing a bug and other issues to my attention.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
