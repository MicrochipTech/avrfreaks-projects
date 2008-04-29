# UAV parafoil

Uploaded by Laurenceb on 2008-04-29 13:08:00 (rating 0 out of 5)

## Summary

 This is a mega168 based parafoil controller. Uses an mlx90609 gyro, avaliable from [www.sparkfun.com](http://www.sparkfun.com) The code implements a two state kalman filter and PID controller, then hardware pwm to the steering servo. A NMEA gps is connected to RX and a radio modem to TX.  

 Full details (and revised code) on wiki page at


 [http://wiki.ukhas.org.uk/project...](http://wiki.ukhas.org.uk/projects:parafoil_avr)


 Currently not fully tested, as hardware is under construction. The Kalman filter code has been tested on PC using simulated sensor data generated with matlab. Most of the navigation code is based on previous work, but the NMEA parser has yet to be tested with a GPS module.


 The present version is designed for release and guided descent from meteorological balloons to a final waypoint, but it would be possible to use OC1B to drive a motor for propulsion and implement a GPS altitude based control loop. 


 Wind vectors are recorded during the ascent and saved to EEPROM, then used to convert from ENU frame to the air heading based frame where the control loop operates.


 You can build with or without ground.c, depending if you want the facility to take control from the ground with an RC system. The PWM monitoring technique has been used before with success.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
