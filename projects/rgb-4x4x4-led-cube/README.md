# RGB 4x4x4 LED Cube.

Uploaded by wpqrek on 2010-06-02 21:22:00 (rating 0 out of 5)

## Summary

Welcome!


 I would like to present one of my finished project. I don't have much time so the presentation will be short and not detailed. I hope you will forgive me it. Inspiration to build such thing was similar devices presented on the Internet network. I am programmer at work and electronic hobbyist at private life so every kind of project that use microcontrolers I like a lot. 


 I know that this construction isn't perfect. Parts used to build I get from my home inventory. Don't ask why I use such parts. I just had have it. RGB LED's with common cathode was only bought. Patterns displayed by lamp are created by multiplexing each level of lamp. During one frame only one level is power on so 4*4*3 LED's are light at the time max. Each LED anode (individual colors) are connected in a column line driven by UDN2981A buffer controlled by 48 bit shift register (6 * 74HCT595N). Each cathode from the same level are connected to the same line driven by ULN2803A. ATMega32 is a heart of this device. Whole lamp can be driven only by 7 digit lines so you can use any other AVR device.


 You can easily extend 4x4x4 LED's cube if you want. Firmware was wrote using C and AVRStudio/AVRlib. Below you can find circuit, PCB board and firmware. Hope it will be helpful.


[Circuit, PCB board and firmware.](http://wpqrek.blogspot.com/2010/06/rgb-4x4x4-led-cube.html)

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
