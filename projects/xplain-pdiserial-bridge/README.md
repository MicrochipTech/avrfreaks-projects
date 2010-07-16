# XPLAIN PDI/Serial Bridge

Uploaded by abcminiuser on 2010-07-16 02:31:00 (rating 0 out of 5)

## Summary

-----------------------  

Updated 16/07/2010:  

 - Builds of the firmware are now available from [http://www.fourwalledcubicle.com...](http://www.fourwalledcubicle.com/XPLAIN.php) so that users do not need to download the LUFA source code and compile the project  

 - Much more accurate serial bridge mode, no more missed characters  

 - Serial bridge baud rate can now be set via the user's terminal  

-----------------------


In 2009, Atmel released one of their first demonstration boards for their new XMEGA AVR architecture, called the XPLAIN. This small board was a low-cost, feature packed way to evaluate the capabilities of Atmel's new devices.


One interesting aspect of the board's design is that it actually carries two AVRs; a new ATXMEGA128A1, and a regular AT90USB1287 AVR. The purpose of the latter was twofold; firstly, it was to provide a simple USB-to-Serial bridge for the XMEGA, and secondly it was to act as a programmer interface for the XMEGA. In this manner, the board would be able to be programmed and used by anyone, without the requirement for additional programmer hardware.


Unfortunately, as of mid 2010, the programmer feature of the board has not yet been implemented by Atmel, leaving many users with a cheap, but ultimately useless board unless they bought a JTAG programmer. To combat this, I modified the AVRISP-MKII Clone and USB-to-Serial projects in my LUFA USB AVR stack to produce an alternative AT90USB1287 firmware to Atmel's own which added in this missing functionality. 


When compiled and loaded into the XPLAIN board via the pre-programmed USB bootloader available on all but the very first hardware revision of the board, a simple jumper selection can choose between the programmer and serial bridge modes of the firmware. Since the programmer mode emulates an Atmel AVRISP-MKII programmer, it is fully AVRStudio compatible. 


 * Can be loaded onto the XPLAIN without additional hardware  

 * A jumper selects between USB-to-Serial and PDI Programmer modes  

 * XMEGA PDI programmer is AVRStudio compatible  

 * Works on all revisions of the XPLAIN (except the first)  

 * No additional hardware required for use or installation


For instructions on how to install the XPLAINBridge firmware onto an XPLAIN board, see [http://fourwalledcubicle.com/blo...](http://fourwalledcubicle.com/blog/archives/508). 


NOTE: AVRFreaks is currently broken and I cannot post a link to the XPLAIN Bridge specific project page ([http://www.fourwalledcubicle.com...](http://www.fourwalledcubicle.com/XPLAIN.php)). The download link below links to the LUFA project page, which contains the XPLAINBridge project source as part of the library download package.

## Links

- [Project Webpage, with download link](http://www.fourwalledcubicle.com/LUFA.php)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
