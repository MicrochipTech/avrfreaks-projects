# MIDI Merge for Tiny1634

Uploaded by Simonetta on 2014-10-25 21:31:00 (rating 5 out of 5)

## Summary

Mix two MIDI (Musical Instrument Digital Interface) signal inputs without stuck notes.


Have one channel be a PC MIDI sequencer and the other be the MIDI piano keyboard that you're playing.


This project uses an AVR with two USARTs to read each MIDI input message stream separately, parse the input bytes into MIDI messages, and send each message from both inputs to a single MIDI out.


Unlike all the other MIDI merge projects on the web, this one really works.  Handles Sys-Ex messages up to 64 bytes in size.

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
