# Attiny2313 Serial 16x2LCD driver

Uploaded by hzrnbgy on 2011-05-28 11:02:00 (rating 0 out of 5)

## Summary

This project began its life more of a debugging tool. Before I have something like this, I would use LEDs to show status of my variables at runtime. LEDs are okay but they don't give you the full picture so I decided to make an easy to use serial interface to the ever popular 16x2 HD44780 based text LCD using a cheapo Attiny2313 microcontroller. Granted that this is intended for debugging code at runtime, its functionality is fairly limited. As of now, it supports the most basic (and probably the only things you would ever need for debugging purposes) functions of the HD44780 controller.


I hope I made it easy enough to use that even a caveman can do it.


Here are the list of functions it supports plus their corresponding codes (these codes must be sent as integers not characters)


+clear display => 0 (zero)


+start writing at line 1 => 1 (one)


+start writing at line 2 => 2 (two)


+turn ON LCD => 3 (three)


+turn OFF LCD => 4 (four)


+turn ON cursor => 5 (five)


+turn OFF cursor => 6 (six)


+return home => 7 (seven)


example:


"SerialSend" is your routine for sending characters over serial


clear screen  

SerialSend(0); <--take note, thats the number zero, not the character zero


display "hello" on first line  

SerialSend(1);  

SerialSend('h');  

SerialSend('e');  

SerialSend('l');  

SerialSend('l');  

SerialSend('o');  

*do not send null terminator as that is equivalent to zero which will effectively clear the LCD screen


display "world" on second line  

SerialSend(2); <- thats the number 2, not character "two"  

SerialSend('w');  

SerialSend('o');  

SerialSend('r');  

SerialSend('l');  

SerialSend('d');


As you can see, it doesn't support writing to specific rows or columns on the LCD. That will be the subject of another version. But as of now, if all you need is to print out status codes during runtime, this should be more than enough.


Here's how to connect the attiny2313 to the LCD  

Attiny -- LCD Pin symbol -- LCD pin number  

PB0 - DB0 - pin 7  

PB1 - DB1 - pin 8  

PB2 - DB2 - pin 9  

PB3 - DB3 - pin 10  

PB4 - DB4 - pin 11  

PB5 - DB5 - pin 12  

PB6 - DB6 - pin 13  

PB7 - DB7 - pin 14


PD3 - RS - pin 4  

PD4 - RW - pin 5  

PD5 - E - pin 6


+plus you ground, Vcc and contrast lines


From your development board to the Attiny2313 controller


your board -- attiny2313  

GND - pin 10 (GND)  

TX\_PIN - pin 2 (RXD)  

*dont forget to provide power to your attiny2313


Since this is meant to be a cheap debugging tool, no external component is needed. Attiny2313 is running at default fuse and clock (1MHz), no external crystal, etc.


The setting for the serial port is  

4800bps, asynchronous, 8 data bits, no parity, 1 stop bit


Its in hex file so all you need is to burn the firmware to your extra tiny2313 lying around using your favorite programmer and your good to go. Remember, the attiny must be running on 1MHz or it will screw up the display (youll get garbage on screen)

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
