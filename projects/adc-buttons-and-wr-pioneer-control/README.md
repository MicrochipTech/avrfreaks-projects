# ADC buttons and W/R Pioneer control

Uploaded by epaloniemi on 2015-02-01 14:15:00 (rating 5 out of 5)

## Summary

After changing the factory audio system in my car(2008 Nissan Tiida, Nissan Versa in north america) to an aftermarket Pioneer SPH-DA120 I wan'ted to regain the use of my steering wheel buttons.


Pioneer sells an adapter unit for my vehicle but I couldn't find specific information on what button functions it supports. Also it was 69€. By making my own adapter I can setup button functions as I please. i.e. quick tap = control x, hold down = control y


I found some info about the wired remote input Pioneer uses: <http://jvde.net/node/7>


Then I used a multimeter to find the steering wheel buttons wires on the factory audio harness. There are six buttons and three signal wires, wires are ground and two signal wires.


The two signal wires have three states relative to ground depending on what button is pressed. No buttons pressed = open circuit(5 V). Button signals for both signal wires are 0 ohm(0 V), 165 ohm(1.7 V) and 652 ohm(3.3 V).


The factory audio system has 330 ohm pull up resistors to the two button signal wires.


I decided to use ATMEGA8535 to make my adapter.


crystal = 14,318180 Hz


I use ADC input pins 0 and 1 to read the signal wires voltage, I use 390 ohm pull up resistors.


Then forward blocking diodes on 9 I/O pins with resistors to generate control signals for Pioneer W/R input.


Pioneer has its own pull up resistors so I only sink current with the I/O pins. The diode is to block 5 V from the mega8535 from over driving the 3.2 V of the W/R input.

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code
- CodeVisionAVR-Standard
- ATmega8535
- car stereo
- pioneer
- W/R
- nissan
- versa
- tiida
- car
- Car Radio
- adc
- microcontroller
- buttons
