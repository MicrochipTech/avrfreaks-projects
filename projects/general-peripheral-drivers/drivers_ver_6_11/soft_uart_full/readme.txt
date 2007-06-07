   
   SOFTWARE UART FUNCTIONS FOR GCC
   Written: by Chris Efstathiou
   Date:    10/Nov/2002 
   Info:    This code is FREE. There are no copyright issues with this code.
            Was! Based on application note AVR 305 but now it is unrecognisable. 


IMPORTANT:  Make sure that you edit the F_CPU field in the makefile to match
            the used MCU clock frequency.


                          BASIC INFORMATION

This code is a Half duplex software uart for the AVR MPU
and is written in GCC mixed with assembly.

It can use an external interrupt for receiving if you configure it
that way so you dont have to lock the MPU when receiving.
You can choose not to have an external interrupt if you wish so.

It can use any 8 bit timer or no timer at all.
 

The timer mode is usefull for very low baudrates (tested down to 30 bps @3686400 Hz)
I think it may be more immune to timing interruptions.

Baudrates are up to 115200 bps @ 3686400 Hz.
You can choose the mode by editing the softuart.cfg file.
It is better to use lower baudrates though (say 19200) for increased reliability
and then step up as you develop your project.
I INTENSIONALY HAVE SET A HIGH BAUDRATE TO GIVE YOU ERRORS OR WARNINGS
IN ORDER TO UNDERSTAND BETTER THE CAPABILITIES OF THIS CODE. 
AT 3686400 IT WILL WORK RELIABLY UP TO 57600 BPS IN FULL CONFIGURATION
AND UP TO 115200 AT STRING ONLY MODE.

You can choose to have the baudrate and (or) the software uart port and pins
dynamically changed during run time or stable (less code space).
You cannot however have interrupt enabled receiving if you select to have
the soft uart port and rx,tx pins change dynamically during run time.
It is not possible yet...

You can in this version, invert the rx and tx logic thus making it possible to
connect the pins (through a resistor-zener network) directly to a RS232 port
without using a MAX232 as long as the RS232 port is able to accept TTL levels.
(Basically understand a 0 volt condition as logical 1).
RS232= 1 equals to -12v, 0 equals +12v 
I have tested this and it works fine in my computer's COM1.


                          GENERAL GUIDLINES

****************************************************************************
* ALL CONFIGURATION IS DONE IN THE softuart.cfg FILE!                      *
****************************************************************************

****************************************************************************
* THE RX PORT MUST BE ABLE TO FUNCTION AS INPUT AND THE TX PORT AS OUTPUT  *
****************************************************************************

****************************************************************************
* FOR RX AND TX USE PORTS WITH REGISTERS LOCATED BELOW I/O 0X3F            *
****************************************************************************

****************************************************************************
* IF YOU RECOMPILE FOR A DIFFERENT AVR MPU DELETE ALL FILES FIRST          *
****************************************************************************

****************************************************************************
* SWITCH OFF ALL UNWANTED OPTIONS IN suart.cfg TO SAVE CODE SPACE          *
****************************************************************************
 
Feel free to modify any part you wish but please
give me a note so i can get feedback.
hendrix@otenet.gr




