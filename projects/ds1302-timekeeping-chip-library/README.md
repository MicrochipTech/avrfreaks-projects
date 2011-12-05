# DS1302 Timekeeping Chip Library

Uploaded by alank2 on 2011-12-04 18:13:00 (rating 4 out of 5)

## Summary

Title : DS1302 Timekeeping Chip Library  

Author : SA Development  

Version: 1.00


FEATURES:  

---------


Will verify that a ds1302 is present.  

Supports and reports a clock is not set mode.  

Uses a signature to detect an uninitialized state.  

Automatically manages the write protect flag to keep the chip safe from unintentional changes.  

ds1302 connection uses 3 pins.  

Each pin can be on any port/pin, they do not need to be next to each other as in 0-2 for example.  

Optimized for minimum functions and code size.  

Interrupt safe, uses SBI/CLI instructions for setting pins.


INSTALLATION:  

-------------


Three files are provided:


ds1302.c - Main code file, you must add this to your project under "Source Files".


ds1302.h - Main include file, you must include this in any files you wish to use the library.


ds1302\_settings\_example.h - This is an example of the ds1302\_settings.h file that the library requires (and will try to include). The settings that are intended to be customized for each project are located in this file.


The advantage to this is that the main C/H files are unmodified and can be updated to a new version without losing custom per project settings. Another advantage is that since they are unmodified, you can put them in a shared or library directory and use them in multiple separate projects. Then you only have one place to update them instead of multiple project directories.


Two ways you can implement this:


Non-shared method:


1. Copy these files into your project directory.  

2. Rename "ds1302\_settings\_example.h" to "ds1302\_settings.h".  

3. Set the values appropriate to your project in "ds1302\_settings.h".  

4. Add the ds1302.c to your project.  

5. Put "#include "ds1302.h" in any of your C files that need to use the functions.


Shared method:


1. Create a shared directory.  

2. Copy these files into this directory.


To use it with a project:


1. Copy "ds1302\_settings\_example.h" to your project directory as "ds1302\_settings.h". NOTE THE "\_example" was dropped from the filename.  

2. Set the values appropriate to your project in "ds1302\_settings.h".  

3. Add the ds1302.c to your project.  

4. Put "#include "..\shared\ds1302.h" in any of your C files that need to use the functions. You may have to modify this to point to your shared directory.  

5. Project -> Configuration Options -> Include Directories -> New -> Add your project directory. It should put a ".\" in the list. This step is necessary because when the library tries to include "ds1302\_settings.h", it will look in your project directory and grab the one customized for that particular project. This is why it is important NOT to have a ds1302\_settings.h in your shared directory and why I have this file named ds1302\_settings\_example.h instead. You can leave the example file in the shared directory as a file to copy and rename when starting a new project.


This library will work with my Advanced Delay Library as well by changing the USE\_ADELAY\_LIBRARY value from 0 to 1. By default it will use the \_\_builtin\_avr\_delay\_cycles function. My only gripe about this built in function is that if you are debugging at the assembly level it does not match C code lines to the assembly lines properly. Other than this it is exceptional. My Advanced Delay Library accomplishes the same thing while also adding additional delay functions that can expect a variable instead of a constant to be supplied and they don't suffer the C to assembly alignment bug that the built in ones do.


ABOUT:  

------


I really like the ds1302 timekeeping chip because it uses so little power that you can run it even from a small lithium battery like a cr1225 and the battery will die of old age before power consumption can deplete it. All it requires is a 32.768 kHz crystal, backup battery (or supercap), and 3 uC pins and you've got a realtime clock along with 28 bytes of battery backed up ram.


HOW TO USE:  

-----------


Once you have the correct pins and ports selected in the ds1302\_settings.h file, you are ready to use some library functions.


if (ds1302\_init())  

 {  

 //successful init, ds1302 is present and can be communicated with  

 }  

else  

 {  

 //failed to init, unable to communicate with ds1302  

 }


This function *must be called* before any other functions because it carries out 3 important tasks:


1. It configures the AVR ports for proper communication to the ds1302.


2. The ds1302 datasheet indicates that nearly all of the registers are undefined at power up so a signature is used to ensure that the device has been put into a consistent state. If the signature is not found, the clock will be put into the clock is not set state, the ram will be cleared, and the signature will be written.


3. The function will indicate whether the ds1302 is present or not. The ds1302\_init() function will return 1 if it can successfully communicate with the ds1302, or return 0 if it is unable to.


The library supports two modes with the clock. It either has a valid time and is running, or it is in the clock is not set state. At anytime the user can stop the clock and put it back in the clock is not set state by calling this function:


ds1302\_reset\_time();


If the user wishes to go a little further and clear all of the ds1302 registers, they can call this function which not only puts the clock in the clock is not set state, but also clears the user ram.


ds1302\_clear();


To get the time you will need to use the following:


struct ymdhms\_type mytime;  

if (ds1302\_get\_time(&mytime))  

 {  

 //time was present  

 }  

else  

 {  

 //ds1302\_get\_time returned a 0 indicating the clock is not set  

 }


You can always set the time using:


mytime.year=11; //2011  

mytime.month=12; //december  

mytime.day=3; //3rd  

mytime.hour=20; //8pm (24 hour time)  

mytime.minute=36; //36 minutes  

mytime.second=28; //28 seconds  

ds1302\_set\_time(&mytime);


Both of these functions use this structure which allows decimal access (NOT BCD) to the following variables. Year will be in the range of 0-99 (representing 2000-2099).


struct ymdhms\_type  

 {  

 unsigned char year,month,day,hour,minute,second;  

 };


The user can also use the ds1302 to store and read back 28 bytes of ram using the following functions;


unsigned char ds1302\_get\_ram(unsigned char AAddress); //AAddress should be in the range of 0-27


void ds1302\_set\_ram(unsigned char AAddress, unsigned char AValue); //AAddress should be in the range of 0-27


The ds1302 has a write protect flag and the library keeps this flag enabled to prevent any sort of unintentional change to the ds1302 registers. When the library needs to write to the ds1302, it will disable the WP flag, perform the change, and then re-enable the WP flag. If you need quicker access to the ds1302, you could remove this feature from the library as it does often turn write commands from a single command into three commands.


This library does not support day of the week or 12 hour mode time because both can be derived from the existing date/time values. If you need this support, you can easily modify the library to use it.


VERSION HISTORY:  

----------------


1.00 - Initial version.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
