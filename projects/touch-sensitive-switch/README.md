# Touch sensitive switch

Uploaded by gclements on 2005-07-22 02:23:00 (rating 0 out of 5)

## Summary

I built a similar device out of a sensor, a 555 time, a rectifier, and an op-amp and a bunch of trim pots (it still needed a latch to eliminate ambiguity).


As an alternitive I want to use the ATtiny13's ADC to simplify the process.


The uC would begin charging the capacitor/sensor directly, then discharge it though a resistor and time how long it takes to discharge. The length of time it takes to discharge would determine the capacitance of the sensor. And from that value decide wether the output should be driven high or low.


This would make the hardware side of things vastly more simple, and possibly allow more than one sensor per circuit.  

The sensor capacitor I'm using is a pice of one sided PCB about 2" x 2" with a line cut dividing the copper into two halves. Two leads are soldered onto each half, and a thin strip of tape covering the copper to preven resistive inteferance.


Layout:  

Simply connect one lead of the sensor to ground. The other lead goes directly into the ADC (I used ADC2/PB4). A 1M ohm resistor connects betwine the charge/dischage pin (I used PB0) and the same ADC pin above.  

Attach a switch/button to the calabrate pin (PB2) so it will default ground (or VCC when pressed).  

Attach the output (PB1) to whatever you want switched, or for testing an LED).  

Connect VCC, GND, and RESET accordingly and you are done.  

(Why do a diagram? There really is nothing to this...) 


I have some code writen, but it didn't work out of the box. I wont be able to begin debuging it without an O-scope. (waiting for school to start up again)


My end goal is to construct a touch sensitive morse code keyer.


UPDATE (JULY 21, 2005):  

I tweeked the code some and I was getting a response. Very sketchy, but a definite response depending on the position of my finger over the sensor!


Some questions:


Does the capacitor charge to fast? to slow? 


Does count increment back around to zero before the charge loop is finished? Or does it not get past 1 or 2?


More added above concerning the circuit.  

New version of the code uploaded.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
