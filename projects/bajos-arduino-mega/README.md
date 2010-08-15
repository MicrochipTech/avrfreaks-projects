# BAJOS - for arduino Mega

Uploaded by bh2009 on 2010-08-15 09:43:00 (rating 0 out of 5)

## Summary

BAJOS version 0.6 is an open academic project at the HWR-Berlin, faculty of Company-Linked Programs, department for informatics.  

BAJOS is a java run-time environment for microcontroller. It consists of BAJVM, a Java Virtual Machine and of BAJOSBOOT, a collection of system classes. BAJOS can be adapted to Embedded Systems with/without operating system.  

The system has been tested for 8 Bit and 32 Bit AVR controllers on different development boards and works under Linux on PC or Linux for Embedded Systems also.  

Installation of Bajos requires:  

- C-compiler (gcc, avr-gcc, avr32-gcc) on a cross development PC  

- (serial) interface between PC and development board for programming and  

interaction  

- at least 80 kbyte program memory on the controller board  

- at least 4 kbyte data memory on the controller board  

Boot classes in BAJOSBOOT include necessary classes of the JVM-specification, other useful system classes as well as the class PlatForm.java for binding the run time environment to the hardware configuration.  

Currently BAJOS is based or better "inspired" on the JVM-specification (nowtoday only < version 1.6) with exceptions and limits, described in the Readme-file in the downloadable packet.  

Version 0.6 works with a priority based scheduler and contains sychronization mechanism for fast interrupt response. Interrupts mean real hardware interrupts from the specific platform. This feature is only fully integrated for Arduino Mega, but could be used on all platforms with little effort.  

The data memory management is improved for increasing the java heap and stack space at Arduino Mega.  

A new build option TINYBAJOS is integrated, but it is an experimental state. TINYBAJOS has the goal to shrink BAJOS to the lowest possible size. This comes along with a reduction of features. On Arduino Mega the JVM needs 32,7KB Flash and 1239Byte Ram with TINYBAJOS option and 50KB Flash and 1749Byte without (don`t forget the bootclasses needs additional space). In this configuration you lose multithreading, interrupts and exceptions and some error outputs. Which features you want to cut off, cut be configured in the makefile.  

You find the working version of bajos at : <http://code.google.com/p/bajos>  

Stable version you get from <http://cs.ba-berlin.de> also

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
