# In-circuit programmer for the Atmega 168 and 328 devices.

Uploaded by osbornema on 2019-02-22 11:43:00 (rating 0 out of 5)

## Summary

The serial programming algorithm outlined in the Atmega 328 data sheet has been implemented in C using WinAVR.


The resulting code enables hex files to be programmed to a target device using a terminal program such as [Br@y](mailto:Br@y)++ or Tera Term.


 


A version of this code has been successfully used in the development of PCB111000 a WinAVR taster pcb, details of which were posted


here by osbornema on Nov, 29 2018. In this pcb a slimmed down version of the code was loaded into the boot partition of an Atmega 328.


 


The code can be further developed or modified so that:


Other similar devices can also be programmed.


Several devices can be programmed in circuit using a single programming IC.


 


Two versions of this code have been developed:


 


Version 1: (this version) This programs the configuration bits and flash


Version 2: (to follow) This will also program the EEPROM with strings and numbers


 


Version 1 runs on an Atmega 168


Version 2 will probably require an Atmega 328.


 


User instructions plus circuit details are attached.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
