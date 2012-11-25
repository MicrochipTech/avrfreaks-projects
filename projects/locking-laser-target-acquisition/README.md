# Locking Laser Target Acquisition

Uploaded by philbowles2012 on 2012-11-25 01:28:00 (rating 0 out of 5)

## Summary

Three modules:  

a) Robot chassis:  

 All lego, controlled by NXT brick, using IR sensor to send commands to RCX brick for motion etc


b) laser target acquisition (LTA):  

Mounted on a 3DOF lego technic gimbal, a Laser Diode / tuned photodiode receptor sends a "LOCK" to gimbal when 3M relective tape on target causes active LOW


c) joining the two:  

6dof MPU6050 feeding a UC3 xplained. Once a lock is received by the LTA, the UC3 will send commands to the 3 axes of the gimbal to return to the lock position on any movement, i.e. remained locked on target.


After a bit of "showing off" i.e. wandering randomly while maintaining lock, either the NXT or the RCX or the UC3 (who knows?) will calcualte the trajectory / firing angle to the target and....fire!

## Compilers

- AVR Studio 5

## Tags

- Complete code with HW
- AVR Studio 5
