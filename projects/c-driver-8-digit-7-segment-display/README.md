# C-driver for an 8 digit 7 segment display

Uploaded by osbornema on 2019-02-01 12:56:00 (rating 0 out of 5)

## Summary

A multiplexer has been developed for use with an 8 digit 7 segment display.


Display modules are Kingbright CA56-12SYKWA or CA56-12SURKWA or similar.


Each is a 4 digit display with 4 outputs, one to activate each digit and 8 outputs one to activate each segment ('a' to 'g' plus the colon).


A multiplexer is required since without it every digit would display the same pattern.


 


The operation of the multiplexer is described in the .PDF attachment “Switch\_matrix\_operation”.


Circuit details are given in Eagle and .BMP fils “switch matrix circuit”.


 


The following four C modules are included:


To display integers from 0 to 99999999


To illuminate individual segments


To vary the display intensity


To drive digits singly in the absence of the multiplexer as an aide to fault finding on the pcb.


 


Note: Display of the colon/decimal point has not been implemented.


 


All C code is contained in the zip file “Driving the display”

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
