# Multi Purpose Automation System

Uploaded by marc*s on 2009-10-17 13:02:00 (rating 0 out of 5)

## Summary

Let me introduce my CombiConceptXM1.x - a multi purpose automation system with a lot of interfaces, like: digital I/O, analog inputs,PWM's,DCF77,TWI,RS485,wireless communication... It's a twin-ÂµC System - the Mega128 processes the critical real-time-jobs and the Mega644 performs the human interface-, and comunication-jobs. With it's grafical dialog surface, the rotary encoder and a IR-receiver it's very easy to find and execute the wanted function. Firmware updates can be done by the well known ISP-interface or wireless via the integrated bootloaders. Wireless programming requires the separately ,progIF' which is connected to a pc's serial interface. 


Functions:  

* Controlling blend function (different up/down times for each blend teachable - compensating mechanical losses)  

* Light control function (synchronized by ac-zero-detector)  

* Delayed switching function  

* Direct switching function  

* Free configurable power port pins  

* Combined function call by ,scenes'  

* Programmable power up/down delays (controls also a seperatly power rail pwm-output)  

* Timers (programmable for one or more days a week - executing a scene by event)  

* ,Compare & Execute' function (compares a setpoint with a value from a sensor link adress or a table based adress, considering programmable hysteresis and executes a system event)  

* ,Hot Shots' for sequential function execution on different network participants  

* ,Key sequences' allows the execution of more functions by one port pin (depending on the L/H-level and the stable state time)  

* Real time clock with calendar (synchronized by an optional DCF77 receiver)  

* GLCD Contrast and Brightness adjustable by pwm  

* IR-interface for RC5 remote (with learn function)  

* AD inputs (linkable to sensor categories)  

* Full remote access (every network participants with the necessary I/O-Equipment can act as a ,long arm' to any network participants)  

* Event-based data transfer (many system actions are producing an event)  

* Object transfer protocol (suitable for independent controller platforms)  

* Direct memory transfer protocol (copies data directly from/to physical adresses)  

* Mighty communication modul (Network scanning builds a table with all direct and ,meshed' reachable hosts)  

* Wireless Bootloaders on both Megas 


For more informations, especially the communication possibilities - have a look to the (german) documentation...

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
