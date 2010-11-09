# DIY ZigBee and AVR

Uploaded by doctek on 2010-11-09 16:04:00 (rating 5 out of 5)

## Summary

This project consists of the circuit board design, parts list, and all software so you can build and use your own Zigbee radios for remote data gathering. Information is provided to guide you in building the radios using surface mount technology. The necessary software, following the Atmel model, is provided for two specific microcontrollers used as the basis for the project.


Warning: This is not a project for Beginners! You don't have to be an Electrical Engineer, but you do need to have intermediate level hardware and software skills. Although I try to explain a lot about the design and what's going on, this is a project, not a tutorial.


This project is complete in the sense that you can follow it to build your own Zigbee Coordinator and Zigbee Device and communicate between them. It is not complete in the sense that there's a lot more that could be done with these radios and the software, including making much larger and more complex networks, as well as exploring very low power modes. So there's room for many exciting sequels and future projects as time permits. But by making this all available at this stage I hope to inspire collaborators to contribute their projects as well.


Motivation


I had two goals in doing this project:  

1.I'd like to be able to experiment with Zigbee radios for a lot less than about $25 each for commercial units. (Mine cost more like $8 each or less).  

2.I wanted to see if it was possible to build working Zigbee radios at all without an RF Engineering degree. (It is.) My hope was for decent range. (These achieve about 100 feet including going from inside to outside.)


What's it do?


This project implements a Zigbee Controller and Device. Data collected by the device are transmitted regularly to the Controller which prints via its USB port to a terminal emulator running on a PC of some type (pick your favorite). As implemented, only one channel of the A/D is read, but more channels could easily be added.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
