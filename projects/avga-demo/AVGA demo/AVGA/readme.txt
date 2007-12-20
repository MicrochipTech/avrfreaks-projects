=================================================================================
# AVGA v0.1
# Author: Jerry981 (Jaromir Dvorak, mailto: md@unicode.cz)
=================================================================================


Description: 
AVGA is software-implemented color AVr Graphics Array for video game development.


Original idea (block referencing) was adopted from project AVR Pacman by Albert 
Seward. The MCU generates decomposite video signal consisting of SYNC, and 4bit 
pixelstream. No additional logic circuitry is needed.

The video driver uses block-reference table compression. It can operate without
any external memory. Blocks (tiles) are fixed width (8px) and can be stored in
PGM or in RAM. This allows displaying floating images. Driver also supports 7px
horizontal scrolling. Can be used to show scrolling backgrounds. Precise video
signal generator is interrupt driven, designed to save as much time as possible.
System should run in any AVR device with elpm and one full 16bit timer/counter
with PWM.

Video generator can be extensively configured, see driver.h and video.h. Various
modules for drawing are included in /utils/. See their header files for detailed
description.


