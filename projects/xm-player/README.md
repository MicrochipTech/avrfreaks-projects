# XM Player

Uploaded by psykotron on 2003-01-11 11:36:00 (rating 0 out of 5)

## Summary

Update 2003-12-10: Now with volume support.


This is a simple, but working, player of compact XM music data for ATmega163 or compatible. It supports raw sound sample playback in 4 channels at various speeds according to the XM format. The sound is mixed and output on an 8-bit port. The current configuration achieves an 11 kHz sample rate using a 3.69 MHz clock, but can easily be increased for higher clock rates.


The XM music data is converted from normal XM files to a slightly more compact format using a conversion tool, and can then be loaded into flash together with the player which takes about 1 kB. Due to the size of XMs (some kB to megs), 16 kB flash or more is highly recommended.


An example hardware implementation called Cellulose can be found at <http://www.tapir.nu>  

It uses a resistor network as 8-bit DAC, a simple op-amp as amplifier, and an old PC-speaker for output.


Experience with XM music files and preferably XM tracker software is highly recommended.

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
