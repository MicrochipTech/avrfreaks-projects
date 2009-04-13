# Uzebox Game Console

Uploaded by uze666 on 2009-04-13 01:58:00 (rating 0 out of 5)

## Summary

(Updated April 12,2009)


The Uzebox is a homebrew game console based on an AVR microcontroller. The design goal was to be simple to assemble for any hobbyist and great sound and graphics with absolute minimum hardware: an AtMega644 microcontroller, an AD725 RGB-to-NTSC converter...and a bunch of resistors! 


The current Uzebox kernel is capable of producing NTSC compatible video in resolutions up to 360x224 pixels with 256 colors. Sprites, horizontal and vertical scrolling, split screen regions, animation and other effects are all accomplished in software. A four channel wavetable synthesizer is included as well as a PCM audio sample playback mechanism. All audio voices are individually pitch and timbre controllable and mixed with adjustable volume levels in realtime.


The console sports good ol' NES, SNES (or possibly DB9) joysticks ports and a MIDI IN input to compose your own songs straight on the console. The kernel is fully interrupt driven to generates video sync, mix music, read joypads, etc. so games can be written in plain C, no insane cycle counting required :D ! 


Considering the 644 has only 4K of RAM and 64k of ROM, it was quite a challenge to cram all that stuff in there. The site has source code and schematics. Everything, including hardware design, is fully open source, licensed under the GNU GPL. 


Right now, there's about 20 games & demos including complete Tetris DS & Dr.Mario clones for the console. 


Two commercially available kits now, the Uzebox AVCore at Sparkfun & the Fuzebox by Adafruit!


Enjoy!


<http://belogic.com/uzebox>

## Links

- [Uzebox project Homepage](http://belogic.com/uzebox)
- [Sources, tools and schematics](http://belogic.com/uzebox/downloads.htm)
- [Prototype Board Preview Picture](http://belogic.com/uzebox/images/board_details.jpg)
- [Uzebox AVCore Picture](http://belogic.com/uzebox/images/avcore-gamecard.jpg)
- [YouTube Video Playing Megatris](http://www.youtube.com/watch?v=oHKiBjiAg8o)
- [New Youtube video featuring all games and demos](http://www.youtube.com/watch?v=MmbjgadImt8&fmt=22)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
