# Task Scheduler Pattern for the ATtiny104

Uploaded by Vahled on 2016-10-08 18:00:00 (rating 5 out of 5)

## Summary

 


 


Using a standard architectural pattern for a very small  

micro-controller.  

When I am asked to create a software application for a small 8-bit microcontroller, I usually  

default to an architectural pattern that has served me well for the last 8 years. Typically, these  

smaller micro-controllers have 8K to 32K bytes of Flash memory and 1K to 2K bytes of RAM.  

Many 8-bit micro-controllers are too small to make use of the standard RTOS  

implementations, but large enough for this simple task scheduler.  

This spring, Atmel introduced the ATtiny104 microcontroller and I asked myself the Limbo  

question. How low can you go? Is it reasonable to use a task scheduler on a micro-controller  

with 1K bytes of Flash and 32 bytes of RAM?  

 


Application and Tools  

I apologize for leaving out the details of the hardware prototype design and the use  

of Atmel Studio 7.0 for software development. If the reader is interested, most of this  

information is available from the Atmel website.



**IDE and Compiler**​ - Atmel Studio 7.0 from Atmel.  

**Editor for Flash checksum insertion​** - Hex Workshop v6.7 from BreakPoint Software.  

**Embedded development platform​** - Atmel ATTiny104 XplainedNano (purchased from  

DigiKey).  

**Half sized prototype board**​ - Adafruit (very nice proto board for a reasonable price).  

**Application** ​- The application file “Prototype1.zip” can be unzipped into the directory:  

“C:\Projects\Atmel\ATtiny104\Proto1\Prototype1”. The Atmel Studio solution file is  

“Prototype1.atsln”.

## Compilers

- Atmel Studio 6

## Tags

- Complete code
- Atmel Studio 6
- ATTINY104
- ATTINY104-XNANO
