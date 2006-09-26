# Data Aquisition System

Uploaded by Grebog on 2006-09-26 14:36:00 (rating 0 out of 5)

## Summary

This program does nothing at the very start of it. You need to enter a number between 1 and 100 in a serial terminal connection. It then will take measurements every 250 ms from the frequency counter and the temperature sensor. Once the number of samples you entered has been complete, it sends out your data in CSV format in the serial terminal. When it sends out this data it again turns off the A/D conversion and the timer until the next number of samples have been entered. Each sample is composed of the frequency and the temperature. The temperature is in celsius or farhenheit, depending on a state of a switch.


The frequency counter is very basic and not all that accurate. It measures between 100 Hz and 1 kHz and limits the values to be no more than the maximum and no less than the minimum. Due to the way I coded this, it is only accurate up to the tens digit of the frequency. (i.e. You will see 10x, where x is always 0, this is true up to 100x too) I also outputed the frequency on some 7-segment displays so that I could easily, visually verify the frequncy.


The temperature sensor is assumed to be a linear sensor that would generate a voltage between 0 V to 1.3V. This was for a temperature between 0 F and 130 F. There was also a switch that determined where to output in Celsius or Farhenheit.


There was serial input and output in this project, which was just using scanf's and printf's. So, the stdio.h is required in this project, which will eat memory space. I had started writing my own version, so that I would not need to #include stdio.h, but did not have time to take this further.


In the background I have a 1 ms timer overflow interrupt that controls all of the timing used in this project. This is where it figures out when 250 ms and 100 ms have passed and if so, do something further. But, every 1ms the sample number it was on in measureing was output to the 7-segment display.

## Links

- [Data Aquistion System source](http://www.grebog.net/AVR_Projects/samplesc.c)

## Compilers

- CodeVisionAVR-Light

## Tags

- Complete code
- CodeVisionAVR-Light
