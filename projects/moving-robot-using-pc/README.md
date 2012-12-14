# Moving robot by using PC

Uploaded by edisonthk on 2012-12-14 02:26:00 (rating 0 out of 5)

## Summary

This is my first project and the robot move as what I expected. But it doesn't mean that there is no bug inside my project. Anyway, hope it help the others who are thinking about create a robot controlled by PC (Window 7 64-bit)


On PC side, serial port WINAPI is written .For personal reason, I am not using c# and serial port in the VB toolbox but c++ and debug it in the form of console. It's very difficult for beginner so I recommend using hyperterminal. It's free software and just download it and you can do serial communication with robot


On Robot side, 3 omni wheel is used and I need 6 PWM port (anticlock-wise and clockwise rotate) so two Atmega328p is used on this project. For those experienced might know that there is 3 timer and 6 PWM output port on Atmega328p but for learning SPI interface, I prefer to use 2 MCU. Program I used included Serial interface , pwm(Pulse Width Modulation) and SPI interface.

## Compilers

- AVR Studio 5

## Tags

- General Lib.functions
- AVR Studio 5
