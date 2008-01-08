# ButtLoad 3.0 - Butterfly AVRISP

Uploaded by abcminiuser on 2008-01-08 05:41:00 (rating 0 out of 5)

## Summary

=====================  

 Update 8 JAN 2007:  

 Now compiles *only* under the latest WinAVR distribution, but now once again fits into the target flash ROM.  

=====================


Buttload is a replacement for an official AVRISP, but is open source and has more features. All you need to get it going for a minimal programmer is an AVRButterfly board, available from many online distributors (eg. AVRFreaks' member Smokey's site at [www.smileymicros.com](http://www.smileymicros.com)). Once loaded, the system's ready to go - no hardware modifications, existing programmers or bootstrap cables to worry about! Since all the changes are in software, all you need to revert your butterfly back into its previous condition is just reload your old firmware.


Because the firmware can be loaded via the Butterfly's existing bootloader, this programmer design does not suffer from the "chicken and egg" problem of other homemade programmers.


ButtLoad is a cheap alternative to the more expensive programmers, and is faster to build. Not only that, but it offers more features than the official AVRISP - and you can always use the Butterfly as a development board afterwards!


The hardware can be extended with minimal components and effort to create a more robust system capable of programming all AVRs at all operating voltages - please see the included manual in the Support directory of the download package.


---------------------------------------------------------------------  

Features:  

* AVRISP Emulation - works with AVRStudio and all AVRISP compatible software


* Self powered - does not need to be connected to a circuit to interface with AVRStudio


* Transparent support for large flash memory AVRs (up to 256KB)


* No extra parts or modifications required for basic 3V programming


* Recovery mode to repair AVRs whose fuses have been misconfigured for an external clock


* Can store a 256kb HEX file, EEPROM, fuse and lockbytes in non-volatile memory  

---------------------------------------------------------------------


The last one is what makes ButtLoad so special. Unlike other programmers, ButtLoad allows you to store a complete application into the on board Dataflash for later retrieval or computer-less programming. That means you can program your AVRs without the necessity of a computer after the initial storage!


A PDF manual and supporting materials are included in the \Support\ directory. Please post all questions, comments and bug reports at the current Academy thread at <http://tinyurl.com/cco7k>.


For those without the AVRGCC, or those whose WinAVR LibC versions are not up to date, a precompiled .HEX file is included. The code is released under the GPL license for now. You will need the latest WinAvR package installed to compile. Enjoy!


The package "ButtLoad USB Modification.zip" contains information and software for converting ButtLoad into a USB programmer. It has been compiled by Nard Awater, AVR Freaks user "Plons".


Please read the included manual in full before attempting to use ButtLoad.

## Tags

- Complete code with HW
