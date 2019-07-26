# Text programmer for Atmega 328

Uploaded by osbornema on 2019-07-26 10:00:00 (rating 5 out of 5)

## Summary

The text programmer places text strings in flash along side the application hex file. For applications in which the text strings have had to be restricted to enable it to run this might well be a good solution. Full code is provided for the text programmer and also for a simple application that can read and print out the strings.


 


The amount of text that can be embedded in a C-program using WinAVR is seriously limited by the size of the program. It is understood that this is because the text strings are copied into SRAM along with the user variables and data. The situation can be alleviated by placing the strings in EEPROM. However an alternative and better solution maybe to place the strings in some of the unused flash. For an Atmega 328 there is probably a lot more unused flash than there is spare EEPROM.


 


The “text programmer” presented here is loaded into flash at address 0x7000. Its hex file is appended to that of the main application that is loaded into flash at address zero. The operation is exactly the same as that outlined in the “Atmega 328 bootloader” posted here by osbornema on Friday 24 May. Following any reset the text programmer checks to see whether or not a text file containing the strings has already been downloaded. If is has, control is immediately passed to the main application. If it has not the text programmer runs and requests a text file. In practice the text programmer only runs once and the user will not normally be aware of its presence.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
