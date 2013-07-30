# BACnet MS/TP Device and Library

Uploaded by stevekarg on 2013-07-30 12:40:00 (rating 0 out of 5)

## Summary

I used the open source BACnet Stack at Sourceforge to make a minimal BACnet MS/TP (RS-485 protocol) device using an Atmega168. The I/O included an RS-485 transceiver, a dip switch for MAC address, and some LEDs. I took the device to the BACnet Interoperability Workshop in 2007 and successfully interfaced it with many BACnet vendor products.


The BACnet protocol stack library at SourceForge provides a BACnet application layer, network layer and media access (MAC) layer communications services. It is an open source, royalty-free library for an embedded system, Windows, Linux, or other operating system. Example BACnet client and server applications are included. The project home page is at [bacnet.sourceforge.net](http://bacnet.sourceforge.net/).


BACnet - A Data Communication Protocol for Building Automation and Control Networks - see bacnet.org. BACnet is a standard data communication protocol for Building Automation and Control Networks. BACnet is an open protocol, which means anyone can contribute to the standard, and anyone may use it. The only caveat is that the BACnet standard document itself is copyrighted by ASHRAE, and they sell the document to help defray costs of developing and maintaining the standard (just like IEEE or ANSI or ISO).


For software developers, the BACnet protocol is a standard way to send and receive messages on the wire containing data that is understood by other BACnet compliant devices. The BACnet standard defines a standard way to communicate over a number of wires, known as Data Link/Physical Layers: Ethernet, EIA-485, EIA-232, ARCNET, and LonTalk. The BACnet standard also defines a standard way to communicate using UDP, IP and HTTP (Web Services).


The BACnet protocol stack implementation is specifically designed for the embedded BACnet appliance, using a GPL with exception license (like eCos), which means that any changes to the core code that are distributed are made available, but the BACnet library can be linked to proprietary code without it becoming licensed under the GPL. See the eCos license overview for easy to read details about this exception to the GPL. The license does not require users to release the source code of any applications that are developed with this BACnet stack - only portions of the BACnet stack that have been modified. Note that those files in this BACnet stack that are expected to be modified are licensed using the MIT License.


The text of the GPL exception included in each source file is as follows:  

"As a special exception, if other files instantiate templates or use macros or inline functions from this file, or you compile this file and link it with other works to produce a work based on this file, this file does not by itself cause the resulting work to be covered by the GNU General Public License. However the source code for this file must still be made available in accordance with section (3) of the GNU General Public License."


The [source code](http://sourceforge.net/projects/bacnet/files/) is written in C for portability.  

See ports/atmega168 and ports/bdk-atxx4-mstp folders for the AVR projects.  

The latest released version is [bacnet-stack-0.8.0](http://prdownloads.sourceforge.net/bacnet/bacnet-stack-0.8.0.zip)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
