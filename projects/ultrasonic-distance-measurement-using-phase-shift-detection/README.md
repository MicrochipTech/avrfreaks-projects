# ultrasonic distance measurement using phase shift detection

Uploaded by Petervg on 2005-12-27 06:59:00 (rating 0 out of 5)

## Summary

How doe sa typical sonar work? -> you send a tone burst with a frequency of 40Khz using ultrasonic tyransceivers. The problem is that this method is not very accurate because you will always 'lose' a couple of samples (startup of the transmitter and receiver). You can use a different technique to measure distances with a very high accuracy using an other technique known as phase-shift measurement. In stead of sending a short burst and measuring the time between sending and receiving, you send a constant stream (sine wave at 40Khz) and you measure the phase difference between the outgoing and incoming signal. Obviously you can do only relative measurements, and not absolute.

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
