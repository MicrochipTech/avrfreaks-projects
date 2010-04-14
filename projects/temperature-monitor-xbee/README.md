# Temperature Monitor with Xbee

Uploaded by paxs1 on 2010-04-14 16:06:00 (rating 0 out of 5)

## Summary

This is a small project, Basically there are two microcontrollers "ATmega32" the main microcontroller "temp\_recptor.c" first task it's to take the temperature of the local sensor using twi protocol, second task it's to read the data from the xbee module using USART port, after this two tasks, the main microcontroller display it on lcd display.  

For the second microcontroller basically read the temperature from the it's local temperature sensor "DS1624" and the send it to the xbee module using the USART port every 1 second.


This is a project where you can take ideas, of how to use TWI port, USART port and LCD Initialization.


Don't hesitate to ask anything about this project or code.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
