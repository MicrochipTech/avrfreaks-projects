# PS/2 keyboard library

Uploaded by Kipmans on 2009-04-12 14:39:00 (rating 0 out of 5)

## Summary

I’ve been playing with the idea to make an user interface for one of my (upcoming) projects using a computer keyboard.


As a first step of course I had to figure out a way to read data from the keyboard. I first searched for completed libraries to do this, but I was dissatisfied with all I could find. One of my top annoyances was the fact that almost all of these libraries contained one or more blocking functions, effectively blocking normal program flow when no keys were pressed.


So I started writing my own code to read data from a PS/2 keyboard. Along the way, I decided that it would be nice to pack it in “library” form so that it’s re-usable and possibly useful for other people.


As of yet, my library contains the following features:


* Reading pressed keys from a regular PS/2 keyboard, using entirely non-blocking functions.  

* Includes extended keys.  

* Extra lookup tables to assign other characters to keys when SHIFT or NUM LOCK is activated.  

* Accessible status register to read the status of SHIFT, CAPS LOCK, NUM LOCK, SCROLL LOCK, CTRL and ALT.  

* Supports bi-directional communication so it’s also possible to send commands to the keyboard.  

* Automatically sets the keyboard LEDs depending on the states of NUM LOCK, CAPS LOCK and SCROLL LOCK.


A few things however, are still missing and probably won’t be implemented:


* Support for PRINT SCREEN/SYSRQ and PAUSE/BREAK. These keys use a weird scancode and I don’t feel like implementing a lot of exceptions in my code for two keys that nobody uses anyway.


Usage of the code should be pretty straightforward. Just adapt the first few #define's in ps2kbd.h to your target and link your application with ps2kbd.c. ps2kbd\_test.c shows a very basic usage example. I have only included a very basic set of keys in the lookup tables in ps2kbd.c, but adding more keys is easy.

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
