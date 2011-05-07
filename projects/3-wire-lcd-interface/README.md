# 3 wire LCD interface

Uploaded by PortiaCyberLogic on 2011-05-07 02:58:00 (rating 0 out of 5)

## Summary

I searched for a way to save more MCU pins while connecting to LCD


I found some solutions talking about 3 wire LCD interfacing,  

All of them use the 4 bit transfer mode & I wondered why ?! especially that the MCU pin count remains the same, so why the rippling overhead ? !!.  

Also they use 74HC164 shift reg. while 74HC595 is in the same price & offers latching too..


So I modified some of these works to get a better solution..


1- I talked serially with the LCD via 74HC595 using 3 MCU pins.  

2- I used 8 bit mode to reduce the overhead.


Every thing is archived in the included zip..


After extracting you will find 2 folders  

1- LCD lib:  

Contains the h & c files that you can drop to your project, also it contains the schematic of use.  

2- LCD demo:  

Contains an AVRStudio demo project, check MainInc.h to see how to configure your LCD pins according your layout, also this folder contains a Proteus DSN file that simulate the demo project.


Any comments or questions are welcomed  

Portia CyberLogic  

portia2220"at"yahoo"dot"com

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
