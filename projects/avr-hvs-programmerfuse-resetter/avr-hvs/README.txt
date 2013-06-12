-------------------------------------------------------------------------------
AVR-HVS-Programmer/Fuse Resetter
-------------------------------------------------------------------------------
Version 1.000
Release date: 09/06/2013
-------------------------------------------------------------------------------
Credits
	Stefan Sykora (stefan.sykora@gmx.de)
-------------------------------------------------------------------------------
Project description

An implementation of the AVR high voltage serial programming protocol.
Supported devices:
ATtiny13
ATtiny24,44,84
ATtiny25,45,85

plus all the above with suffix "A"

It can also be used to reset the fuse bytes in case you disable Reset or
serial programming.

-------------------------------------------------------------------------------
Documentation

You'll need an Atmega8 or Atmega8A to flash with "hvsp.hex".
A schematic of the Hardware is included.

"programmer_windows" runs on your Windows PC and transfers data to the programmer.

It is a command line progam with the following input parameters:
-h high fue
-l low fuse
-e extended fuse
-f .hex-file
-c com-port
-d ATtiny device
-k lock mode (1,2,3);

of which f,d and c are mandatory.

An example call:
programmer_windows.exe -f test.hex -d attiny13 -c com1 -l 0xFF -h 0xFF -e 0xFF -k 3

"-d" accepts the following:
attiny13
attiny25
attiny45
attiny85
attiny24
attiny44
attiny84

-------------------------------------------------------------------------------