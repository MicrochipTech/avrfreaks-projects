# Wheely balancing robot

Uploaded by calvin on 2006-03-17 13:22:00 (rating 0 out of 5)

## Summary

Wheeley is a two wheel balancing robot.  

A gyro and an accelerometer are used to determin the angle the robot makes with the floor.  

The combination of the different modules electronically is very straight forward. Both gyro and accelerometer have digital outputs with pulse width modulation. No electronic signal conditioning is necessary.  

The encoders give about 30.000 pulses per rotation. Speed measurement is therefore done separately by a Tiny26 to ensure stable timing on the Mega8. The control loop needs to be run at a fixed time interval. No interrupts can be accepted. The encoders have a A and B output, which make the speed and direction measurements simple. The data is transferred serial to the Mega8.  

The acceleration measurement is done with a ADXL202. The connection of the IC is very simple. The digital output of the IC is used.  

The measurement of the angle speed is done with a gyroscope. It is a standard gyroscope used in model planes and helicopters.  

The angle speed measurement and the acceleration measurement are combined to determine the angle the robot makes with the floor. Both measurements are needed for a few reasons:  

- The gyroscope suffers from drift. Even a small drift can cause high deviations when the signal is integrated for a longer time.  

- The acceleration measurement is influenced by horizontal acceleration of the robot. 


The robot is able to balance on a horizontal surface as well as a hill. 


Further details:  

Target Environment: Indoors  

Locomotion Method: 2 Wheels  

Sensors / Input Devices: Gyro, accelerometer, encoders  

Actuators / Output Devices: 2 DC motors with planetary transfer  

Control Method: IR Remote control/autonomous balancing  

CPU Type :Atmel Mega8, 2x Atmel Tiny26  

URL for more information:  

[www.dena.demon.nl](http://www.dena.demon.nl)  

[http://www.dena.demon.nl/balansb...](http://www.dena.demon.nl/balansbot.html#Balancing)

## Links

- [Wheeley balancing on a slope](http://www.dena.demon.nl/Balans5.mpg)
- [Wheeley balancing on a horizontal surface](http://www.dena.demon.nl/Balans4.mpg)

## Compilers

- BASCOM-AVR

## Tags

- Complete code
- BASCOM-AVR
