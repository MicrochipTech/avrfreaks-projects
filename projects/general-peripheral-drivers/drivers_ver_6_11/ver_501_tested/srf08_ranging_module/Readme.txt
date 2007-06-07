/****************************************************************************
 Title  :   SRF08 RANGING MODULE FUNCTIONS library 
 Author:    Chris Efstathiou hendrix@otenet.gr
 Date:      13/Sep/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Info:      This code is FREE. There are no copyright issues with this code.

*****************************************************************************/

IMPORTANT!  Make sure that you edit the F_CPU field in the makefile to match
            the used MCU clock frequency.
            Also make sure that you edit the AVRGCC_VERSION field in the makefile
            to match the avrgcc version that you use to compile the code.
            330=winavr, 320=version 3.20, 302=version 3.02 etc.
            This is essential in order to include the correct files.
            From version 3.02 (302) and below you can use 302 as the version number.


I use it to drive the SRF08 ultrasonic ranging module
for use with Free Fall night parachute landing 
where i MUST know the distance from ground in order to land safely.
Infrared goggles gives you an idea about the drop zone
but you cannot judje the precise distance from the ground due to
optics distortion.
For programming example see the SRF08 module code.

CAUTION: THIS IS A TEST PROGRAM. DO NOT USE IT FOR PARACHUTE LANDING!!!
         There are too many thing missing from the code like
         audio and visual alerts, height averaging, variable gain, 
         variable range settings and many other safety features
         needed to make it suitable for this task!
         I can experiment because it is my life! 
         Its only purpose is to give an idea of how to use the driver code.

I hope it can help someone.
Chris.
