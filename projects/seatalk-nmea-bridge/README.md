# SeaTalk Nmea Bridge

Uploaded by fwallenwein on 2013-01-07 12:52:00 (rating 0 out of 5)

## Summary

The SeaTalk <-> NMEA Bridge / Repeater is a single chip computer, that translates information between SeaTalk and NMEA Protocol.  

It can read & write NMEA ( i.e. from a GPS) and read & write SeaTalk data.


This device reads infomation like Depth, Speed, Water temperature, Windspeed, Windangle, etc. from Seatalk devices and send them to  

a NMEA output.  

It also reads information like Speed over Ground, Position, etc. from a NMEA input port and sends them to the SeaTalk bus. 


I have added a spezial NMEA Tag that allows you to send ANY SeaTalk  

data to the seatalk bus.  

( Send $STALK,xx,yy,zz from a PC and it will be sent to the SeaTalk Bus )


It has an optional LCD display to show navigation info's like position,speed,depth, wind speed etc. etc. 


The bridge has 1 SeaTalk bus connector and 1 serial port for NMEA etc.


* ATmega32 Processor ( also Atmega128 and Atmega1284p Version avaliable )  

* 1 * SeaTalk Port ( read & write SeaTalk )  

* 1 * NMEA IN & OUT Port ( RS232 or RS422 )  

* On board volatage regulator  

* LCD display connector  

* Boot loader (to update to new Software Versions)  

* ..... 


I've also made a USB SeaTalk NMEA Bridge  

News & Info at the Project homepage


I tried to upload the project sources.  

For some reason it did not work. AVR Freaks came back with an error message. If you like to get the Project files and the sources, just send me a mail to [fwallenwein@gadgetpool.de](mailto:fwallenwein@gadgetpool.de)

## Links

- [Project Homepage](http://www.gadgetpool.de)
- [NMEA & SeaTalk discussion group](http://www.gadgetpool.de/nuke/modules.php?name=Forums)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
