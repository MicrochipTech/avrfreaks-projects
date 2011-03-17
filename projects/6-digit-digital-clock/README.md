# 6 digit Digital clock

Uploaded by Mr.Holub on 2011-03-17 14:49:00 (rating 0 out of 5)

## Summary

6-digit digital clock


This is my first hobby project ever any constructive critique would be appreciated. I've started this about 4 years ago and finally got finished.


Features:


- clock  

- alarm clock mode using internal beeper  

- alarm clock mode activating relay to connect or disconnect wires of external circuit  

- countdown timer mode enabling internal beeper when reaching zero  

- countdown timer mode enabling internal relay to connect or disconnect wires of external circuit when reaching zero


- all values and modes can be set using menu on Led display controlled by four buttons


Note about relay and speaker:


Please read documentation of your relay regarding value of resistor R\_RELAY switching the coil  

I've used PC-speaker as internal buzzer but any piezo-speaker would work too.


Note about power supply:


you can use 5V power supply to power whole circuit. I've decided to use external 5V power supply because of overheating while using voltage regulator.


Usage:


button PinB0 enter setup/cancel without changes  

button PinB1 increases selected display/select  

button PinB2 select next display/move in menu  

button PinB4 set values to counter of seconds and exit menu  

internal alarm now uses speaker to generate sound


OFF function appears in menu only if countdown/alarm is set.  

When selected, alarm/coutdown is disabled immediately


Please send any questions, bugs or constructive critique to: [mrholub@hotmail.com](mailto:mrholub@hotmail.com) or to Mr.Holub at [www.avrfreaks.net](https://www.avrfreaks.net)

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
