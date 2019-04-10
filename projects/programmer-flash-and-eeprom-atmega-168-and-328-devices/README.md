# Programmer for flash and EEPROM of Atmega 168 and 328 devices.

Uploaded by osbornema on 2019-04-10 15:50:00 (rating 0 out of 5)

## Summary

**Introducing version 2.3\_UNO.**   


The Arduino UNO is an obvious place to do a quick test on a bit of unfamiliar code such as this programmer.  However Arduino and WinAVR use slightly different compiler settings.  Most of these generate error messages and can be fixed quite easily.


However Arduino treats chars and signed while WinAVR treats them as unsigned.  Finally the UNO provides a 16MHz clock while the programmer requires 8MHz.  This is easily fixed using the system clock prescaler.  Hopefully these issues have all been resolved in issue 2.3 a copy of which is attached.


 


 


Details of an In-circuit programmer for the Atmega 168 and 328 devices was posted by osbornema on Feb 22 2019. It was stated in the introduction to this project that an upgrade would be posted giving additional subroutines required to program the EEPROM.


 


This posting contains the upgrade. A basic client program is also included that prints out the strings saved to EEPROM and also reads the numeric data and does some simple arithmetic.


 


The text strings and data to be programmed into the EEPROM are initially saved to a .txt file. This file has several sections. The first contains explanatory text which will be ignored by the programmer. Text strings are then placed between a pair of -”- characters.


Each string is terminated by a -cr- or cr/lf combination.


After the second -”- data can be appended in either decimal or hex format. All text such as data names for example is ignored. Data items are separated from one another by a space, tab or -cr-.


The advantage of using hex format is that integer numbers can be combined for form long numbers or split to form char numbers.


 


Because of the time required to burn data to EEPROM a file cannot simply be downloaded at 57.6kB, the speed used to download hex program files.


The speed can either be reduced considerably or the file can be downloaded several times.


It was felt that the latter approach generated less hassle for the user and has been adopted here.


Each time the file is downloaded in full but only 256 chars or less are stored to an array in SRAM.


Every time that this array fills and at the end of the file its contents are copied to EEPROM.


 


It is recommended that the programmer runs on an Atmega 328 because of the strings it uses which are all saved in flash to simplify setting the device up. It has been used successfully to program the flash and EEPROM of both Atmega 168 and 328 devices during tests.


 


Details of the circuit are given in the posting of Feb 22 and are not repeated here. The user guide has been modified slightly and expanded to include details of the EEPROM programmer and client program.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
