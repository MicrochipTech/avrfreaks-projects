# State Machine LED Blink Demo

Uploaded by AndyPalm on 2011-10-11 15:19:00 (rating 0 out of 5)

## Summary

Note: This is a revision dated 10/10/2011. I have added an alternate function on extended push feature to the pushbutton code. This required that pushbutton function activation occurs on button release for the normal and alternate functions. The acceleration feature works as before. I have also added a state to the pushbutton state machine and cleaned up the timer control parts.


The overall structure of this program follows the approach of Tom Baugh  

found in "MSP430 State Machine Programming."


This program was written to explore the use of non-hierarchical (flat) state machines as a way of organizing a program. The state machine structures and functions used here are based on the QEP FSM processor found in Chapter 3 of "Practical UML Statecharts in C/C++" by Miro Samek. 


The program also illustrates the use of the watchdog timer to generate a system tick every 16 ms, with the processor waking from from power-down mode on each tick.


The modules in this program:


 Main: General shell program for initialization and execution of the  

 system tick loop. Program sleeps in power down mode when not  

 executing a tick. This version uses the watchdog timer to  

 generate system ticks, leaving timers free for other modules.


 System: Does general low-level hardware initialization and contains  

 the WDT ISR.


 StateMachine: Contains base state machine and event structures and  

 performs base state machine functions. This code is  

 modified from code copyrighted by Miro Samek under the  

 GNU Public License Version 2. This code is free for any  

 use or distribution provided that the copywrite notice  

 at the beginning of the module files is retained.


 Blink: Sets up and runs state machines that independently blink  

 three LEDs.


 PushButton: Sets up and runs state machines that control pushbutton  

 functions, including debouncing and an optional  

 button acceleration feature. The functions actually  

 executed on a push are determined by "event sink  

 registration functions" called by the client, which  

 in this case is the Blink module.


See the individual module headers for further descriptions.


This version is ported to the ATmega168. Note that LED outputs are  

0 = on, 1 = off for the development board used. See System.c for the  

hardware connections.


Andrew Palm  

2011.10.09

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
