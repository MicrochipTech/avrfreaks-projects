# Jtag  twice (STK500 +Jtag Ice)

Uploaded by Ali_dehbidi on 2011-09-18 07:27:00 (rating 0 out of 5)

## Summary

hi every body  

it's a cool project that you can build yourself!  

if you google the jtag ice subject you will find a lot of web pages that build jtag ice clone but here you will see how to build jtag twice(stk500 + jtag ice).


1-first of all we will use the same schematic of jtag ice.


2-we will use jtag boot to upgrade our Atmega16.


3-program your Atmega16 with boot.hex then start avr studio and from tools menu choose avr prog then find the upgrade.ebn in jtag ice folder of your avr studio.  

for example C:\Program Files\Atmel\AVR Tools\JTAGICE. 


4-we will read back Atmega16 flash with another programmer and confirm the last code address in this example the address is 0x2d2d  

then open the hex file with notepad and delete the line that start with 


:103800000C942A1C0C94471C0C94471C0C94471CC9  

and delete following lines till :00000001FF do not delete :00000001FF line and save the file.


5-once again program your chip first erase it and program it with new file.


6-we will set the start address of our stk500 firmware that i was written once  

jtag last byte + some words  

change the following line in makefile


LDSECTION = --section-start=.text=0x2da0


7-then compile the stk500 code and program the chip without erasing it!


8-then we will use the following code in assembler(in asm folder) and set the fuse byte to SMALLBOOTSTART.  

in this code we will decide to choose between stk500 or jtag ice respect to pinc.6  

if pin==0 then start stk500  

else start jtag ice.


9-again we will program the chip without erasing it and using asm.hex.


10-in stk500 you can add a led to PORTB.2 for showing communications and stk500 pins is as follow  

sck sck pin of Atmega16  

miso miso pin of Atmega16  

mosi mosi pin of Atmega16  

reset portb.1 of Atmega16  

led portb.2 of Atmega16  

now you have a cool tool to use.


for those of you who live in Iran if you could not build the project contact me and i will sell you the complete project with PCB.


and please do not forget to rate project  

project files

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
