# gps demo for EVK1100

Uploaded by MarcusW on 2008-11-22 20:45:00 (rating 0 out of 5)

## Summary

This is just some getting started code for the EVK1100. Connect a GPS to the DB9 on UART1 via a null modem adapter, and a male-male gender changer. Time, Lat, Lon, and Alt is displayed on the LCD. There is some pretty good parsing code and interrupt implementation that might be useful.


lat/long sign should be correct for anywhere in the world :-)


Parses GPGGA and GPVTG strings.


How to get it on the UC3A:  

1, NEW-> AVR C Project  

2, Select MCU and give it a name  

3, right click on your project in the works space and select import. Import all the files, especially the .cproject, .framewor, and .project. and press ok/import  

4, right click on project and build it. Should be work fine, just did it myself. Be sure step 3 works out correctly though - that messed me up at first.


Enjoy!  

Marcus

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
