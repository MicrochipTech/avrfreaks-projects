# Stepper Motor Driver

Uploaded by admin on 2002-04-18 04:19:00 (rating 0 out of 5)

## Summary

This program is designed for the Atmel 4433. It controls any 2 winding unipolar stepping motor with a center tap for each winding. (These motors are abundant in old printers and office machinery) It utilizes the A-D converter combined with a 10k pot to control the speed of the motor. It also uses two buttons connected to the digital input pins to control forward/reverse and enable the vibrate function. This program uses no interrupts and is programmed in a down and dirty get the job done style. There are comments however. Note : the output pins on the avr chips will not drive a stepping motor by themselves, so you must use a fet or transistor of some sort to actually drive the motors. Its also nice to put an led on the pins so you can see which pins are being powered when. There is a jpg picture of the circuit on a breadboad for anyone who wants to reproduce the setup.

## Compilers

- AVR Studio 3

## Tags

- Complete code
- AVR Studio 3
