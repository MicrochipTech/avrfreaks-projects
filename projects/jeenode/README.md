# JeeNode

Uploaded by jcwippler on 2010-01-06 15:59:00 (rating 0 out of 5)

## Summary

The JeeNode is a low-cost combination of radio and microcontroller board.


Think of the JeeNode as an Arduino + Xbee shield + Xbee, but smaller and less feature-full than the XBee.


The JeeNode is based on an Atmel ATmega 328 - the same as an Arduino - and was inspired by Modern Device's Really Bare Bones Board, which is like a minimal implementation of an Arduino. The USB communication interface has been removed and any USB-to-TTLSerial based on the FTDI232R chip is guaranteed to work as connection. Being compatible with Arduino boards such as the Duemilanove means that you can use the IDE to upload a sketch using any PC running Windows, Mac OS X, or Linux.


The radio is a HopeRF wireless module, which is available for the 433, 868, or 915 MHz frequency bands.


There is an "RF12" driver for the wireless module, and a "Ports" library for connecting sensors, I2C devices, etc.


See <http://news.jeelabs.org/docs/>

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
