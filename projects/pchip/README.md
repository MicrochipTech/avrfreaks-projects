# PChip

Uploaded by Altenburg on 2003-11-11 07:58:00 (rating 0 out of 5)

## Summary

PChip stands for a Pascal programmable single chip system! The delivered Pascal compiler is a powerful tool generating stackbased pcode for a virtual machine running on the chip - here a Mega163. All you need is the chip and a RS232 3 wire cable. The IDE allowes compilation, download and source level debugging! Additionally you still can use the serial interface for terminal outputs or modem control...  

A small protoboard ist also given - I used Eagle in the freeware mode. All sources of the kernel are included. Special parts are the virtual machine and the bootloader - all together less than 4kB. With the Mega163 you have 12kB left for application. 


Use STK500 (or others) to program the bootloader to the Mega163 - all other updates can be done from the IDE.


Everybody is invited to port the kernel to other AVR types!

## Compilers

- AVR Studio 3

## Tags

- Complete code with HW
- AVR Studio 3
