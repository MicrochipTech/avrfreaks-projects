# Geiger Counter on a Chip

Uploaded by iolair on 2013-01-14 20:37:00 (rating 0 out of 5)

## Summary

Complete back-end for geiger counter: second and minute display on LCD, event indicator, Sq. wave generator for HV section. I've found many Geiger Counter projects on the web but they all seem to generate the HV pulses with a LM555 timer chip - why do that when the Atmega is just looping around doing very little? The frequency of the pulses (2.6KHz) is changeable in code, but it works well with the Radio Shack audio transformer specified.


2x16 (HD44780) LCD connected to PORTB; buffered input from geiger tube on PORTD pins T1 (11) and INT0 (4); 2.6 KHz square wave to drive HV transformer available on PORTC pin 5, indicator signal (LED, speaker) from PORTC pin 4.


The Atmega8 can't produce or process high voltage, and my geiger tube requires 350-450 volts DC. The square wave output on pin C5 can drive an easily obtained Radio Shack 8 ohm to 1K ohm audio output transformer, which is then multiplied in voltage several times up to the required 400 volts or so. A schematic is enclosed.


The LCD connection is outlined in the main code. Please note that it is D4-D7 of the LCD that gets connected to PORTB, not D1-D3.


If you build the HV section, be careful of the high voltages! Also, if you try to measure the voltage with a standard 10MOhm voltmeter your voltage reading will be low - the current produced by the generator is so miniscule that even 10Megs poses a significant drag on the voltage. I found that enough voltage came from 4 stages of multiplication even though I've indicated 5. If you find yourself getting double indications for a single event, try taking the tube voltage off the previous stage of the multiplier.


Still to come: send counts-per-minute over UART on PORTD pin 3 - this works but I want to clean it up a bit.


All using Atmega8 internal oscillator running at 8MHz. Set your fuses accordingly!

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
