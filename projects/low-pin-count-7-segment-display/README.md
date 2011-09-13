# Low pin count 7-segment display

Uploaded by sashman on 2011-09-13 03:09:00 (rating 0 out of 5)

## Summary

Typically, seven segment displays consume lots of MCU pins, up to the amount of characters + segments in the whole display. Using demultiplexors like the 74HC138/238 can save some pins. I've gone further and decided to multiplex both segments and characters lines. A multiplexor with latching input came in handy.


I've got a common-cathode display so it's set up that way. The project can be trivially modified for a common-anode one.


#### Example operation


![!](https://www.avrfreaks.net/modules/FreaksFiles/files/4406/20110126-173248.gif)


#### Project features


Low pin count: requires 6 GPIO pins to drive a display up to 16 digits x 8 segments (seven segments plus a decimal point).


4 of the pins are address lines and may be reused when not actually emitting light.


Requires three 74-series devices: two 74HC138 inverting demultiplexors for character selection and one 74HC237 latched demultiplexor for segment selection.


CPU utilization: on the scale of N*rows*segments microseconds every 1000 us.  

Adjust timings for deisred brightness/performance.


Memory utilization: a "frame buffer" of [characters] size in RAM. A [font](http://en.wikipedia.org/wiki/Seven-segment_display_character_representations) is stored in program memory.


Peripheral utilization: uses a timer.


#### Theory of operation


(0) port is initialized for output on address pins, ~LE (latch enable) pin and ~GE (glow enable) pin.


When invoked, **sevenseg\_refresh()**:  

(1) sets an address combination for a given segment on the output port and strobes 237's ~LE line thus storing an address in the 237's latch and selecting a segment line.


(2) for every character **c** in frame buffer, if a currently selected in (1) segment **s** is active in **c**, outputs a an address corresponding to the character **c**, asserts the ~GE (glow enable) line for 1 us (the more the delay, the brighter the LEDs).


(3) steps (1) and (2) repeated for all segments x characters.


(4) control is returned to the calling code for another 1 ms. The more this delay, the dimmer the LEDs. 


Data is output by manipulating the bytes in frame buffer. Generally, no locking/double buffering is required.


The refresh routine is not time critical, however, systematically delaying the call on a busy MCU may result in fluctuating brightness and/or flicker.


#### EMC considerations


Decouple the MCU and 74HC power lines with 0.1 uF ceramic capacitors placed close to the respective devices. Place a larger electrolytic or tantallum capacitor somewhere on the +5 V bus. If EMI is an issue, consider using ferrite beads on LED lines.


#### Possible modifications


If you need only up to 8 digits, pin count is reduced to 5 (3 reusable) and 74-chip count becomes two.


If you need more than 16 characters, I guess it is possible to add one or two more address lines.


Also can be fitted to use 74hc238 and 137 chips instead of 138 and 237. You need some inverting and some non-inverting devices, some latched, some unlatched. I've not investigated whether it is possible to use only latched devices.


\_delay\_us() may be replaced for doing useful code.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
