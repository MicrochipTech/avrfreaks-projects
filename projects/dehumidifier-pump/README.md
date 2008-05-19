# Dehumidifier Pump

Uploaded by admin on 2008-05-18 19:36:00 (rating 0 out of 5)

## Summary

Automatic sump pump with pump run-dry prevention


Opening:  

This project is a ‘sloppy project’ in my book. It was something I threw together one rainy afternoon after humping a full dehumidifier collection tank up the stairs to be emptied. It has since been made a few times over because as dopey as it looks, it works and saves the grief of needing to monitor the dehumidifier.


Outline:  

The original concept was to keep this as inexpensive(cheap) as possible. It sort of happened but more on that later. 


The device needed to conform to the following rules:


1) Electronically be completely self supporting with no external power supplies like wall-warts  

2) Must be constructed with parts IN-HOUSE. This is a junk drawer only design(yeah right).  

3) Since 110vac is gong to be switched(I thought of the 12/24vdc bilge pumps but killed the idea. Read later why), the device needs to be electrically sound.  

4) Not take forever to build.


Design:


The circuit is based on an ATMEL Tiny13 and a generic LM358 dual opamp wired as a comparator. 


The LM358’s non inverting input is held high via a 1megohm resistor(see schematic) The same input is also connected to a 14gauge copper wire that is cut to length for the desired ‘HI’ and ‘LO’ limits. Again refer to the schematic for the wiring and the pictures also enclosed.


The LM358 triggers the TINY13’s two inputs. One, being the ‘hi’ limit turns on the pump to start draining the tank, and also illuminates a high intensity blue LED to indicate that the pump is on(again refer to photos.. While this is happening the TINY13 is constantly polling the other ’lo’ input to see when the tank is now empty. This is indicated by the low limit sensor/LED turning off. Once this happens, the pump is turned off and the TINY13 resets and waits again for the ‘hi’ limit to occur, repeating the process.


At the same time that the ‘hi’ limit is triggered , along with the pump being turned on, a timer routine is started that counts out 5 minutes. Should the primary loop not reset this timer, the timer shuts off the pump, light’s the ERROR LED(again see photos) , and then goes into an endless loop. The pump will no longer restart, and the dehumidifiers own tank full shut down system eventually kicks in..


Cost:  

The dehumidifier by itself cost $159.00


The components for the controller cost retail $35.00


The sealed, UL rated submersible pump was $75.00.


The ‘junk drawer’ became a tad expensive, but the hassle of remembering to empty the tank is no longer an issue.

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
