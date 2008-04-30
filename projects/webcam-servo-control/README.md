# WebCam servo control

Uploaded by C@PPER on 2008-04-30 00:27:00 (rating 0 out of 5)

## Summary

I needed a simple way to add pan and tilt capability to my LabCam from a remote location using a web page interface. Two RC servos and an ATmega16 was the solution. This project is fairly simple in nature, and was completed in less than a week.


The two servos are attached directly to the webcam to form a gimbal. See attached photo: webcamservo.gif


The controller consists of an ATmega16, a MAX232 IC, and a small 5V power supply. See attached photo: webcamctrl.gif


The LabCam was already running on a Mepis Linux server using Camstreams to take snapshots at defined intervals. PHP code outputs servo control commands from the Mepis PC serial port to the controller. The controller then adjusts the servos in response to commands received.


A custom web page on the Mepis server links both the snapshot photos and the servo control. See attached code. The HTML file uses the PHP serial class found here: [http://www.phpclasses.org/browse...](http://www.phpclasses.org/browse/package/3679.html)

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
