# APL - VGA, Audio and PS2 keyboard library

Uploaded by akund on 2021-04-23 22:45:00 (rating 5 out of 5)

## Summary

Hello AVR fan!


 


I'm happy to share on [GitHub](https://github.com/akund/APL)my [VGA, PS/2 keyboard and Sound library](https://forum.arduino.cc/t/vga-ps-2-keyboard-and-sound-library-for-arduino-uno-nano/643713) with the following features:


* Timer Interrupt based implementation (execution not impacted by the main loop)
* Low RAM footprint by tile rendering from PGM memory (4 clocks / pixel)
* Tile size is 6 px by 8 px (Text mode) or 8 px by 8 px (GraphicPgm and Graphic modes)
* Runtime tile creation for the Graphic mode with tiles allocated in the RAM
* Fast screen horizontal and vertical pixel scrolling for the GraphicPgm and Graphic modes
* Optional hardware pixel multiplexer for RGB capability
* [PS/2 keyboard](https://www.arduinolibraries.info/libraries/ps2-keyboard) support
* Sound Tone from 45 Hz to 12 KHz

## Links

- [APL Github](https://github.com/akund/APL)

## Compilers

- Atmel Studio 7

## Tags

- Complete code with HW
- Atmel Studio 7
- #vga
- #ps2
- # Atmel studio #atmega328p
