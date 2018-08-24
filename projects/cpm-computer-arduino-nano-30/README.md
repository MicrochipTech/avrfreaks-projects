# CP/M computer on Arduino Nano 3.0

Uploaded by FoxyLab on 2018-08-23 21:37:00 (rating 5 out of 5)

## Summary

Hello!![smiley](https://community.atmel.com/profiles/commons/libraries/ckeditor/plugins/smiley/images/regular_smile.png "smiley")  

I will begin here with a description (on english) of my project **cpm4nano** - a CP/M-compatible computer based on Arduino Nano 3.0.  

Currently one (!) Arduino Nano (!) in my project:  

- emulates the i8080 processor, 64 KBytes RAM+additional banks, 4 FDD;  

- loads the OS CP/M and runs its programs;  

- exchanges information with PC on the serial port, including uploading files to the emulator using the XMODEM protocol;  

- with the help of the converter PS/2-UART allows you to enter information from the keyboard;  

- displays text information on the TV screen (20 rows with 40 cols x 2 virtual screens) (without jitter!).


 


**Youtube** -


my homebrew Arduino Nano-based computer (recording with TV tuner): power on, memory test, CP/M loading, DIR command, virtual screens toggle, CPU test, BASIC:  

<https://youtu.be/NxQ2Omf0swM>


<https://youtu.be/Yuq1yEH-WZ8>  

(earlier version) - <https://www.youtube.com/watch?v=LHFmt3qWAuY&feature=youtu.be>


**Description (on russian)** - <https://acdc.foxylab.com/node/76>


 


Current project scematic:  

![](https://acdc.foxylab.com/sites/default/files/cpm4nano_mkV.png)


 


PS/2-UART converter:


[![](https://acdc.foxylab.com/sites/default/files/ps2_uart_cir.png)](https://acdc.foxylab.com/sites/default/files/ps2_uart_cir.png)


 


*Minimal configuration:*  

Arduino Nano 3.0 + SD card with adapter + USB-UART converter  

*Maximal configuration (currently)* -  

Arduino Nano 3.0 + SD card with adapter + USB-UART converter or PS/2-UART converter + 1 or more FRAM chips FM24C256 + passive adapter for composite TV-input


 


Displaying the font on the TV screen after the redesign and adding symbols of the pseudographics and Russian letters in the KOI-8 encoding:  

![](https://acdc.foxylab.com/sites/default/files/cpm4nano_font.jpg)


 


For example, the "screenshot" of the TV screen shows the results of the DIR command and the CPU tests:  

![](http://acdc.foxylab.com/sites/default/files/cpm4nano_tv_1.png)


 


Compilation of the program TOWERS.ADA for solving the problem of the "Hanoi Tower" (output on terminal program):  

![](https://acdc.foxylab.com/sites/default/files/towers_ada_comp.png)


 


P.S.


The project is unfrozen :-)


Successful tests of connection the PS/2-keyboard via the STM8 adapter made my "nanocomputer" autonomous.  

![](https://acdc.foxylab.com/fox_ok.png)  

So **cpm4nano Mk V**:  

Arduino Nano 3.0 - emulates the i8080, provides work with memory and disks, performs video output and serial communication  

RAM - 2 x 32 KB FM24C256-G I2C-chips  

FDD - SD-card  

input - PS/2-keyboard via STM8S103F3P6 adapter  

output - in parallel to the video output (20 lines x 45 columns, font 4 x 8) and serial port (USB-UART converter)  

![](https://acdc.foxylab.com/sites/default/files/cpm4nano_MkV_case.jpg)


P.S. I tested the connection to the tube TV (the clarity of the image is worse than on the LCD, but the synchronization is not broken and the font is readable).


I added to the "nanocomputer" the possibility of input-output via an audio file or magnetic tape.  

Board for matching voltage levels (classic solution):  

![](https://acdc.foxylab.com/sites/default/files/cpm4nano_tape_ckt.png)  

![](https://acdc.foxylab.com/sites/default/files/cpm4nano_tape_brd.jpg)


Formats:  

![](https://acdc.foxylab.com/sites/default/files/cpm4nano_tape_fmt.png)  

Bits in Audacity:  

![](https://acdc.foxylab.com/sites/default/files/cpm4nano_tape_bits.png)


Functionality was checked for the linear input and output of a "large" computer with a speed deviation of up to 20%.


Example (WAV-file with three blocks):  

**CAUTION! HIGH VOLUME!**   

[tape.wav](https://acdc.foxylab.com/sites/default/files/tape.wav)  

 


P.P.S. I did a redesign of the project, now it looks like this:  

![](https://acdc.foxylab.com/sites/default/files/cpm4nano_MkVI_overview.jpg)


 


P.P.P.S. 8080/8085 CPU Exerciser from Ian Bartholomew & Frank Cringles successfully completed (screenshot from TV tuner):  

![](https://acdc.foxylab.com/sites/default/files/8080ex1_ok.png)


 


*To be continued!*

## Compilers

- Arduino IDE

## Tags

- Arduino IDE
- Arduino
- i8080
- CP/M
- emulator
