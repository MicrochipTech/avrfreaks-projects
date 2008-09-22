# Household power logger

Uploaded by dstahlke on 2008-09-22 01:32:00 (rating 0 out of 5)

## Summary

Current is measured via current transformers (made from TV flyback cores) in the household fusebox. Voltage and current are amplified and fed into the ADC of a mega168. These signals are used to compute power which is displayed on a GLCD and logged to an SD card. The GLCD can also show an oscilloscope view of the voltage and current waveforms. The data is written to the SD card in a raw binary format and a simple program is included which will convert the data to a text file readable by gnuplot (Linux only for now).


The amplifiers deliver three different gain settings to the ADC so that accurate measurements can be made at levels of only a few watts all the way up to several kilowatts (200 amps). A PCF8583 RTC chip keeps track of the time for logging purposes.

## Links

- [Web page with pictures, etc.](http://www.stahlke.org/dan/powermeter/)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
