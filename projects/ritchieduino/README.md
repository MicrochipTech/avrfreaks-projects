# Ritchieduino

Uploaded by aiwarrior on 2012-03-18 13:44:00 (rating 0 out of 5)

## Summary

The Ritchieduino comprises much of the Arduino-0022 library functionality (pin compatibility), just completely written in C99 (not K&R C) and with some re-arrangement to make the code more modular. Also all the C++ code was mostly stripped off and the more important parts were partially re-written, namely the Serial, the SPI and Ethernet (w5100) libraries. Because of this re-writing the API changed significantly and i admit it is aimed at a more savvy audience than the original Arduino.


There is a guide in [http://www.pneves.net/2012/03/ho...](http://www.pneves.net/2012/03/how-to-setup-and-run-ritchieduino.html) for how to set up the environment needed to run and start using the library.


On the other hand as the including headers have been streamlined and the namespace cleaned some room for extra optimization control was gained. A clearer example is that mostly only the wiring library remained, which was already C. There were also some functions which were, in my opinion superfluous, that were cleaned out, namely re-naming of standard C functions, like the ones in WCharacter.h and WMath.h. I do reckon renaming is mostly inlined and doesn't bloat the code by itself but makes the analysis and deeper understanding of what is going on behind scenes mores obscure.


Still on the line of modularilization is the fact that in my opinion, Class Inheritance has been abused as to make Matroska doll code, where you have to go deeper and deeper to inspect what is happening under the hood, becoming even harder for newbies to start progressing to Intermediate level. An example of this is the print inheritance on pretty much every class, which sometimes calls code that is less than desirable, namely floating point code, which increases the size of the programs by some orders of magnitude.  

There is documentation on how to set everything up in Eclipse IDE with some hints that may help the transition to Ritchieduino and even normal Arduino programming a bit faster and satisfying. In the meantime if you can take care of using the library you can try the WebServer examples which is functionally equal to the one provided in the original Ethernet library.


In the meantime i leave here the github repository with library available to anyone who so desires. It is also important to note that the library is LGPL, because the previous Arduino work upon which Ritchieduino is derived, is also LGPL, so you can use it in proprietary applications.


The files can be had in [https://github.com/ptsneves/Ritc...](https://github.com/ptsneves/Ritchieduino)

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
