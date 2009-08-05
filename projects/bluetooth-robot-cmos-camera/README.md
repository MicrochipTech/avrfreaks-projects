# Bluetooth Robot with CMOS Camera

Uploaded by himanshumangal88 on 2009-08-05 15:37:00 (rating 0 out of 5)

## Summary

 The aim of the project is to design and construction of Bluetooth robot with CMOS camera to capture the Image of desired place (with in the range of Bluetooth), using the AVR microcontroller.


The project was divided into three phases. The First phase is to demonstrate the application of Bluetooth communication for coordinated robotic search. The second phase of the project attempts to controlling the robotic movements and the third phase is capturing the pictures using CMOS camera placed on the robot which sends the information to master Bluetooth.


The information flows in two ways: on the one hand there are commands from the computer to the Bluetooth robot for controlling directions and also to camera to change different characteristics of it, on the other hand images from the camera should be sent to the computer. 


This project deals with one of the application of robotics. In this project one moving object is developed which is controlled by Bluetooth technology. Micro controller along with the driver circuit is provided to run the stepper motor. According to the signal from Bluetooth device (connected to a system) robot follows the path, which is driven by a stepper motor. The up/down and left/right arrow keys of PC keyboard control the robot movement. The communication between the camera and the microcontroller is: using the I2C protocol to access to the different registers of the camera and using an 8-bit port to read the images. This Image is sent to computer via Bluetooth. In addition the camera will be connected to a TV with its analog output for debugging purpose.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
