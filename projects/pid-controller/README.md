# PID CONTROLLER

Uploaded by admin on 2002-06-13 16:04:00 (rating 0 out of 5)

## Summary

This is a project I did for an engineering course. It consists of a AT90S4433 which reads an analog input, has an enterable setpoint via up/down pushbuttons, and sends an output to a LTC1661 which is converted to 4-20 or 0-20ma via an LM358 op amp. The process value, setpoint, and %output is displayed on a 16x2 LCD, and the UART also sends LCD display information at 9600 baud to a terminal COMM port. I developed a 24bit floating point math (uses recipicles then multiply for division). There are BCD routines. There are 5 pushbutton switch inputs multiplexed with the LCD output. All this works fine. However, I never had enough time to finish the programming function so the parameters are stored in flash, not downloaded from EEPROM. The PID algorithm is not exactly PID, rather a modified algorithm. FP Math is performed like an HP RPN calculator, with a XYZT stack like arrangement. It was capable of execution rates of 10 passes through the code per second easily at 3.68MHz. These routines are modular and might have use on any similar application. I have Eagle schematics availabIe for it. I gladly accept constructive criticism on how to improve the code.

## Compilers

- AVR Studio 3

## Tags

- Complete code with HW
- AVR Studio 3
