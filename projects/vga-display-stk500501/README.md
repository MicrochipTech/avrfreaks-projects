# VGA Display for STK500/501

Uploaded by patchell on 2008-06-24 14:59:00 (rating 0 out of 5)

## Summary

There is a whole lot in here.  

This is really intended to show how to interface a Spartan 3 FPGA up to an Atmel ATMega2561 or ATMega128...(or any other AVR with an EBI).


Included in the zip file:  

1. Schematic of the cable to connect between the STK500/501 and a Spartan 3 Starter Board.  

2. Source code written in Verilog for the XC3S200 fpga.  

3. Source code for the ATMega2561.  

4. Source code for the Font Editor to make the character generator for the VGA display.  

5. Source code to convert a binary file to a COE file for ISE to load the character generator data.


Extras...I used a very simple RTOS that I wrote for exercising the hardware.


A note about the source code for the FPGA...ISE expects to find the COE file in a very specific place. This is a shortcoming of ISE. You may have to regenerate the character generator ram in order to get the design to compile again.


And hopefully, I have included everything you need to recreate this project.


Tools used:  

WinAVR, Xilinx ISE 9.1sp3 (web edition), Microsoft Visual C++ 6.0.


Hardware used:  

Spartan 3 Starter Board [http://www.digilentinc.com/Produ...](http://www.digilentinc.com/Products/Detail.cfm?Prod=S3BOARD&Nav1=Products&Nav2=Programmable)


STK500 and STK501


April 15, 2008


A word of caution. Make sure that the power supply on the STK500 is set to 3.3 volts. You set this by using the control port on the STK500 along with AVRStudio. I just got reminded about this. Somehow, my STK500 got reset back to 5 volts. The Spartan 3 has 3.3 volt tolerent I/O, but not 5 volts. My Spartan 3 is now in silicon heaven.


I am planning to do a small PC board to do the interface. I will be putting resistors in series with the IO lines to help prevent future disasters.


April 17, 2008


Added a screen shot of the VGA display.


June 20, 2008


Well, I finally uploaded the schematics and pc board for the interface between the STK500/501 and the Digilent Spartan 3 starter kit. These files are yours free to use. The cam files are in two flavors. The REV A version is the one I actually built. I put a MIDI interface on it, and the connectors were about 10 mils too close together...(I made them fit). I still have not gotten around to testing the Midi interface yet. There just hasn't been any time...(I am in the middle of a medical crisis). The REV B cam files should fix the connector spacing problem. But, I am not going to build it. So I am not sure in the layout is any good. I have also supplied the cad files. The schematic was done in Orcad 9.2, and the PC board was layed out with Protel 2.7.


Again, these are public domain...they may be used as you see fit.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
