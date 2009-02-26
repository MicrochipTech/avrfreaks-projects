# Ping))) Ultrasonic Distance Sens

Uploaded by Armor on 2009-02-25 22:40:00 (rating 0 out of 5)

## Summary

Parallax Ping))) Ultrasonic sensor is one of the most popular distance sensors, its communication protocol is very simple, and only one pin is required to interface with it


Atmega32 microcontroller (8MHz crystal) is used to get distance from Ping))) ultrasonic sensor at every period and sends each value to the computer using USART


Timer1 is used to calculate the time for the signal emitted from the sensor to hit the object and echo back again. Input capture ICP facility is used as an interrupt, so no other external interrupts are needed.Only Timer1 and ICP pin are needed.


Another version is available where any other timer can be used along with an external interrupt(e.g. INT0)


Project and simulation files are attached.  

For photos and detailed description about how to write the code from scratch, please visit Project's link bellow.

## Links

- [Parallax documentation](http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf)
- [Project's page](http://sites.google.com/site/osamaslab/Home/projets-list/ping-ultrasonic)

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code with HW
- CodeVisionAVR-Standard
