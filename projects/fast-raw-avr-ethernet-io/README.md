# Fast raw AVR ethernet for I/O

Uploaded by Rogier1980 on 2009-01-01 08:59:00 (rating 0 out of 5)

## Summary

Guido Socher ([www.tuxgraphics.org](http://www.tuxgraphics.org)) has an excellent project called "An AVR-based Ethernet device" that shows how to connect your AVR to a network, using the ENC28J60 ethernet controller chip. The included software is mainly useful if you want full-blown TCP or UDP. 


I however wanted to have a really fast, possibly real-time I/O system for controlling a robot from my PC. Therefore I made an implementation that allows for faster and possibly real-time communication, under the following assumptions:  

- The PC and one or more AVRs are the only ones on the network  

- The payload data is 255 bytes or less


This allows for raw ethernet communication without a higher-level protocol. The EtherType field of the ethernet protocol is used as a length field.  

Furthermore, I do not have to deal with MAC addresses in the AVR, because the AVR only sends broadcasts. Lastly I did some code optimizations in the transmit part.  

Now we have fast communication, and you could make it real-time by using a real-time extension for Linux (such as Xenomai+RTnet).


UPDATE: Guido has adopted this project and put a more extensive explanation on his website ([http://tuxgraphics.org/electroni...](http://tuxgraphics.org/electronics/200901/avr-ethernet-realtime.shtml))


UPDATE: note that you have to run the PC code as root because normal users cannot send raw ethernet packets

## Compilers

- AVR32 GNU Toolchain

## Tags

- Part-specific Lib.functions
- AVR32 GNU Toolchain
