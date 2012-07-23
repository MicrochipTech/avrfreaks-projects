# IsoJtagISP 2.5

Uploaded by CoronaFire on 2012-07-22 22:35:00 (rating 0 out of 5)

## Summary

The IsoJtagISP is an optically isolated usb programmer for Atmel AVR's that combines a JTAG programmer/debugger and an ISP programmer. The isolation means no more blown up serial/usb ports, which is a big plus for us designers.  

In JTAG mode it is fully compatible with Atmel's JTAGICE (Mk 1 only), and can be used any way the AVR product can, including programming and debugging with AVR Studio. In ISP mode it is an AVR910 compatible programmer, but with expanded device support and block mode programming. It can be used from AVR Studio via the AVR Prog program or any other 3rd party ISP programming software that supports the AVR910 programmer.


The code consists of bootloader and ISP firmware. The bootloader handles 'dual booting' between jtag mode (updating the jtag firmware from avr studio and running the jtag firmware) and isp mode (run the isp firmware).


The hardware is completely open design for you to build, schematics and pcb layouts provided.


Update: hardware v2.1, firmware v2.5  

The hardware update consists of a slightly smaller board and the microcontroller is now clocked from the usb chip, so no obscure crystal is required. The firmware update consists of improvements to the crystal changing code and updating the isp firmware to support block mode programing so that it only takes a few seconds to program devices that can handle it.


Firmware Update: v1.51  

* Tested auto-patching firmware with 8Mhz crystal and it works fine, so you no longer have to find the somewhat obscure 7.3728MHz or other usart divisible value.  

* Minor change to the crystal frequency setting in source to make it easier to set to custom value.


Hardware Update: v2.0  

New PCB layout, double sided board with much smaller footprint. v1.5 layout still published for those preferring single sided board.


Update: v1.5  

* 4 pin JTAG header to program on-board micro replaced with full 10 pin  

* Target Power LED added, USB power LED moved to USB end of board.  

Firmware:  

Added code to change crystal frequency, patches atmel jtag firmware when it's updated from AVR Studio.  

Activity LED now flashes while in boot-wait mode for updating jtag firmware.


Website now at:  

[http://www.alelec.net/saab-artic...](http://www.alelec.net/saab-articles/isojtagisp)

## Links

- [IsoJtagISP Website](http://www.floppyspongeonline.com/automation/isojtagisp/isojtagisp.html)

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
