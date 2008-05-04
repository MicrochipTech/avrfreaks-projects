# AVRsulabh- Parallel Programmer

Uploaded by boseji on 2008-05-04 02:58:00 (rating 0 out of 5)

## Summary

This is a development targeted to Stand alone AVR programming without PC and for Rescuing AVR microcontroller when wrong Lock Bits and Fuse bits are programmed.  

The attempt to make a AVR Parallel Programmer has been tried many times. Also many people who do development with AVR have STK500 or AVRDragon.  

But for people who dont have such facilities I have tried to put together a small cheap and easy innovation.  

I call it the Poor-Man's AVR Parallel Programmer - AVRsulabh.  

The name as it indicates - 'sulabh' in our Ancient Sanskrit language means "Making things Easy or Convenient". So I named it what its suited for.  

I have developed a main board which helps to generate the required voltages for Parallel Programming and controlling the Programming operations. There are different Target boards for Different Microcontrollers.


First Release V0.0 = 3rdMay2008  

---------------------------------  

Features:  

1. Erase Lock Bits  

2. Erase Flash  

3. Erase EEPROM (if EESAVE not Programmed)


Support & Tested:  

ATmega16,ATmega8,ATmeag8515


Docs & Release:  

1. Details of Project  

2. Picture of Schematics  

3. Zip File with Schematics & PCB in Eagle Format


!! ERRATA in REV0.0 -> Updated 4-May-2008 !!  

--------------------------------------------  

Problem :There was a mistake in the ATmega8 Adapter board  

 BS2 in ATmega8 HVPP connection require to be connected to PC2  

 and not PC5  

Effect :The AVR ATmega88 & ATmega8 would not erase properly  

Solution:Updated Schematics & PCB - Please update the same


Future Release:  

----------------------------------  

Wish List:  

1. Controller Based Progrmming  

2. Upgrade from Existing V0.0  

3. Fuse Erase and Restore to Normal


Any Suggestion and Support are always welcome,


Let make AVR Development Child's Play - Creative Living!!


AVR Rulez!!

## Links

- [Eagle Schematics & PCB Files](http://prog.ic.googlepages.com/AVRsulabh.zip)
- [Picture for the Present V0.0 Sulabh Schematics](http://prog.ic.googlepages.com/sulabh1.gif)
- [My Webpage Describing this Innovation](http://prog.ic.googlepages.com/avrsulabh)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
