# Olympus Digital Camera Serial Control

Uploaded by Guttermouth on 2003-01-07 21:31:00 (rating 0 out of 5)

## Summary

Full BASIC code, schematic, serial protocol, and text write up are included in zip file.


I came up with this project to put my old 640x480 max resolution digital camera back to some good use by using an AVR (currently an AT90S2313) to control the camera thru its serial port. The AVR can command the camera to take photos as well as a few other basic tasks like set the flash mode. Once I got the program working to take a single picture, it was then very easy to set it up to take a picture every xx seconds or minutes. Or if you want the code can be modified to take a picture on command (say a trip wire or other sensor detecting a person or animal crossing the camera's path).


This is version 1.0 which simply sets the camera's flash mode to forced off and wakes up the camera and commands a photo every 30 seconds. Later I will add other options like jumper settings to pick different time intervals or possibly different flash modes.

## Compilers

- BASCOM-AVR

## Tags

- Complete code with HW
- BASCOM-AVR
