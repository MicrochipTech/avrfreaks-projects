# Microbilt TAT 150

Uploaded by admin on 2003-07-14 21:44:00 (rating 0 out of 5)

## Summary

This project takes a Microbilt TAT 150 credit card validation terminal as used by Discover a while back, replaces the MCU with an ATmega8515 and a few mods with a goal to provide an inexpensive AVR development platform having an LCD, keypad, peizo, modem, DTMF generator and a mag card reader.


This could be used to implement an inexpensive card lock access terminal.


TAT 150s appear at various surplus outlets and I have seen them on EBay.


-------  

reader-revD.pdf (updated Aug 25, 2003)  

 - project summary  

 - pictures of PC board  

 - MCU I/O map  

 - Notes regarding reset circuitry changes needed and for ISP connection  

 - Detailed notes of mods needed to the hardware


I have completed the hardware mods and used AVRISP to read the signature bytes and examine the fuse configuration. The next step is software.


I think when it comes to doing the firmware, we may be able to use some software tidbits from some of the other projects here!  

 - The LCD uses the HD44780 which I have seen SW for here.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
