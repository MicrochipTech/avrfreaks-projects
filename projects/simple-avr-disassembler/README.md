# Simple AVR Disassembler

Uploaded by Martino06 on 2013-09-06 12:28:00 (rating 0 out of 5)

## Summary

Hello,


one and half year after the publication I`ve received a message,  

that the program doesn`t work properly with the correct HEX and INC files. It was caused by the "no-compromise" settings; the disassembler worked only with the existing instructions. The projects developed and compiled in the Atmel Studio or other IDEs contain data fields or comments like "+++Project XY+++, compiled on..." or "Arduino ([www.arduino.cc](http://www.arduino.cc))", which are not a part of the code and  

have caused the break. I personally see little point in disassembling of such projects, but because the people feel like doing it,  

here is the re-compiled version with following changes:  

- Data or unknown opcode word is replaced by "???", followed by the hex an char representation of the two bytes,  

- not programed words 0xFFFF (if any in .HEX) are marked by "-", so the user can see the entire flash,  

- the max. value of the "Label\_"-counter was increased from 500 to 5000,  

- the instructions lac, las, lat, xch were added.  

- new functions Reset HEX, Reset HEX and INC.


+++++ 04.04.2012 +++++++++++++++++++++++++++++++++++++++++++++++++++++++  

This project from the field of "reverse analysis" is a very simple disassembling tool for AVR Atmel devices and it could be  

useful for hobby and professional AVR programmers as well. 


The ASM file - as the conversion product from the code in the standard (Intel) HEX format - is easy to survey and unproblematic  

compilable after the upload in the AVR Studio. The 50 kB program resolves the instructions on the basis of an arithmetical algorithm,  

the only extern data source which the program needs is the INC file - for the registers and bits naming purposes. 


REQUIREMENTS:  

It is necessary to install the latest JRE and then copy the files Disassembler.bat and Disassembler.jar in the same folder. 


START:  

1. Start the BAT file  

2. Choose the appropriate INC file - it is very important that the INC  

 file corresponds with the device for which is the HEX file  

 programmed. If the INC and HEX files are not compatible, the program  

 can catch an unexpected exception. For this reason  

 is implemented a watchdog, which closes the program after 15 sec. of  

 idle time. (The program has no possibility to verify the  

 correctness of the INC file)  

3. Choose the HEX file. The disassembling process starts immediately  

 after the choice.  

4. The ASM text can be copied (right mouse button), or saved as ASM file  

 (menu).


All important messages will appear in the log part of the program.
