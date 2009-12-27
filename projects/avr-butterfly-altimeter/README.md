# AVR Butterfly altimeter

Uploaded by jremington on 2009-12-26 20:26:00 (rating 0 out of 5)

## Summary

I chose the AVR Butterfly to interface with the SCP1000 chip and am very pleased with the result. The 6-character display is adequate, and can be read from quite a distance. Display is in feet or meters (depends on compilation constant METERS\_TO\_FEET). 


The altimeter can easily detect a 1 foot change in height, with the noise level being around 1 Pa (~8 centimeters) with 30 fold sampling and averaging. Keep in mind, though, that long term variations in atmospheric pressure amount to hundreds of feet in apparent change of altitude!


The joystick allows intuitive input of current altitude for short term calibration. As a bonus, the built in RS232 interface allows tracking by another CPU. Or, one could use the Butterfly's built in 512Kb dataflash memory to log altitude and temperature for extended periods (not yet implemented).


For interfacing, I used the SCP1000 breakout board from Spark Fun, connected to one bit of the JTAG port (PF6) and the USI port (PE4,5,6), see schematic. The current draw of the entire system is about 1 mA, so the supplied CR2450 battery should last for 400-600 hours of continuous duty.


I did not implement a real time clock--if that is desired refer to my Butterfly RTC project. 


I looked into implementing local temperature corrections but it is not entirely clear how to do that. At the moment, I assume that the temperature at sea level is 288.15 K. In one 4-hour test the altitude reading did not show any obvious temperature dependence (when moving from inside the house to outside, and back -- see attached Alt\_t.GIF file for graph of temperature and pressure data). 


Please hack away on this project as there are lots of additions one could make. The code currently takes up less than 10 Kb, about 60% of flash memory.


Note: I haven't been able to upload the zipped code and schematics folder to the Freaks server, which fails with an obscure error message: "Failed to load module Freaks Files (at function: "saveFile")"


You can download the zipped folder from [http://www.uoxray.uoregon.edu/or...](http://www.uoxray.uoregon.edu/orangutan/altimeter.zip)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
