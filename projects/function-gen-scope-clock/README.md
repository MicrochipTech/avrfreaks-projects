# Function Gen on Scope Clock

Uploaded by jrseattle on 2007-09-04 21:51:00 (rating 0 out of 5)

## Summary

DDS based Function Generator using the Dutchtronix AVR Oscilloscope Clock.


The Dutchtronix AVR Oscilloscope clock is a cute small Atmega168 based board which displays a pretty analog (and numeric) clock on an oscilloscope in XY mode; it works best on analog scopes (anything 2 Mhz or better) but also on high end digital scopes. Of course, not everyone has a scope on their bench (though they should) but for those who have, this gadget is a steal at just $25.00, incl. shipping (it comes as a kit using all thru-hole components). You can find all details at [http://www.dutchtronix.com/Scope...](http://www.dutchtronix.com/ScopeClock.htm).


This oscilloscope clock has a built-in boot loader, intended to allow users to upload new firmware versions, but it means that anyone can load new programs in this gadget using just a serial cable. The clock application software itself is available on the website so you can always reload the original. If something bad happens, you can send in the Atmega168 for reprogramming.


I decided to port the mini-DDS generator from Jesper Hansen ([http://www.myplace.nu/avr/minidd...](http://www.myplace.nu/avr/minidds/index.htm)) to this clock because it has a built-in DAC (the TLC7528 from Texas Instruments, a dual 8-bit parallel DAC). The only complication was the fact that the DAC is not connected to one port on the Atmega168 but on 2 different ports (PORTB and PORTC, both bits 0..3). The reason for this design is the fact that there is NO complete port available on the 28-pin DIL Atmega168 when using features like the serial port (PD0, PD1), the crystal (PB6-PB7) and the reset pin (PC6).


As a result, the assembly code uses 11 cycles, as opposed to the original 9 cycles to generate a waveform sample value. Because the clock runs at 20 Mhz (as opposed to the original 11 Mhz), the maximum frequency which will generate a decent waveform is still higher than the original from Jesper. The frequency of the DDS generator is a percentage of the "Adder Value", a 24-bit entity. In Jesper's generator, this value is 7.324%, in this implementation, it's 10.837%. My experiments show that a sine waveform becomes distorted around 200 KHz.


Jesper also provides a small windows application, called DDSControl.exe, to interact with the signal generator. Unfortunately, there is no source code. It connects to COM1 at 115,200 baud but seems to have some quirks. The arrows are unintuitive (I think): > means decrease frequency and < means increase frequency. I changed the interpretation of the UP and DOWN commands from 1, 10 and 100 change in Hz to 1, 100 and 10,000. The frequency values as used by the DDS control application are somewhat puzzling since a frequency input of, e.g. 1,000,000 yields an adder value of 11,236,096, suggestion a percentage of 8.899, which does not correspond to the project as documented. This is not a problem, as long as it is understood that the values are very approximate.  

It looks like there is an updated version of DDSControl.exe and hopefully Jesper will make this available to us.


The source code also contains a conditional called "ASCIILINK". It is turned off by default but when turned on, provides an ascii interface to the PC so you can use a simple terminal program (Hyperlink, Brey's Terminal) to interface using the following commands:


1: sine output  

2: sawtooth output  

3: triangle output  

4: square output  

+: up 1 Hz  

u: up 1000 Hz  

U: up 10,000 Hz  

-: down 1 Hz  

d: down 100 Hz  

D: down 10,000 Hz  

s: Set adder value using 6 hexadecimal upper case digits


The mini-DDS generator will return the current 24-bit adder value in hexadecimal.


Use of the mini-DDS generator on the Dutchtronix AVR Oscilloscope clock could not be simpler: connect a scope probe or counter input or whatever needs the signal to the CH1 turret terminal and connect a serial cable to COM1 on the PC.


Downloading the new application in the Oscilloscope clock is done by turning the power to the board on while keep the button switch pushed down (the led will blink quickly). Use avrprog.exe (Tools - AVR Prog) to download the new application.


Use the new application using either the DDSControl.exe program from Jesper's website (if ASCIILINK is 0) or use a terminal program ASCIILINK is 1). When using a scope, you can see the waveform change as you enter the commands.


Hopefully, other people will come up with better versions of this application; it only uses 9% of the available code space! I apologize for any errors; testing has been minimal.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
