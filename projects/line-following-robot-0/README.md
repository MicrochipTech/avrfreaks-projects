# Line Following Robot

Uploaded by allenrl on 2009-05-03 00:26:00 (rating 0 out of 5)

## Summary

Goal: Use input from 5 analog IR reflection sensors to make a robot follow a line.


Hardware specs


-Differential drive (2x Lego motors)  

-Analog IR sensor array (8x sensors, I only use 5)  

-Dual channel H Bridge motor controller  

-Lego platform  

-Circuit board created with DipTrace, cut with circuit router  

-ATMega168 at 8MHz


Micro controller  

-Analog to digital conversion, free running mode, 5 sensors 0 - 5 volts  

-2 channel Fast PWM motor control, 50 hz


Other Considerations  

-Sensors are 0 volts when they see white, 5 when they see black.  

-Fuzzy control, tunable  

-5v regulator allows for multiple power options


Get input, run input through fuzzy controller, take fuzzy output and send it to the motors. 


Fuzzy Controller  

-5 current sensor values (scaled to 8 bit)  

-5 previous sensor values  

-Look up table for left motor tuning  

-Look up table for right motor tuning


First normalize the sensor values, then compare them with the previous values. Multiply the minimum by the corresponding value in the look up table. Do this 25 times, once per compare of unique normalized sensor pair. The sum of these products is the output for the respective motor.


I can't get the project files to upload on this site properly, here's a link that contains the .zip file (84k) with code, allocation table, dip trace files, and a schematic in .pdf.


[http://allenrl.fileave.com/LineF...](http://allenrl.fileave.com/LineFollower.zip)


enjoy.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
