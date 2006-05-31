DS1307 Utility

Copyright (C) 2005 by Donald R. Blake
donblake at worldnet.att.net

This program is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.



This project demonstrates the use of the Dallas Semiconductor DS1307
64x8 Serial Real Time Clock.  Address and data information are transferred
to the DS1307 via a Two-wire Serial Interface (TWI).

This project provides the means for controlling and viewing the DS1307 settings
via an RS232 interface.



The data sheet for the DS1307 is available here:

     http://pdfserv.maxim-ic.com/en/ds/DS1307.pdf

I'm using a DS1307 module from Spark Fun Electronics.  Go to the Spark Fun
web page at:

     http://www.sparkfun.com

and search for Comp-RTC.



This project runs on an ATmega32 using a 7.3728 MHz crystal.  It has also been
tested on an ATmega8.  It will probably compile and work for any AVR that has
TWI and a hardware UART.  You may need to change MCU and F_CPU in the makefile.

Connect the AVR UART to a PC serial port through a device like the MAX232,
ST232, etc.  I run Tera Term (Pro) on the PC side.  You can obtain Tera Term
(Pro) from:

     http://hp.vector.co.jp/authors/VA002416/teraterm.html



The program responds to ASCII commands typed by the user.  Here is the list
of commands:

?

  Show time, date and day of week.

thh:mm:ss

  Set time to HH:MM:SS in 24-hour mode.

thh:mm:ssa

  Set time to HH:MM:SS AM in 12-hour mode.

thh:mm:ssp

  Set time to HH:MM:SS PM in 12-hour mode.

dyy-mm-dd

  Set date to YY-MM-DD.  YY is the year (0..99), MM is the month (1..12) and
  DD is the day (0..31, depending on the month).

yd

  Set day of week to D (1..7).

o0 or o1

  Set the SQW/OUT pin to a logic 0 or logic 1 level.

s0, s1, s2 or s3

  Set the SQW/OUT pin to a squarewave output.  The SQW/OUT frequency is:
  1 Hz for s0, 4.096kHz for s1, 8.192kHz for s2 and 32.768kHz for s3.

h

  Help - show list of commands.  Enter just the command character with
  no parameters for usage.
