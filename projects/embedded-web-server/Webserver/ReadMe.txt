Webserver software

   Copyright: 
   This code is modified from code I found for an Arduino system by Guido Socher and the 
   Tux Graphics web server system. Big thanks to both. It is supposed to be open.
   
   SW tools:
   This system is designed to operate on an Atmel ATmega32 micro using AVR Studio version 4. 
   This version supports the Olimex ICE called AVR-JTAG-USB whereas the newer versions do not!

   Notes to hardware:
   The mega32 runs with a 16 MHz crystal.
   A 7-segment display is connected to PORTA. It will display the second counter from the 
   time task. The web page gives you the possibility to count up or down. Not required.
   The LAN interface is Olimex ENC28J60-H using Microchip IC with SPI interface. This 
   version of the code uses the interrupt from the LAN module.

   Notes to code:
   This version gives examples of HTML strings based in ROM aswell as in RAM. It uses 
   interrupt from the LAN module to ease the integration into an existing scheduler. It 
   enables you to decode incomming commands  - in this case only single character commands.
   S-optimization is used to keep the size of the code down - switch it off for debugging.
   Using sprinft like me will cost you 1.5 KB of code space.

   Contact: 
   Erik Rasmussen, Lecturer, ITET department, Copenhagen School of Design and Technology, 
   KEA, Copenhagen, Denmark

   mail at erikrasmussen.dk
