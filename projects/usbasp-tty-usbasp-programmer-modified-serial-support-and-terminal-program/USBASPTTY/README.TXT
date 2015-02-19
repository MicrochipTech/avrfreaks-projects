usbasp+ is a modification to the wonderful usbasp programmer with added support 
for the on chip UART for communicating with your AVR projects through the programmer device.

When combined with the PC host component USBASP_tty.exe you wil be able to send and
recieve TTL level serial data to/from four microcontroller project from the programmer.

The firmware is targeted for the ATMega8 but could be ported to other devices
with more than 4K of flash (sorry but I could not make the modified code fit
into the 4K of the ATMega48... maybe you can)

If you have a version of usbasp built with an ATMega8 according to the 
schematic here: http://www.fischl.de/usbasp/  you can simply re-flash the 
firmware using main.hex supplied in this archive.
Otherwise you will need to modify the makefile for your chip & clock.

Once that's done, and assuming you have the usbasp drivers installed
you should be able to run the USBASP_tty.exe host program.
If all is well it should show that it detected the programmer in the status
field at the bottom of the screen. Choosing "Send CMD1" from the Commands
menu should cause the 2 on-board LEDs to blink for about a second.
If you've got this far things are working very well indeed.

Next step migt be to add the connections to RXD & TXD signals (PD0 & PD1) if 
your programmer dosen't already have these as depicted in the schematic.
If you purchased one of the many versions of this programmer available on ebay
these connections are most likely NOT made. Fortunately, with some careful soldering
you can easily connect these to 2 of the unused pins on the 10 pin connector.
These unused pins are typically connected to GND so insulate them well.  

With the connections made a simple test might be to connect RXD & TXD to a
USB-TTL serial converter (if you have one) and fire up your terminal program
(like HyperTerm or PuTTy). 9600 is the default baud rate.  
Characters typed into the terminal window should show up in the USBASP_tty window 
and characters entered into USBASP_tty window should show up in the terminal window. 

Finally, make the connections to your microcontroller project and enjoy being able to 
use a single device to program and communicate with your device. 

Files in the usbasp+ folder are the modifide USBASP programmer firmware files.
Files in the USBASP_tty folder are the sources for the Windows PC terminal program.
The PC program is built with Code:Blocks the IDE for the MinGW Gcc compiler and the 
USBASP_tty.cbp is the project file for this IDE.

Enjoy.
Eric   (emklausATgmail.com)


USBasp         http://www.fischl.de/usbasp/
WinAVR         http://winavr.sourceforge.net/                    
libusb-win32   http://sourceforge.net/projects/libusb-win32/
Code::Blocks   http://www.codeblocks.org/
MinGW          http://www.mingw.org/