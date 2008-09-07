# MyUSB Vendor defined demo

Uploaded by prime on 2008-09-07 15:38:00 (rating 0 out of 5)

## Summary

This is a demo of vendor using a vendor defined HID type to communicate between with a USB device using a USB enabled AVR.


The demo comes in two parts the, firmware for the AVR which is written in C, and uses Dean Camera's MyUSB library which can be obtained from


[http://www.fourwalledcubicle.com...](http://www.fourwalledcubicle.com/MyUSB.php)


The demo was compiled with version 1.5.2 of the MyUSB library, it may require modification for later versions of the library.


The PC end is written in Delphi and requires the Jedi VCL components installed, which can be obtained at <http://www.delphi-jedi.org>.  

This was compiled using Delphi version 7, but should compile under later (and possibly earlier) versions too.


The source archive should be unpacked in the root of the folder in which you have MyUSB installed. The structure below this is as follows 


Apps/VendorDefined : Source and makefiles for AVR end.  

Apps/VendorDefined/PC : Delphi source and project files.


Any questions or comments, I can be contacted on AVRFreaks, as user prime or by email as afra AT aurigae DOT demon DOT co DOT uk.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
