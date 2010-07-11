# Python GUI Config for DS1631

Uploaded by jgenicio on 2010-07-11 08:53:00 (rating 0 out of 5)

## Summary

The project has two main parts:


1 .- The python program to act as a GUI  

2 .- The microcontroller firmware to talk to the DS1631 (initial address 0x90, A0-A1-A2 to ground)


The code modules for Python are pyusb, pyserial. You may need some more, we will know once you do:))


For communication with the microcontroller, a ATMEGA32, using the Arduino Nano, this is powered by the USB itself, or externally ( 5 Volts.). Plate used Rx-Tx pins and connect to ATMEGA32. In this way we communicate with the computer.


It does so because it is the only part that still holds:))


Python looks in the USB bus, the Arduino Nano. Which has the following credentials:


$lsusb -v


-> IdVendor 0x0403 Future Technology Devices International, Ltd  

-> ProductID 0x6001 FT232 USB-Serial (UART) IC


In hexadecimal, decimal past:


idVendor = 1027  

ProductID = 24577


If you use another method to go from USB to serial changes with appropriate data.


Running python Config\_DS1631.py, should be able to set the thermostat.


New:


Gauge example for DS1631.


With the equal hardware config, show the temperatura in gauge.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
