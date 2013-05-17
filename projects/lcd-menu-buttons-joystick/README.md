# Lcd Menu with buttons & Joystick

Uploaded by panoscure on 2013-05-17 07:38:00 (rating 5 out of 5)

## Summary

This project is just a simple menu with 6 lines and a sub menu with 4 lines for 16x2 LCD(you can add or remove lines and submenus), the keypad i use is a common 4x4 keypad. With button A and B you can go up or down at the menu. I use button # to select the option i want and button * to return. This program created in C using Avr gcc and the microcontroller is the atmega32.I have created 5 libraries to make the program more clear, one for the lcd one for the adc one for joystick, one for keypad and one for lcd\_menu. There is no schematic so the basic connections are: Joystick uses the analog portA(the first two pins 0,1) Keypad uses the port B and the LCD uses Port C.  

Important: If you use JTAG programing then change the LCD port and remove the two lines from main program with the following information  

MCUCSR = 0x80;  

MCUCSR = 0x80;  

I have also created another library that supports both 16x2 and 16x4 lcd, you can download it from my site  

[http://panoscure.no-ip.biz/avr/l...](http://panoscure.no-ip.biz/avr/lcdMenu.php)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
