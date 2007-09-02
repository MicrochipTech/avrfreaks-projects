# MIDI interface for RS232 or USB

Uploaded by tuttut on 2007-09-02 13:27:00 (rating 0 out of 5)

## Summary

Use a ATmega8 and a FT232 to get a bidirectional MIDI-interface for USB. Midi-data will be transmitted 1:1 from PC at 38400 baud to MIDI at 31250 baud and vice-versa. With a generic serial midi driver you can use MIDI stuff with your PC even if you have only a USB or RS232 port.


My source shows how to implement a full-duplex interrupt-driven software-UART using Timer1, it shows simple-to-use non-blocking timerfunctions (using timer0) to get many softwaretimers and it shows how to implement a simple ringbuffer. All is GPL, using AVR GCC.


Have fun.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
