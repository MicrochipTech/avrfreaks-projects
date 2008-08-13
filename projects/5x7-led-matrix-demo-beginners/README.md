# 5x7 led matrix demo (beginners)

Uploaded by Zaphodikus on 2008-08-12 18:11:00 (rating 0 out of 5)

## Summary

Will work with either the common-cathode or anode LED matrix versions  

with only modification being inverting the drive on the common line  

and data line at runtime.  

LEVEL :  

=======  

Beginner (my 1st micro project ever)


MICRO :  

=======  

ATMega 8 (requires 5 + 7 output lines)  

 944 bytes of flash.  

5 transistors, kingbright common-cathode TC07, 5 resistors, power  

some filter capacitors, 3xAA cells+Homemade parallel programmer. 


DESIGN NOTE:  

============  

The port code is 'hard-coded' :-( it works like this:  

1. turn on the column  

2. turn on the bits (rows)  

3. leave it on to get the eye to see it (we rely on Persistence Of Vision)  

4. turn it off  

rinse and repeat for each column, 20 times over before we scroll the whole window. see it at [http://www.youtube.com/v/Umy0rV2...](http://www.youtube.com/v/Umy0rV2MsZA)


The diagram is simply drawn over the atmel-pinout page because that is how I built it.


TIME:  

=====  

About 10 hours - (3 hours to learn C and Win-Avr), the rest to fiddle with the font, fiddle with the breadboard and  

LEDs, lastly write an algorithm. And write some windows test-code.


CONSTRUCTION NOTES:  

===================  

I started by hooking 8 leds onto the port, and incrementing a variable and writting it about every 50ms  

Then I hooked up the LED matrix with some resistors and mapped out the pins. Notice that the ROW, COLUMN pins  

are not in any sane order. Next I added the driver transistors into the fray. (I picked up the idea from an  

assembly project which does approximately the same thing on another chip. 


Next I turned on all the rows with jumpers to Vcc via 220 Ohms, and got the port to light each column in turn,  

lastly I wired up the rows to the micro and started to watch the show. Lastly I added scrolling. The LED matrix  

I had is common-cathode, although the original design was for common-anode, and half-way through my building I  

changed and inverted the drive from using NPN to using PNP transistors which are better suited to my LED. If  

 you have a common-anode LED array, use NPNs. Any general-purpose capable of 200mA will do since they drive  

about 40mA in reality.


You will see the project has a windows app. this was for designing font characters. I wrote this early on  

along with a tiny console test-prorgam so I could rip the font from the .asm source and check and fix any  

characters I did not like. 


Mail/message me if you want the font-dumper test-code or the GUI app test-code.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
