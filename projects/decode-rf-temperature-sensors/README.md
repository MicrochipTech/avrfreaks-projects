# Decode RF temperature sensors

Uploaded by jremington on 2012-01-21 23:15:00 (rating 0 out of 5)

## Summary

Two main routines are presented. Each decodes and displays the output of one sensor type. La Crosse sensors TX3-TH and TX6U are supported by TX6\_decode.c, while Oregon Scientific sensors, for example THR138 and THN128 that use V1.0 protocol, are decoded by OSV1\_dec.c The demo programs run on an ATtiny26 using the 1 MHz calibrated internal RC. 


Only two port pins are required, so a very small micro is suitable. Input comes in from a 433.92 MHz receiver, cheaply available from many sources. Output is intended for a serial LDC display (16x2 or 20x2), driven by software uart code at 2400 baud.


This is part of a larger project intended to collect data from various different remote sensors for recording and display in a central location. In beginning the project I found very few useful examples of code for the Oregon Scientific sensors and none at all for the LaCrosse sensors. Arduino code is already available for OS V 2 and 3 protocols, but I don't have any of those sensors. Hence this post! 


Questions, comments and additions/corrections are welcome.


V1.0 Jim Remington, Jan. 20, 2012

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
