# MMA7260Q based Robot Control

Uploaded by sagunms on 2010-08-21 01:23:00 (rating 0 out of 5)

## Summary

I had previously done an accelerometer controlled robot and I wanted to share my project to the AVRfreaks community after it help me to start walking on AVR MCUs. This project was done in CodeVisionAVR however, it can easily be done in AVRGCC or other compilers. In this project, when the ATmega32 starts, it auto-calibrates, the accelerometer center position as the orientation you had at the beginning. On tiling the accelerometer board, you can move the robot front, back, left, right or stop. very simple! the outputs could be driven to both L293B h-bridge as well as 16Ã—2 LCD. Actually the total project iâ€™m doing is a far complex one which uses, 1 Xilinx Spartan-3E FPGA, 2 Mega32s, 1 ARM Cortex-M3, nRF24L01 2.4 GHz transceivers, Speech Recog DSP implementation, ENJ28J60 Ethernet, blah blah blahâ€¦ so this is just an accelerometer implementation in Mega32â€¦


MMA7260Q is a 3-axis accelerometer which is used in this project. With a tilt of the accelerometer break-out board, you could control the front, back, left, right and stop movements of the differential drive robot (which i built by combining to ultra-cheap Chinese toy tanks which individually could only move back and front directions). however, this simple project is just for displaying the result as output in a 16x2 LCD...

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code
- CodeVisionAVR-Standard
