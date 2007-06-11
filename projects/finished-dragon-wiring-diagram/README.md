# Finished Dragon Wiring diagram

Uploaded by Simonetta on 2007-06-11 16:30:00 (rating 0 out of 5)

## Summary

The AVR dragon is a complex tool with too many configurations available for its small PCB. Therefore is shipped in an 'unfinished' state and the user makes the final wiring according to the devices and modes that will be used. The documentation on this process is correct, but limited.  

 Here is an example of a versatile final Dragon wiring arrangement. I use mostly the eight-pin Tiny11/85 series, the 14-pin Tiny24, and 28-pin Mega48. The Mega48 has debugWire but not high-voltage programming. It needs to use parallel programming in order to re-activate the reset pin after a deBugWire session. The Tiny11 doesn't have ISP programming or deBugWire, only high-voltage serial. The Tiny24 needs ProgramEnable pins connected. Other devices use the reset wire on the ISP connector for a deBugWire interface.  

 This JPG file is an extension of the wiring diagrams in the Dragon documentation. It shows the final wiring that I use to implement the above configurations. I hope that it will be of use to anyone who is having difficulty with the final wiring assembly of their Dragon.
