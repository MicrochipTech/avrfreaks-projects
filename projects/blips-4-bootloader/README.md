# BLIPS 4 Bootloader

Uploaded by stevech on 2007-11-27 02:28:00 (rating 0 out of 5)

## Summary

See revision history, below.


BLIPS - bootloader for IP (LAN, wirleess LAN, etc) and Direct Serial connection to AVR serial port.


AVR side is just 512 bytes so it doesn't hog space in a mega8.  

PC side is a simple MS Windows GUI interface for IP or serial connections, with a simple built-in terminal emulator.


Does fast block-mode binary format downloads. Much faster than byte-at-a-time or ASCII formats.


Conforms to Atmel's AVR109 protocol so PC side loaders other than the one included here can be used, e.g., one for Linux or Windows batch files.


Supports mega128's large flash - RAMPZ tested OK.


Tested using serial port and ethernet LAN and WiFi connections to mega8 and mega128.


Also has built-in dumb terminal program.


AVR on-chip bootloader asm source is an Atmel Studio file that I hope is very easy to configure for your processor type and crystal and blinking LED or other boot by-pass strategy you prefer.  

============================================================  

Revision History


NOTE: Be sure to keep the "manifest" file in the same directory as the BLIPS EXE. This manifest file is in the uploads here. It allows the XP look and feel without you having to have .NET2 installed.


V4 - changed PC\_side software serial input timeout to 5 seconds; was 1 second.


V3.1  

25 Oct 07 added one line of code to the AVR side asm code. For LED blinking.


10 March 07 uploaded BLIPS 3.0 zip file and new AVR side asm code. Main change in 3.0 is optional support for command line/batch/script file invocation of BLIPS PC side.  

BLIPS 3.0 too large for AVRfreaks.net. So here it is  

<http://www.esnips.com/web/blips>


26 Nov 06 - uploaded BLIPS 2.8 EXE (PC side). Added: auto-reload hex files for flash/eeprom before programming begins. Changed: typo in baud rate for 1152000


BLIPS 2.7 - fixed bugs related to foreign language versions of Windows XP.


23 July - uploaded BLIPS 2.5 EXE to replace v2.4 and earlier. Use v2.1 if you need the OCXes, then replace EXE file with latest. Changes in this version are: (1) the Terminal Mode bug fixed in buffering and scrolling; (2) In terminal mode, added support for backspace (ASCII 8) affecting the display. 


22 August 06 - uploaded BLIPS 2.6 EXE to replace v2.5 and earlier. Use v2.1 if you need the OCXes, then replace EXE file. Bug in 2.5 caused failure to read XML part files for some non-english-language versions of Windows XP.


21 July - uploaded BLIPS 2.4 EXE to replace v2.3 and earlier. Use v2.1 if you need the OCXes, then replace EXE file v2.4. Changes in 2.4 are the Terminal Mode now has its own baud rate setting and local echo option. I believe that the "window won't close" bug is fixed.


20 July - uploaded BLIPS 2.3 EXE to replace v2.2 and v2.1. The only change was to the serial port discovery (enumeration) to avoid problems with non-English language port descriptions. See also below.


19 July - uploaded BLIPS 2.2 EXE to replace the v2.1 EXE. Hope it fixes the run time error 9 related to enumerating the COM ports (see forum discussion). The problem with the German version is still being investigated. To get the OCX's that BLIPS needs, use the v2.1 setup.exe then replace the EXE with 2.2.


12 July - uploaded SETUP.EXE version with OCXes in a cab file.


in the avrfreaks.net forum, German users report BLIPS won't run on the German language version of XP. Don't yet know why.  

-=-============================


9 July 2006. Changes vs. v2.0 are described in the PDF file.


BLIPS 2.1 - PC side bootloader and AVR-side mega128 and smaller ASM code. See BLIPS readme PDF document in the BLIPS zip file.


Tested somewhat with mega8 and mega128.  

More testing by others is needed!


>>> Note  

PC side used to be stored at the below URL. But now it's here on AVR Freaks.net


<http://www.esnips.com/web/blips>

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
