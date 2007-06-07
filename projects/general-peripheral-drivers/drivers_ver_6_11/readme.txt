
/**********************************************************************************
 Title  :   DRIVER PACK VERSION 6.11
 Author:    Chris efstathiou hendrix@vivodinet.gr
 Date:      6/Jun/2007    
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  Read the readme.txt of each driver located in each drivers directory.

***********************************************************************************/

DRIVERS FOR: 1)  HD44780 LCD USING 6 OR 2 I/O LINES AND MULTIPLE LCD UNITS
             2)  UART, SINGLE OR DUAL
             3)  SOFTWARE UART (MIXED C AND ASSEMBLY) TTL & RS232 CAPABLE
             4)  ANALOG TO DIGITAL CONVERTER
             5)  HEXADECIMAL MATRIX KEYBORD WITH EDITOR
             6)  I2C MASTER
             7)  1 WIRE
             8)  DS18X20 1 wire temperature sensor driver
             9)  LM75 i2c temperature sensor driver
             10) DELAY FUNCTIONS IN ASM
             11) SRF08 ULTRASONIC RANGING MODULE
             12) SRF04 ULTRASONIC RANGING MODULE (in the older driver pack)
             
IMPORTANT!  Make sure that you edit the F_CPU field in the makefile to match
            the used MCU clock frequency.
            Finally i have to say that errors (bugs) might and probably do exist
            in the code files although i am doing the best i can.
            It is getting very hard for me to keep developing those drivers
            as the code involved is getting more and more.


VERSION HISTORY

IMPORTANT CHANGES of version 6.11
Just a little house keeping done on the files, small improvements and some typos corrected.
Verified with winavr 20070525 and of course with linux gcc 20070317, binutils 2.17 and libc 1.45
It seems to me that the newer versions of avr-gcc or winavr (prior to 20070525) have some bugs but the latest version looks good.
Note: When the eeprom is not used the compiler will issue a rather annoying error at the end but this does not affect anything. The hex file is created without a problem.


IMPORTANT CHANGES of version 6.10
All the code has been modified, tested and found to be working well with winavr 20070122 and the 
prerelease gcc 20070317 with binutils 2.17 and libc 1.45.
Improved are the one wire library, the lcd library and added a small lcd driver which consumes less space and it is easy to debug.


IMPORTANT CHANGES of version 6.00
All the code has been modified, tested and found to be working well with winavr 20060421. 
I also include the older version of the drivers which it was very well tested just in case something is wrong
and you need some reference to the older code.
I am having real trouble making all the code work with winavr 20070122 
but i am working on it. 
The one wire code is re written and it is rock stable now.


IMPORTANT CHANGES of version 5.01
Some minor mistakes were found and removed and i made sure that everything compiles ok with
WinAvr 20040720. After that some macro commands changed and some new macros are needed to
restore compatibility. There is a compatibility.h in the lcd_io driver that shows how to implement those macros.
I am working on it.


IMPORTANT CHANGES of version 5

Rc channel decoder has been added and an rc application for rc navigation lights is also there.
Some older code needs some Macro definitions of the now missing commands
like the inb or outb commands (ok macros realy) in order to work properly on the newer gcc 
versions.

IMPORTANT CHANGES of version 468

1)  The i2c driver now checks for an occupied bus and if it finds the bus occupied it
    waits for a stop condition in order to issue a start.
    Also some options are available in order to save some code space.


IMPORTANT CHANGES of version 467

1)  The i2c driver now has better error control. Due to different hardware vendor
    techniques used to signal a device busy situation, i made some alterations so
    the driver can cope with the varius techniques.
   


IMPORTANT CHANGES of version 466

1)  The hex keyboard driver now supports multiple hex keyboard units on the I2C bus.
   
2)  A bug that prevented the correct interrupt operation of the hex keyboard driver
    in I2C mode is fixed.
 


IMPORTANT CHANGES of version 465

1)  The hex keyboard driver now supports I2C data transfers by using a PCF8574
    
2)  The hex keyboard function names got slightly changed from hkeyb_*() to hkb_*().

3)  A bug that prevented the right chars to by inputed in hkb_gets() is fixed.
 


IMPORTANT CHANGES of version 464

1)  The lcd driver now supports I2C data transfers (read-write) by using a PCF8574
    with up to 8 lcd units of different character capacity.



IMPORTANT CHANGES of version 463

1)  The lcd driver now supports I2C data transfers (read-write) by using a PCF8574.

2)  An error in the i2c driver when no error checking is enabled is fixed.



IMPORTANT CHANGES of version 462

1)  The one wire driver got more robust and timing accurate.



IMPORTANT CHANGES of version 461

