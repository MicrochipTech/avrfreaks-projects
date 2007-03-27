# USB SD Card Reader

Uploaded by bozai on 2007-03-27 11:47:00 (rating 0 out of 5)

## Summary

2007-03-10:  

This project is for studying USB interface and MassStorage protocol.  

Perhaps, because I should not using AVR device which has no extended bus,  

or should not using SD card as the medium but others which has parallel interface. The speed of this work is quite slow, Read 370KB/s and Write 195KB/s.  

But this project is quite simple for studying USB.


The function of this board is USB SD card reader. Since It adopt MassStorage protocol, It will be treated as a removable disk when plug in the computer, just like a flsh disk.  

Using the attached code it will be shown as a hard disk, because I can use the function of the hard disk (eg, partition). Of course just change a character of the code it will be a removable disk.


This is my diy work, so I only draw the shcematic. If you like you can make one.  

By the way, ATmega8 88 are all right for it.


Sorry for my English!


Thanks!


Qibo Zhang


2007-03-27:  

Soory, a friend found a bug of it. And it was fixed now.  

It's a coincidence of my previous code can work.  

Thanks!

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
