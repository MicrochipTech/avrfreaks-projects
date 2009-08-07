# UC3B-based MP3-player

Uploaded by perchrc on 2009-08-07 13:31:00 (rating 0 out of 5)

## Summary

This project features a custom UC3B1 based MP3-player board with very simple hardware, and software. The software runs on EVK1101 as well, without modifications. It is based on the Helix MP3-decoder.


The board is 14.6 by 60.5 mm and features a mini-jack for headphones, joystick for navigation, a single LED, battery clamps for a single AAA-battery, USB-connector and uSD-connector.


The software is designed to read MP3-files from a FAT file system on the uSD-card (for EVK1101, standard SD-card), decode and play them in real-time. The user can skip files and adjust the volume using the joystick. The player only supports the MP3-format.


In addition to the UC3, the board features a tiny25-based buck-boost converter that creates 3.3V for the UC3 from the 1.5V AAA-battery or the 5V USB VBUS. The player can run on both.


The player uses two PWM channels for each audio channel to output the audio. The headphones are driven directly by the MCU pins. A filter on each audio channel is supposed to combine two PWM channels to achieve 16-bit resolution. This is a very simple setup, and while the audio quality is good enough for many purposes, it is not what you would expect from a hi-fi audio player.


The software is tested with 256 kbps @ 48MHz, which works fine. 320 kbps playback might work.


Apart from the UC3 and the tiny25, the only active components are the two transistors for the voltage converter.


The archive contains the following:


- The AVR32 Studio workspace.  

- A report describing the functionality, unresolved issues etc.  

- The board schematic as a PDF and PCB layout as a Protel pcb-file.  

- A bill of materials.  

- The source code for the buck-boost converter in assembly. (The program for the tiny25.)  

- A few pictures of the board.

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
