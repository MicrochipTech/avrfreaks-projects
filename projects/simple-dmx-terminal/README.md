# Simple DMX Terminal

Uploaded by strenhel on 2010-01-09 11:09:00 (rating 0 out of 5)

## Summary

Tool used together with a simple terminal emulation ( I used termite) to control DMX-Equipment. Ideal to test equipment with simple commands. Not for complex control tasks.


The tool  

- gets commands from PC over the USB/Serial port  

- build DMX packages  

- send DMX packages to the DMX interface  

- automatically every given time interval (default ~1sec)  

- manually by command  

Hardware: Program was developed and tested on a mySmartControl-M168 board (<http://shop.myavr.com/>) connected and powered via USB to a PC. To drive DMX signals a small adapter is necessary with a 75176 driver and few resistors (see DMXControlAdapter.pdf).  

For DMX output PortC is used: Bit 0 : DMX Data, Bit 1 : Enable Driver  

USB PC: Using virtual Comport driver on COM3, 9600 Baud, 8,n,1 (s. <http://shop.myavr.com>)  

Software: GCC Toolset used with AVR Studio 4.18.  

Written by Gerhard Lehnerts, Germany, 2009  

[Gerhard.Lehnerts@t-online.de](mailto:Gerhard.Lehnerts@t-online.de)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
