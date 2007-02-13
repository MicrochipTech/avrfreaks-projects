# ESC - Electronic Speed Controller for DC Motors (with brushes)

Uploaded by A-men on 2007-02-12 17:07:00 (rating 0 out of 5)

## Summary

This ESC is using 4 FET's (= Full Bridge, No relays).  

It has regenerative braking to reduce battery power consumption and FET heating.  

The input signal is a traditional RC Pulse (1 to 2 ms 5V pulse every 20 ms).  

It is designed around a single ATtiny 13 and a HIP 4081 FET bridge driver.


On May 1, 2006 I removed all outdated information and posted a new Zip-file containing all the up to date information.  

The main change in the software is an improved failsafe function.


Using this ESC :  

- I ended 2nd during the robot competition in Hattingen (Germany) on October 2 2005.  

- I ended 2nd during the UK Championship 2006 on August 27 2006.


The Zip file includes all the PCB board data, a part number list with all Digikey part numbers, the AVR studio 4 project with the assembler and extensive documentation about using PWM for DC motor control, a few pictures and a few movies.


I'm currently working on a version that will include current limiting.


16 October 2006 : Corrected some spelling errors. 


12 February 2007 : I've finished the hardware design of a version with current limiter. I will use an ATtiny26 in this version and I will use the PWM-signals that the ATtiny26 can generate. I still intend to write the code in c. BUT I have to admit it will take several months before that will be completed since my current priority is on the transmitter (project Pulse Place Modulation signal generator for RC transmitter).


A-man

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
