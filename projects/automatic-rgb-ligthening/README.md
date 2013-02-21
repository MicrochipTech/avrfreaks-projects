# AutoMatic RGB Ligthening

Uploaded by Heikhama on 2013-02-21 02:57:00 (rating 0 out of 5)

## Summary

In this project ATMEGA8 with two timer is enable and the PWM port are enable. This PWM are used to fire three different MOSFET with pullup voltage of 12V. The RED green BLUE pin are connected to OCR1A and OCR1B and OCR2. respectively


LED  

brightness with a Pulse Width Modulation (PWM) scheme  

that gives the application the capability of displaying a billion  

colors in an RGB cluster. The maximum current is set by an  

external resistor.  

The LED brightness is controlled by performing PWM control  

on the outputs. The brightness data of the PWM signal for  

each LED is stored in three 10-bit registers. The peak value  

for each LED can be adjusted (dot-corrected) to compensate  

for mismatch, aging, and temperature effects. All the internal  

latched registers are loaded by a 32-bit shift register. One  

address bit controls whether dot correction/clock divider ratio  

or brightness data is loaded into the registers. The remaining  

bits are used for the data.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
