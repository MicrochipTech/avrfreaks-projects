# Simplex software uart

Uploaded by jeroen3 on 2011-12-14 19:12:00 (rating 5 out of 5)

## Summary

This software is an timer interrupt driven software uart mainly developed for attiny13.  

It can be used to create an small ADC to UART converter or anything.


Tested to work at 9600 8N1.  

Baud rate can be changed, formula inside.


Easily portable to any other tiny or mega with a timer


Size is about 554 bytes flash and 4 Ram bytes (-0s)  

Chip requirements is one output compare timer  

Uses AVR Studio 4


This does only send data. It cannot receive. (yet?)


I made this from a flowchart I found once, as a good programming exercise.


Update:  

>There is probably a bug with the putint\_ascii, it is for unsigned int, and does work with signed int argument.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
