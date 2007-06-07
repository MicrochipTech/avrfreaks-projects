/*********************************************************************************************************
 Title  :   C library for LM75 I2C bus temperature sensor 
 Author:    Chris efstathiou  
 E-mail:    hendrix@otenet.gr
 Homepage:  ........................
 Date:      2/Dec/2002
 Compiler:  AVR-GCC with AVR-AS
 MCU type:  any AVR MCU device
 Comments:  This software is FREE but without any warranty of any kind.
*********************************************************************************************************/

IMPORTANT:  Make sure that you edit the F_CPU field in the makefile to match
            the used MCU clock frequency.
            Also make sure that you edit the AVRGCC_VERSION field in the makefile
            to match the avrgcc version that you use to compile the code.
            330=winavr, 320=version 3.20, 302=version 3.02 etc.
            This is essential in order to include the correct files.
            From version 3.02 (302) and below you can use 302 as the version number.
 


I use it to drive the LM75 I2C bus temperature sensor.
Configuration is done in the lm75.h file

The temperature can be negative or positive and the accuracy is 1 degree Celsius
The LM75 is sending the temperature over the I2C bus in 0,5 degrees steps 
but there is no point to read the whole value from the temperature register
since the LM75 sensor accuracy is 2~3 degrees Celsius over the entire range.

Remember to give the LM75 an I2C bus address using the appropriate pins of the LM75 IC

I hope it can help someone.
Chris.
