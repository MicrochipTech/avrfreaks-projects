# Control DDS-60 RF Synthesizer

Uploaded by AndyPalm on 2008-12-30 19:13:00 (rating 0 out of 5)

## Summary

 cDDSControl.c - Controls DDS-60 with frequency display on LCD and  

 input on keyboard.


 For ATMega8 at 4 MHz.


 The DDS-60 is a PCB kit available from the American QRP Club,  

 except for the Analog Devices AD9851 chip, which must be  

 obtained separately.


 The frequency in the AD9851 is controlled by sending a 40-bit  

 serial word. The connection requires 3 wires and ground.


 Keypad is 2x8, All Electronics CAT# KP-24. KP-23 could be  

 substituted (4x4 format). Pin 9 is not used.


 Keypad functions:


 ENTER  ENTER - Direct frequency entry in Hz.  

 Displays xx,xxx,xxx Hz.  

 CLEAR - Cancel function in progress  

 HELP n - Retrieve frequency in Memory n, n = 0 to 9  

 2ND HELP n - Store current frequency in Memory n  

 UP - Increment frequency digit at cursor position  

 DOWN - Decrement frequency digit at cursor position  

 2ND UP - Move cursor to left one digit  

 2ND DOWN - Move cursor to right one digit  

 2ND ENTER +xxxx - Enter freq calibration offset. Must be  

 exactly 5 characters long, first a + or -  

 and with leading zeros present if needed.


 The LCD is 2x16 Hitachi 44780 type in 4 bit data mode.


 Startup frequency is value stored in Memory 0.


 Mega8 hardware connections:


 PORTD connected to 4x4 keypad as follows:  

 PD0 to row 1, pin 1  

 PD1 to row 2, pin 2  

 PD2 to row 3, pin 3  

 PD3 to row 4, pin 4  

 PD4 to column 1, pin 5  

 PD5 to column 2, pin 6  

 PD6 to column 3, pin 7  

 PD7 to column 4, pin 8


 Notes: Internal pullups are used on row pins.  

 Pin numbers are for All Electronics CAT #KP-24,  

 pin 9 not used.


 PORTC connected to HD4478 LCD display using 4/6 wire  

 connections and simplified timing and pin assignments:  

 PC0 to LCD pin 11-D4  

 PC1 to LCD pin 12-D5  

 PC2 to LCD pin 13-D6  

 PC3 to LCD pin 14-D7  

 PC4 to LCD pin 6-E  

 PC5 to LCD pin 4-R/S


 Note: LCD pin 5-R/W is held at ground.


 PORTB connected to DDS-60 digital sythesizer:  

 PB0 to DDS-60 Load (Pin 1)  

 PB1 to DDS-60 Clock (Pin 2)  

 PB2 DDS-60 Serial Data (Pin 3)


 Andrew Palm  

 2008.11.30

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
