# PCB_111000_UNO part 2: Assembly and programming the UNO and PCB_A

Uploaded by osbornema on 2020-07-02 13:51:00 (rating 0 out of 5)

## Summary

 


**INTRODUCTION**


 


PCB\_111000\_UNO has been developed for any one who would like:


      A brief introduction to the Atmega 328 hardware and the task of writing C programs for it. 


      Some pre-designed electronics that enables them to get started straight away. 


      A selection of tried and tested sample projects


 


In this case the pre-designed electronics consists of:


         An Arduino UNO 


        PCB111000\_1 that is designed to plug into it. 


 


PCB111000\_1 contains an 8 digit 7 segment display and some user switches. The pcbs are connected together with a programming interface and an I2C bus.


The 111000 refers to the 56 sample projects originally written for it.


 


The project is being presented in a series of three postings.


 


**Part\_1 of the project**


 


This introduced:


 


        PCB111000\_1 a plug in pcb for the UNO


       PCB\_111000\_UNO which consists of PCB111000\_1 plugged into a UNO.


      “UNO\_bootloader\_for\_hex&text\_V6.hex” a new bootloader for the UNO


 


**Part\_2 of the project**


 


This project (part 2) provides everything needed to complete the assembly of PCB\_111000\_UNO and program both Atmega 328 devices.


There are 3 attachments:


 


        pcb\_design\_and\_assembly.zip (which includes Eagle files)


        PDF file “Setting\_up\_PCB111000\_UNO”


        WinAVR\_and\_UNO\_files.zip


 


The third attachment WinAVR\_and\_UNO\_files.zip gives:


 


        Hex files for “PCB\_A\_Mini\_OS\_I2C\_V18” the mini-OS and bootloader for PCB\_A.


        Source files for the mini-OS


        Arduino project “Project\_programmer\_UNO”, enabling the UNO to program the PCB\_A device 


       Two sample user projects (Proj\_1B\_LEDdisplay and Proj\_2C1\_random\_LEDs\_UNO)


       A “Hello world” text file


 


**Part\_3 of the project**


 


Project PCB\_111000\_UNO part 3, to be posted later will give a selection of sample projects.


 


**Part 4: Compiler choice**


 


WinAVR, Atmel Studio and Arduino all have a great deal in common. Many projects can easily be transferred between them. And of course projects can use elements of all three. 


This is the case with PCB\_111000\_UNO.


 


           Most of the system code was developed with WinAVR


          The hardware relies on the UNO and Arduino compiler


           User projects are probably best developed using Atmel Studio


 


WinAVR was probably the obvious choice ten years ago, but more recently many programmers seem to have migrated to Atmel Studio. Arduino which offers, in one package, all the hardware and software needed to get started , has also become very popular especially with newcomers to micro-controller programming.


 


**Other resources**


 


A PC application normally used for sending hex files to the target processor hardware has not been written.   Instead a terminal program is used and the entire hex file is sent to the bootloader.


There are several excellent free terminal programs.  My favorite can be downloaded from  <https://sites.google.com/site/terminalbpp/>. But take care to download the version 20130820, other versions


may have an issue with the "scroll" button.


 


A good text is also a great help.  I recommend going to “[https://epdf.tips/c-programming-...](https://epdf.tips/c-programming-for-microcontrollers.html)” and downloading a copy of Jo Pardues book.  It is full of interesting background, examples and amusing anecdotes, though it was written for a different project, the AVR Butterfly.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
