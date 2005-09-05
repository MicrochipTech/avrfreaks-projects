This project interfaces an ATmega32 and Siteplayer websever to control some home security features as well as some home automation.  
Probably the most unique feature is the custom bootloader which allows the use of the provided VB program to re-flash the AVR over the web.

This software was written by Edward Welch edwelch9000@hotmail.com
It is released under the GNU Public License, a copy of which is included in this distribution.
This software is release "As-Is" with no warranty whatsoever.
However, I will respond to questions as best as I can.

Updates new release etc can be found at www.edjusted.com under Projects

The bootloader folder contains the custom bootloader (note the bootloader only supports .hex files compiled as binarys, see the FORMAT parameter in the final code makefile )

The final code folder contains the firmware running on the ATmega32

The udp_email is a heavily modified version of siteplayers udp_email program available from www.siteplayer.com the version released with this distribution receives UDP packets from the sitplayer and will send an e-mail corresponding to the udp packet received.

Note:  I have never attempted to port any of this software to another computer and therefore do not know how easy such a feat will be accomplished.  I would almost guarantee the VB programming program will need to be modified to support a new environment.  My goal in releasing this software is to aid anyone in developing a similar project.  As I rely heavily on the open source community and would like to give back what I can.

Again, please visit www.edjusted.com and navigate to the Projects page for more information on this project.