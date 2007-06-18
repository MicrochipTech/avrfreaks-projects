# Orangutan-lib 0.3

Uploaded by benedict on 2007-06-17 22:58:00 (rating 0 out of 5)

## Summary

Orangutan-lib is a small library of routines for use with a Pololu Orangutan, Baby-Orangutan, or Orangutan-X2 controller. It's a complete project in that the 0.3 version has been released, but it's also a work in progress in that more functionality is being added. Functionality includes:


8/10-bit ADC  

Digital I/O  

Buzzer  

Up/Down Counters  

I2C Master  

LCD  

Pin-change interrupts  

PWM motor control  

Relay-style motor control  

Servo control (up to 8)  

SPI Master  

UART-SPI Master (Baby-O only)  

UART


The code is tailored specifically for the Orangutan family of processors (e.g. the LCD on the Orangutan, which is 4-bit, does not have all four I/O bits on the same port), but much of it is useful on other Atmel platforms as well.


The library is written in C and is set up to be used with AVR Studio 4 using WinAVR. It also includes a shell script to tweak the Makefiles so it will also work in a UNIX environment running avr-gcc as a cross-compiler. 


The website for the project is [http://orangutan-lib.sourceforge...](http://orangutan-lib.sourceforge.net)

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
