   
   LCD DRIVER FOR GCC
   Written: by Chris Efstathiou
   Date:    20/Apr/2007 
   TARGET:  All AVR MPU's
   Info:    This code is FREE. There are no copyright issues with this code. 



IMPORTANT:  Make sure that you edit the F_CPU field in the makefile to match
            the used MCU clock frequency.



This code is a LCD, IO mode driver for the AVR MCU
and is written entirely in GCC.
Its main goal is to be small sized and easy to debug before
i use the full featured one. 
It does not use any interrupts or timers.
 
Feel free to modify any part you wish but please
give me a note so i can get feedback.

TO CONFIGURE THE LCD DRIVER 
edit lcd_io.h header file.
Instructions for setting up the driver are provided
within the lcd_io.h header file.


The supplied LCD driver is set for 6 pin operation @ 3686400 Hertz
so if you want t use another mode or cpu frequency remember to edit
the lcd_io.h header file.

NOTCE: For the 2 pin io mode you need to contruct the extra circuit
       shown in the 2wirelcd.jpg photo. The cirquit is quite simple
       and you shouldn't find any problems. I use it in all my projects
       since it is possible to use longer wiring.
       


