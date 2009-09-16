# Yet Another Programmer (YAP)

Uploaded by eskoilola on 2009-09-15 20:57:00 (rating 0 out of 5)

## Summary

This is a 3-5 volt compatible programmer dongle which attachs to the parallel port. Compatible with UISP.


Circuit description:


The four line used to upload the code, MOSI, MISO, SCK and /RESET have all diode protection and slw rate limiters. This all makes it possible to use a long cable between the project and the desktop.


The PC parallel port side has a simple level shifter which simply makes the signal to bang it's head to the limiting diodes as needed. There is a transistor to keep the VCC from climbing up with the input signal.


Lasttly there is a HC125 to buffer the four signals and to switch them off as needed.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
