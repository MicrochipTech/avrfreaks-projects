# USBL - 256-word USB Bootloader

Uploaded by spacewrench on 2010-08-29 23:06:00 (rating 5 out of 5)

## Summary

My friend Paul, a talented engineer and programmer, makes [hobbyist ATmega widgets](http://www.pjrc.com/). He sells a couple of USB-based gadgets, and he mentioned that they come with a USB bootloader he wrote that fits in 256 words. I had been screwing around with USB on NXP-ARM (and not getting very far) and I was pretty skeptical. But I had a couple of Atmel chips that could do USB, so I gave it a try. It is, in fact, possible, and not really even that hard. Here is my implementation (along with a Qt application that can talk to the bootloader, upload and display the contents of Flash, erase Flash, and download new code. Perhaps it'll inspire somebody else to try doing it -- it's not terribly difficult, but it is a useful learning exercise (sort of like writing quines in a new language).


Paul's version is probably better than mine, since he's done all the hard work to make it work seamlessly on Windows, Mac and Linux. I've only tested on Linux, and only on my Ubuntu-9.04 laptop, with a self-compiled version of AVR-GCC. I'm an [IP lawyer](http://www.mersenne.com/) by day, and I don't have much interest in making a bulletproof, cross-platform version of this code. I just wanted to learn something about USB. And as far as that goes, Mission Accomplished!

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
