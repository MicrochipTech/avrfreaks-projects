   
   4X4 KEYBOARD DRIVER FOR GCC
   Written: by Chris Efstathiou
   Date:    16/2/2003 11:05 рм  
   TARGET:  All AVR MPU's 
   Info:    This code is FREE. There are no copyright issues with this code.



IMPORTANT:  Make sure that you edit the F_CPU field in the makefile to match
            the used MCU clock frequency.


This code is a hex (4x4) keybord driver for the AVR MPU
and is written entirely in GCC. 
It does not use any interrupts or timers.
It can however use an external interrupt for notifying when a key is pressed.
Also the driver can use any port-pin combination as well as the I2C bus.
When the I2C bus is selected you can use multiple keyboards with one external interrupt.
In any case see the test_hkb.c for usage information. 

Remember that if you use the I2C bus the following apply:
1) The port definitions are inactive.
2) The pins definitions refer to the PCF8574 port pins and not to the AVR pins.
3) The I2C address set the keyboard address.
4) The interrupt comes now from the PCF8574.
5) Do not define more than the keyboards that are actually present.

 
Feel free to modify any part you wish but please
give me a note so i can get feedback.

TO CONFIGURE THE KEYBOARD DRIVER 
edit hex_keyb.h header file.
Instructions for setting up the driver are provided
within the hex_keyb.h header file.
In case you want to use your lcd driver
you can define the lcd functions needed
by the driver in the hex_keyb.h header file
(They are already defined for my LCD driver).


The supplied LCD driver is set for 6 pin operation (No lcd reading).

NOTICE: 
When the test_hex_keyb test program asks you for a password
the password is hardcoded to be 1234


