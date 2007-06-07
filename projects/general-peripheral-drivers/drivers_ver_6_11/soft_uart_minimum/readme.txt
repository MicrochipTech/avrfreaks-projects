   
   SOFTWARE UART FUNCTIONS FOR GCC
   Written: by Chris Efstathiou
   Date:    03/Oct/2002 
   Info:    This code is FREE. There are no copyright issues with this code.
            Based on application note AVR 305 



IMPORTANT: You must use ports that have registers located below 0x3f
           when you compile for the MEGA128 otherwise the driver will not work!
           Thats because the assembly file uses the in/out/sbi etc. instructions.

            Make sure that you edit the F_CPU field in the makefile to match
            the used MCU clock frequency.


This code is a software uart for the AVR MPU
writen in assembly.
It does not use any interrupt at all.
This driver is the absolutely bare minimum code required 
to realise a software uart. 


Feel free to modify any part you wish but please
give me a note so i can get feedback.

TO CONFIGURE THE SOFTWARE UART EDIT THE
suart_min.cfg file ONLY!!!
YOU DONT NEED TO CHANGE OTHER FILES!!!

