# OSXAVR

Uploaded by ddd7 on 2008-02-02 18:44:00 (rating 0 out of 5)

## Summary

02FEB08


I have been looking at AvrMacPack as a recommended platform. They have done a really nice job ob doing everything that I was trying to do and they also have paid attention to a lot of detail that I haven't. 


Please look at this and tell me what you think. I am considering dropping OSX-AVR and focusing on other underserved platform pieces if everyone agrees that it is done as well as I think it is. 


<http://www.obdev.at/products/avrmacpack/download-de.html>


I will be contacting them to see if they can consider bundling nick's x-code templates or I may put any pieces that are missing as individual packages.  

01JUN07


I will be putting out a new release as soon as I figure out what EW and Jeorg have been up to lately. I have been busy using the chain and working on some other projects but I will be getting back to this corner soon.


Email me if you have requests....


Don.


24FEB07  

The gcc-4.1.1 version is up at sourceforge. The installer may complain if you are installing over the december release. This is not as well tested as I would like so any feedback would be highly appreciated.


There are still many complaints about avrdude and older systems and I will try to get to these. (....Compared to the tool chain avrdude is pretty easy to build....) 


31DEC06 I am releasing a new version which adds gdb, avarice, and simulavr to ther previous release. I also added a patched version of avrdude (which resolves some of the issues around the intel macs and avrdude) and Nick made some corrections to the Xcode templates. Right now only the intel version is posted but I wil lhave the ppc version up in the next day or so. 


(21 Jan 07: Give or take a few weeks :) PPC is done and will be posted.)


Happy new year!


27OCT06. The intel binaries are up for testing!  

17OCT06. This project is now up at [sourceforge](https://sourceforge.net/project/showfiles.php?group_id=177221).  

The first release for contains.


avr-gcc 2.4.6 (patched as of 05oct06)  

avr-binutils 2.17 (patched as of 05oct06)  

avr-libc 1.4.5  

Avrdude 5.2  

Avra 1.0.3  

Xcode Templates  

Mfile.


(...I need to come up with a more positive introduction to this...)  

A year ago after looking at 6 months of either broken, out of date, or unweildy results from the darwinports and fink collections I found myself buidling avr-gcc for OSX, and frankly every piece of the chain (assembler, programmer) was broken, out of date or unweildy on some level. And if you have to build it you might as well package it. 


This is an installable package for OSX (tested on panther and tiger). 


It origionally contained patched versions of 


avr-gcc-3.4.5  

avr-binutils-2.16.1  

and libc-1.4.3


This includes the new\_devices patches posted to the freebsd ports collection as of 01MAY06.


It installs into /usr/local by default but should be relocatable at install time.


A description of the process used to build this package is at


<http://www.digithink.com/embedded/AVR/OSXToolChain>


This is not an unfinished product how ever it is being worked on and will eventually become a tool and some sort of cooperative effort. In the mean time the osxavr download is the avr tool chain plus avrdude and my patched version of avra1.0.1. 


At present (23jun06) the avrdude is being rebuilt. The Toolchain and Avra in the osxavr should be good to go. ddd7


The thread for much of this is at 
href= [https://www.avrfreaks.net/index.p...](https://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=38593)  

>[https://www.avrfreaks.net/index.p...](https://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=38593)


07 AUG 06.  

I have found a couple of threads and their coresponding solutions for the avrdude package. When I get these up I will aslo roll in the xcode templates and wuenches tcl stuff. --ddd


09 Sep 06 Finally got the stuff I built last month along with nicks xcode templates into a dmg and built a meta package for it. I have tested this on g3s and g4s running panther and tiger.

## Links

- [Avr-gcc plus libc for OSX 10.3](http://www.digithink.com/embedded/avr/AVRGCC-LIBC-20060501.dmg)
- [OSXAVR (avr-gcc, libc, avrdude, avra)](http://www.digithink.com/embedded/avr/OSXAVR-10JUN06.dmg)
- [OSXAVR (Aug06)](http://www.digithink.com/embedded/avr/OSXAVRAUG06b.dmg)
- [OSX-AVR at sourceforge](https://sourceforge.net/project/showfiles.php?group_id=177221)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
