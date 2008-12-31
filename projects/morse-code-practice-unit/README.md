# Morse Code Practice Unit

Uploaded by AndyPalm on 2008-12-30 18:44:00 (rating 0 out of 5)

## Summary

Send sequence of words selected randomly from a list  

of the most common 500 English words or send groups  

of five randomly selected characters for Morse code  

practice. Uses three pushbuttons and a 2x16 LCD screen,  

plus a speaker for the sidetone.


On startup, goes to speed selection screen. Speed is  

selected with UP and DOWN pushbuttons, between MAX\_WPM  

and MIN\_WPM. 


When MENU/SELECT button is pushed again the user may  

select to either receive words or random character  

groups using the UP and DOWN buttons. 


If words are selected, the next push of MENU/SELECT  

allows the user to choose the size of the most common  

word list used, 100, 200, 300, 400, or 500.


If random groups are selected, the user has the choice  

of either letters or numbers only, letters and numbers,  

or all characters.


When MENU/SELECT is pushed again, the program reseeds  

the random number generator with a running timer value  

and begins to send.


While sending is occuring, the DOWN and UP buttons  

act as PAUSE and CONTINUE buttons, respectively.  

PAUSE (DOWN) freezes the display so the user can check  

a word or group that has just been sent. It must be  

held down until the current word or group has been  

completely sent. The CONTINUE (UP) button unfreezes  

the display and sending continues.


The MENU/SELECT button may be pushed during sending to  

get the menu screens again. Note that this button  

also must be held down until the current word or  

group has been completely sent.


 Hardware: ATMega8 using internal RC oscillator at 1 MHz


 Pushbutton inputs:  

 PD5 to UP/CONTINUE  

 PD6 to DOWN/PAUSE  

 PD7 to MENU/SELECT


 PORTC connected to HD4478 LCD display using 4/6 wire  

 connections and simplified timing and pin assignments:  

 PC0 to LCD pin 11-D4  

 PC1 to LCD pin 12-D5  

 PC2 to LCD pin 13-D6  

 PC3 to LCD pin 14-D7  

 PC4 to LCD pin 6-E  

 PC5 to LCD pin 4-R/S


 Note: LCD pin 5-R/W is held at ground.


 PB1/OC1A is sidetone output, a square wave at 680 Hz.  

 PB0 is keyed line output for an external code practice oscillator.


 Notes: Uses TIMER1 to generate the 680 Hz sidetone and provide a  

 new random seed when the program is (re)started.


 This version was written to use only basic delay loops and  

 assumes a 1 MHz system clock. Libc basic delay loop  

 function \_delay\_loop\_2(unsigned int count) uses 4 system  

 clock cycles per count (plus some set up overhead), so here  

 is 4 us per count here. A delay of 1 ms gives a count of  

 250 (ignoring overhead).


 The Libc long integer random number function is used to  

 avoid floating point operations used in previous versions.


 Changing to basic delay loops and eliminating floating  

 point math significantly reduces code size.


 Andrew Palm  

 2008.12.26

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
