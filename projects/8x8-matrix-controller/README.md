# 8x8 matrix controller

Uploaded by pvarelas on 2011-04-13 14:35:00 (rating 0 out of 5)

## Summary

The hardware is very simple and is consisted of the MCU (ATtiny2313), the LED matrix and four buttons. The MCU pins are directly connected to the pins of the matrix and the switches are multiplexed with some of the matrix' pins. The software scans the matrix and presents graphics, read from a frame buffer (a part of flash memory). There are two more frame buffers, for temporary storage of graphics, used by some effect-mechanisms. The program is also capable of regulating the power output of each LED, using an algorithm called MIBAM (MIrrored Bit Angle Modulation). The final version of the code packs a kind-of-rally game, a morphing-effect routine, a Cylon/K.I.T.T scanner and of cource the "Game of Life" :-)


You can watch more, on my YouTube channel: [www.youtube.com/pvarelas](http://www.youtube.com/pvarelas)


:-)

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
