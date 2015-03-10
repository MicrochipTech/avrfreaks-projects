# USB Bulk Vendor example without ASF for the UC3B series

Uploaded by tominsf on 2015-03-09 20:11:00 (rating 5 out of 5)

## Summary

For those using the standalone toolchain, without the ASF, with a text editor and a makefile for development, this is a simple example of a USB bulk vendor implementation. I am using the simple interrupt system header file I've uploaded here recently, so no ASF is required. Optional is the USART i/o header file with its printf capability that I also recently uploaded – this software uses it to output status info to a terminal, but the code referencing it can be removed if you don't need the status output.


 


This program is specific to the UC3B parts because the interrupt system is, (as is the i/o system), but I expect that both of these could be easily modified to be used with the other UC3 parts.


 


All the program does is accept data – two bytes at a time – on endpoint 4 and immediately echo them back on endpoint 3. But it shouldn't be too hard to extend this to do useful tasks.


 


The major part of the program is the interrupt routine that does setup and enumeration. That out of the way, getting data to and from the controller over USB is almost trivial.


 


I'd recommend these two guides for understanding what's going on during setup:


 


<http://www.usbmadesimple.co.uk/>


 


[http://www.beyondlogic.org/usbnu...](http://www.beyondlogic.org/usbnutshell/usb1.shtml)


 


I have not implemented the DMA access that the USBB module is capable of – I'm leaving that for a little later. It obviously will be very useful for fast data streams. Instead, I write and read directly to and from the FIFO buffers. There is not nearly enough information on these in the spec sheet, and in fact there's no way to figure out their absolute location from the spec sheet. The only place on the entire web where I found that information was in one post on this forum. I don't remember who it was, but I'm very grateful to him. The FIFO buffer positions are all relative to AVR32\_USBB\_SLAVE\_ADDRESS, which is found in the appropriate processor header include file in the toolchain. FIFO0 is at that address, and each succeeding FIFO (FIFO1, FIFO2...) is 0x10000 higher. For the AVR32UC3B1256, this base address is 0xD0000000, but that changes depending on the model.


 


Each endpoint is either IN or OUT, except for endpoint 0, which although technically an IN endpoint, both transmits and receives, doing both from the same FIFO buffer. For transferring data on the other endpoints, all you have to do is to read bytes from successive byte locations in that register – up to the maximum size specified when you set up the endpoint, and the same goes for writing. Then do the appropriate interrupt bit clear (even if you're not using interrupts for read and write) and fifocon clear, and start the process over again.


 


Note that I've implemented the various setup-time descriptors as just arrays of bytes. It's ugly and needs fixing, but it works for now. And there are a few other things that need cleaning up, but this is only a demonstration, after all.


 


I've provided a very simple host-based program, based on libusb, to demonstrate the controller software. Its command line takes two byte-sized numbers as arguments, and the program sends them via USB to the controller, then retrieves them and prints them on screen. I don't know how easily it can be compiled under Windows; I tried to keep it simple so problems would be minimal.


 


For information on how the interrupt and i/o header stuff works, see my two project posts about them, and I've described the interrupt system in a forum post.


 


 


UPDATED 13 Mar 2105 to wait for TXINI to be set - indicating FIFO is free - before writing to FIFO.

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
- USB USBB UC3B
