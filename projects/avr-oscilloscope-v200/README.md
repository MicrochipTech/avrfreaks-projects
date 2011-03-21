# AVR oscilloscope v2.00

Uploaded by serasidis on 2011-03-21 09:56:00 (rating 5 out of 5)

## Summary

This AVR based oscilloscope can work from 10Hz up to 7.7 kHz input signals in square waveform or almost 4 kHz in any other waveform as triangle or sine.


If the measured waveform is not stable you can freeze the the signal to see the real form of the measured waveform.


-------------- Features ---------------  

Measured frequency: 10Hz - 7.7 kHz  

Voltage input: 24V AC / 30V DC (by using 1:10 prob)  

Power supply: 12V DC  

Liquid Display Crystal: Graphical 128x64 pixels  

Measurement display area: 100x64 pixels  

Information displaying area: 28x64 pixels  

Triggering: Auto


Changes on v2.00 (firmware was updated on 19 Mar 2011)  

======================================================


- Indication for Volts peak-to-peak, DC volts and frequency of the measured waveform is appeared on LCD.


- Increased the sampling rate up to 7.7kHz. From 2300Hz the frequency indication is not very accurate because the frequency calculation is based on the took-up pixels on LCD of the waveform for one period.


- A small bug with the timeDivision buttons was fixed.

## Links

- [AVR oscilloscope project](http://www.serasidis.gr/circuits/AVR_oscilloscope/avr_oscilloscope.htm)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
