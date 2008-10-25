# basic tune player

Uploaded by tevvybear on 2008-10-25 06:00:00 (rating 0 out of 5)

## Summary

This program plays a tune supplied in a *.c file as the global 'song\_t' variable 'song'. It was written for a lab at uni (University of Wollongong, ECTE333 course). Simply connect a small speaker and resistor to PORTD pin 4 and it should play the music. Note that the sound isn't pure, and contains harmonics at every odd multiple of the fundamental frequency (the note you ask it to play), so it sounds very artificial. 


A list of notes over three octaves (including sharps) is included as defines, making it easy to write your own tunes (or convert from existing sheet music). 


Written for use on an ATmega16 running at 1MHz. To select the tune to play, the tune simply needs to be the 'song' global variable, which is easily accomplished by compiling in the *.c file for the tune you want to play. 'Fuer Elise', 'Happy birthday' and 'The entertainer' are included, with 'The entertainer' currently set to be played.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
