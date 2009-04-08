# Quick and dirty 1wire clk RC1MHz

Uploaded by chris.vanbeukering on 2009-04-07 23:03:00 (rating 0 out of 5)

## Summary

AVR (in my case Mega8) runs of internal uncalibrated 1MHz oscillator. These 'routines' were created due to seperate pin requirement. If more sensors are required, just copy-paste-edit the segments to use specific pins.(I never did get around to incorporating arguments for the primitives to read from different pins in the same function). I've had five sensors running off my board, dumping the values to PC for data logging, which worked just fine (I had to monitor temperature distribution in an enclosure)

## Compilers

- CodeVisionAVR-Standard

## Tags

- General Lib.functions
- CodeVisionAVR-Standard
