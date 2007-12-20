# AVGA demo

Uploaded by jery9871 on 2007-12-19 21:39:00 (rating 0 out of 5)

## Summary

Original idea (block referencing) was adopted from project AVR Pacman by Albert Seward. The MCU generates decomposite video signal consisting of SYNC, and 4bit pixelstream. No additional logic circuitry is needed. The video driver uses block-reference table compression. It can operate without any external memory. Blocks (tiles) are fixed width (8px) and can be stored in PGM or in RAM. This allows displaying floating images. For example - a sidescrolling "Commander Keen" - like game could be written on top. Driver should run in any AVR with elpm and one full 16bit timer/counter with PWM. The demo runs on ATMEGA168. Achieved resolution for PAL: 192*144*4bpp @ 19.6608MHz.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
