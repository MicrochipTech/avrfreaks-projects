# Guitar Tuner v1.2

Uploaded by Chancy99 on 2003-02-01 04:27:00 (rating 0 out of 5)

## Summary

This is a simple guitar tuner for both acoustic and bass guitars built around an at90s2313 AVR microcontroller. The code is loosely based on Jesper Hansen's tiny guitar tuner, which you can see at :


 [http://www.myplace.nu/avr/gtuner...](http://www.myplace.nu/avr/gtuner/index.htm)


This is a complete project : schematic, pcb layout, source code, the works. The total user interface is a small 8x2 character LCD, a single NO momentary pushbutton and an LED to indicate signal detection. It uses a dual opamp, 1/2 to amplify the signal from a tiny electret mic, the other 1/2 to form a schmidtt trigger to provide a nice square wave for the AVR. Gain is adjusted with a small thumbwheel 5k pot. The whole thing fits into a small Radioshack 3x2x1" plastic box. It's a tight fit, but it works.

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code with HW
- CodeVisionAVR-Standard
