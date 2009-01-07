# NGW100 Led Blink C program AVR32

Uploaded by Dingo_aus on 2009-01-07 09:41:00 (rating 0 out of 5)

## Summary

This is a simple "Hello World" type application.


It is a heavily commented C program which creates a Linux executable which when run on the NGW100 board will blink the 'B' Led at 1Hz.


It is an example of using the sysfs interface to access hardware.


It should compile on a standard installation of AVR32Studio. Starting a new C, Linux executable project and cutting and pasting the contents of this file should then compile and generate the .elf file which can be transferred to your NGW100 and then executed.


EDIT: Addition on 7 January 2009 demonstrates how to write to the "export" file to create a gpioXX folder in sysfs

## Tags

- Complete code
