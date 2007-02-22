# Digital Fuse with ATtiny45 and BTS640

Uploaded by gerdsperling on 2007-02-22 16:16:00 (rating 0 out of 5)

## Summary

This project describes the use of Power Switch BTS640 (Infineon).  

Together with a small ATTiny45 it's a smart solution for a 12V digital fuse.  

Features:  

- Simple Button to switch ON/OFF  

- Serial 8N1 9600 baud TX  

- Short circuit protection  

- Current limitation  

- Overload protection  

- Thermal shutdown  

- Digital adjustable Fuse with proportional load current sense


How it works:  

A simple button is switching the Power Switch BTS640 (max 12A). It's load current sense signal is used to calculate the load current and switch the load off after exceeding a predefined value (like a fuse).  

Also a delay time can be defined so that the predefined max current can be exceeded for e.g. 3/10 sec after switching on. So switching a load, which needs a little bit more current for a short time is no problem.  

The load current is transmitted via 8N1 9600 baud to FTDI232R (see project 'Simple ATtiny45 USART ').  

Source, BTS datasheet, schematic and pictures of prototype included.

## Compilers

- ImageCraft ICCAVR V6

## Tags

- Complete code with HW
- ImageCraft ICCAVR V6