1)  The one wire driver got much smaller in size.
2)  The "ow_config_io()" function is renamed to "ow_configure_io()".



IMPORTANT CHANGES of version 460

1)  The one wire driver got CRC8 checking.
    Every rom code returned from the one wire driver is CRC8 verified.
    You can use the calculate_crc8() function to calculate the CRC8 of any
    data you use. 

2)  I removed some new found bugs and rewrote the "lcd_getxy()" function
    to return the actual lcd's address counter when lcd reading is available.
    The return value of the "lcd_getxy()" function is an integer,
    with the high byte containing the current line number (y) and the low byte 
    containing the char position in that line (x).
    If the lower byte has a value of 20 that means that you filled that line.
    This position result can only happen when no lcd reading is available.
    When lcd reading is available the maximum x == 19. 
    I have tested it extensively but you never know... 
    See the test file for usage.



IMPORTANT CHANGES of version 459

1)  The lcd driver is completely rewritten (it was about time...).
    The result is a more robust lcd driver that needs less code space.
    It has many new options for further reducing code size.
    I have tested it extensively but you never know... 
    See the test file for usage.



IMPORTANT CHANGES of version 458

1)  The lcd driver has now full read support plus the ability to backup & restore
    the entire lcd's DDRAM. 7 pins are needed for the lcd i/f if lcd reading is desired.
    The lcd reading is only possible in mode 6 (1 lcd) for now. I will implement it
    later on mode 7 as well (multiple lcd units).
    See the test file for usage.



IMPORTANT CHANGES of version 457

1)  In this release the hex keyboard driver has the ability to use an external
    interrupt that is activated when a key is pressed.
    Also the keyboard assigned pins can be on varius ports and the keyboard 
    function names are changed in order to be more C correct.
    See the test file for usage.

2)  The Software uart drivers and all drivers in general are more compatible
    with older versions of avrgcc after some feedback i got.

3)  The SRF04 driver now it works ok after a typing error i made, that excluded
    the timer ISR from compiling in some configuration schemes.  



IMPORTANT CHANGES of version 456

1)  In this release the lcd drivers lcd_put_i() function got better by 
    left alignment of the printed number and by allowing 0 to 8 lcd digits
    to be reserved for the number regardless of its length. Tested OK.
    Read the readme.txt file found inside the lcd_io directory.

2)  The one wire driver is working fine so i am done with it.
    The test application for the ds18x20 is showing how to use the driver.
    I use the same test application with 3 DS18B20 connected on the same 1 wire bus
    in my room for temperature measurements.

3)  The F_CPU (MCU clock frequency) and the AVRGCC version are defined
    in the makefile so you have to edit the F_CPU to match your MPU frequency.
    This way it is not needed to edit the F_CPU definition found
    in almost every file of this driver pack and possibly in your files also.

4) The makefiles supplied are working fine in all versions of AVRGCC 
   including 3.2 and 3.3 (Winavr)



IMPORTANT CHANGES of version 455

1) This release had some wrong files so it is scraped. The replacement was version 4.56.



IMPORTANT CHANGES of version 454

1)  A lot of things have changed particularly in some function names.
    I did that in order to be more "C" correct.
    You have to change the functions names accordingly in your programs.
    example: i2c_transmit() is now i2c_put_byte().





GENERAL INFORMATION

The drivers have been tested with the standard gcc version as well with the
modified one that compiles code for the MEGA128.
Also they have been tested with the use of the modified io headers that permit
assignments, avrgcc 3.20, winavr (avrgcc 3.30 experimental)
and winavr (avrgcc 3.30 RC).

The drivers have a readme.txt file inside each directory that you should read.
Driver information can be found in the above named file and inside the .h files.

To compile the code use the supplied makefile included in each drivers directory.
 
WARNING: You might experience errors with the elf to coff programs that are available
         for AVRGCC in general. There is nothing i can do about it.
         The code is compiled ok. Only the coff generated file is incorrect.

Credits to a fiend named Daniel for his help and tips on the I2C driver.
I dont write his full name or email in order not to make him a possible spam victim.
 
I release that code with no warranty and copyright issues.
I also grand you the right to change, alter, modify anything you want.
Finally i have to say that i seek no credit, so feel free to put your name in the files
if you modify them (to the better i hope!).


LAST WORDS

My work is in another field of Electronics (Analog Electronics)
so i write code just for the fun of it. I do enjoy making gadjets with AVR's.
I have to say that reading 8051 and PIC related books in the past 
used to give me the creeps.
Other people helped me in the beggining of my AVR learning curve
by posting free code so the only thing i want is to help newcomers
or experienced (i hope!) AVR users if possible.
This is the best i can do for now and believe me i have tried hard,
so please dont shoot at me if something is wrong.

Chris Efstathiou.



       


