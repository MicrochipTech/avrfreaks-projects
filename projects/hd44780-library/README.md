# HD44780 Library

Uploaded by alank2 on 2011-08-19 11:54:00 (rating 5 out of 5)

## Summary

Title : HD44780 Library  

Author : SA Development  

Version: 1.11


Parts of this code have been created or modified by Peter Fleury, Martin Thomas, and Andreas Heinzen as well. I went through it line by line and modified or improved it as necessary. This library has been cut down to only what was necessary to communicate with the LCD and does not include scrolling or wrapping features. See the libraries for the mentioned authors to get those features if you need them.


FEATURES:  

---------


HD44780 LCD connection using as few as 6 pins.  

Each pin can be on any port/pin, they do not need to be next to each other as in 0-3 for example.  

R/W signal is optional.  

Two methods for LCD ready checking, check busy flag or delay.  

Multiple display support (up to 4).  

Optimized for minimum functions and code size.  

Interrupt safe, uses SBI/CLI instructions for setting pins.  

4 bit and 8 bit modes supported.


INSTALLATION:  

-------------


Three files are provided:


hd44780.c - Main code file, you must add this to your project under "Source Files".


hd44780.h - Main include file, you must include this in any files you wish to use the library.


hd44780\_settings\_example.h - This is an example of the hd44780\_settings.h file that the library requires (and will try to include). The settings that are intended to be customized for each project are located in this file.


The advantage to this is that the main C/H files are unmodified and can be updated to a new version without losing custom per project settings. Another advantage is that since they are unmodified, you can put them in a shared or library directory and use them in multiple separate projects. Then you only have one place to update them instead of multiple project directories.


Two ways you can implement this:


Non-shared method:


1. Copy these files into your project directory.  

2. Rename "hd44780\_settings\_example.h" to "hd44780\_settings.h".  

3. Set the values appropriate to your project in "hd44780\_settings.h".  

4. Add the hd44780.c to your project.  

5. Put "#include "hd44780.h" in any of your C files that need to use the functions.


Shared method:


1. Create a shared directory.  

2. Copy these files into this directory.


To use it with a project:


1. Copy "hd44780\_settings\_example.h" to your project directory as "hd44780\_settings.h". NOTE THE "\_example" was dropped from the filename.  

2. Set the values appropriate to your project in "hd44780\_settings.h".  

3. Add the hd44780.c to your project.  

4. Put "#include "..\shared\hd44780.h" in any of your C files that need to use the functions. You may have to modify this to point to your shared directory.  

5. Project -> Configuration Options -> Include Directories -> New -> Add your project directory. It should put a ".\" in the list. This step is necessary because when the library tries to include "hd44780\_settings.h", it will look in your project directory and grab the one customized for that particular project. This is why it is important NOT to have a hd44780\_settings.h in your shared directory and why I have this file named hd44780\_settings\_example.h instead. You can leave the example file in the shared directory as a file to copy and rename when starting a new project.


This library will work with my Advanced Delay Library as well by changing the USE\_ADELAY\_LIBRARY value from 0 to 1. By default it will use the \_\_builtin\_avr\_delay\_cycles function. My only gripe about this built in function is that if you are debugging at the assembly level it does not match C code lines to the assembly lines properly. Other than this it is exceptional. My Advanced Delay Library accomplishes the same thing while also adding additional delay functions that can expect a variable instead of a constant to be supplied and they don't suffer the C to assembly alignment bug that the built in ones do.


HOW TO USE:  

-----------


Supports LCD communications on as few as 6 pins or as many as 11 pins depending on configuration.


The first choice you must make is whether you want to use 4 bit or 8 bit mode. Honestly this isn't a hard choice as I've tested both on my scope to see how the performance differed and both were very close to the same under all clock speeds I tested (16khz to 16mhz). I don't see the point in wasting 4 uC pins for 8 bit mode as it seems to have no advantage. Use the LCD\_BITS parameter to set this:


LCD\_BITS=4 // 4 for 4 Bit I/O Mode  

LCD\_BITS=8 // 8 for 8 Bit I/O Mode


The next choice is whether to implement a RW signal or not. If you don't need to read anything back from the LCD, then you can skip implementing it and simply connect the RW signal to ground. This is nice because it doesn't take up a uC pin this way. If however, you need to read something back from the LCD, you will need to implement RW. Use the RW\_LINE\_IMPLEMENTED parameter to set this:


RW\_LINE\_IMPLEMENTED=0 //0 for no RW line (RW on LCD tied to ground)  

RW\_LINE\_IMPLEMENTED=1 //1 for RW line present


The last big decision is which WAIT\_MODE to use. You can select between Delay Mode or Check Busy Mode. Delay Mode will delay after each LCD command to make sure that there is time for the LCD to execute the command before the next one can be issued. Check Busy Mode will read the check busy flag from the LCD to see if the LCD is still busy or ready for the next command. Check Busy Mode requires the RW line to be implemented, however you can implement an RW line (RW\_LINE\_IMPLEMENTED=1) and use Delay Mode (WAIT\_MODE=0). You might think that the Check Busy Mode technique would be faster, but it is actually slower when running a clock below 10Mhz. This is because the extra code is takes to check it takes up more time that the Delay Mode would have. At 10Mhz or above, Check Busy Mode will be faster. At 16Mhz, it was 20% faster than Delay Mode, but at 8Mhz Delay Mode was 10% faster. Use the WAIT\_MODE parameter to set this:


WAIT\_MODE=0 // 0=Use Delay Method (Faster if running <10Mhz)  

WAIT\_MODE=1 // 1=Use Check Busy Flag (Faster if running >10Mhz) ***Requires RW Line***


This version implements multiple LCD display support for up to 4 devices. All devices will share their data/RS/RW(if implemented) pins. Each device will have its own E(enable) pin. You can use the command lcd\_use\_display(x) to choose which display commands will execute on. You will need to lcd\_init() each one individually. This not only allows you to run 4 independent LCD display, but some displays like the 40 character x 4 line display are actually implemented with 2 lcd controllers. They will have an E and E2 pin so you will need this multiple display functionallity to use a display like this.


To init the display, clear the screen, and output "Hello World...":  

 lcd\_init();  

 lcd\_clrscr();  

 lcd\_puts("Hello World...");


To put a character:  

 lcd\_putc('A');


To turn off the display:  

 lcd\_command(\_BV(LCD\_DISPLAYMODE));


To turn on the display:  

 lcd\_command(\_BV(LCD\_DISPLAYMODE) | \_BV(LCD\_DISPLAYMODE\_ON));


To turn on the display AND display an underline cursor:  

 lcd\_command(\_BV(LCD\_DISPLAYMODE) | \_BV(LCD\_DISPLAYMODE\_ON) | \_BV(LCD\_DISPLAYMODE\_CURSOR));


To turn on the display AND display a blinking cursor:  

 lcd\_command(\_BV(LCD\_DISPLAYMODE) | \_BV(LCD\_DISPLAYMODE\_ON) | \_BV(LCD\_DISPLAYMODE\_BLINK));


To move the cursor to the left:  

 lcd\_command(\_BV(LCD\_MOVE));


To move the cursor to the right:  

 lcd\_command(\_BV(LCD\_MOVE) | \_BV(LCD\_MOVE\_RIGHT));


To move the cursor to a specific location:  

 lcd\_goto(0x40); //0x40 is often the beginning of the second line  

 //each LCD display will have its memory mapped  

 //differently


To create a custom character:  

 lcd\_command(\_BV(LCD\_CGRAM)+0*8); //The 0 on this line may be 0-7  

 lcd\_putc(0b00000); //5x8 bitmap of character, in this example a backslash  

 lcd\_putc(0b10000);  

 lcd\_putc(0b01000);  

 lcd\_putc(0b00100);  

 lcd\_putc(0b00010);  

 lcd\_putc(0b00001);  

 lcd\_putc(0b00000);  

 lcd\_putc(0b00000);  

 lcd\_goto(0); //DO NOT FORGET to issue a GOTO command to go back to writing to the LCD  

 //ddram OR you will spend hours like me thinking the LCD is locked up  

 //when it working just fine and you are outputting to cgram instead of  

 //ddram!


To display this custom character:  

 lcd\_putc(0); //Displays custom character 0


To shift the display so that the characters on screen are pushed to the left:  

 lcd\_command(\_BV(LCD\_MOVE) | \_BV(LCD\_MOVE\_DISP));


To shift the display so that the characters on screen are pushed to the left:  

 lcd\_command(\_BV(LCD\_MOVE) | \_BV(LCD\_MOVE\_DISP) | \_BV(LCD\_MOVE\_RIGHT)); 


VERSION HISTORY:  

----------------


1.00 - Initial version.


1.02 - Delay\_ns, Delay\_us, and Delay\_ms added via a new included file "delay.h". All of these functions support values from 1-65535 so you can delay 65.535 seconds using Delay\_ms, or Delay\_ns(1) to delay 1ns. Realize that a delay of 1ns would only be possible if you were running at 1ghz, but asking for 1ns delay will get you a single clock delay. At 8mhz this is 125ns. The delays will get you "at least" what you ask for with as little more as possible. The reason the delay functions were added is because the LCD library I based this on "assumed" that 2 clocks were enough for a 500ns wait. This is TRUE if you are running at less than 2mhz, but not true if you are running faster. I modified these functions to use the new Delay\_ns function above so it will ALWAYS wait 500ns on the enable line now.


1.03 - No longer includes my delay functions, but instead uses the internal builtin\_avr\_delay\_cycles instead. You can still use it with my Advanced Delay Library, check the C file for info. This version also adds a clrscr in the init function. I was experiencing issues where a reset would corrupt part of the screen so this was necessary to make sure it starts clear.


1.05 - Reorganized all code to follow the standard C and H file techniques.


1.10 - Multiple LCD display support (Up to 4) added.


 Bugs in the read command and 8 bit modes fixed and tested.


 You are now able to put any pins on any pin and port. The data pins are no longer required to be on 0-3 or 0-7. This gives you full freedom to put these pins anywhere.


 All pin changes are now done through SBI CBI instructions meaning there will be zero problems with interrupts of other things occuring on pins of the same port as the LCD pins.


 Checkbusy used to end up in an infinite loop if the LCD didn't response with "not busy". I have put a 3ms maximum time on it (or 16 attempts minimum). Since all LCD commands should run with 1.64ms, this should be more than enough and will allow the processor to continue on instead of being permanently stuck. The delay however at 3ms everytime a call is made to the LCD will probably slow things down too much anyway, but I figured having this limit was better than nothing.


1.11 - A big issue in the LCD init code has been corrected which will now allow 4-bit mode to work properly below 2mhz. I've tested both 4-bit and 8-bit modes from 16khz to 16mhz with no issues.


 Many commands have been marked as static if you don't need to access them, the only change is that lcd\_read(x) is no longer available. You must use lcd\_getc() instead.


 RW\_LINE\_IMPLEMENTED has been added which allows you to indicate whether you are implementing the RW line or not. This used to be part of the WAIT\_MODE, but having this option now allows you to implement the RW line so you can read from the LCD, but still use WAIT\_MODE=0 for delays instead of using the check busy flag.


 Check Busy has had an additional 6us delay added to it when the previous command involved a read or write that changes the address pointer. This is due to the check busy flag going low before this pointer is updated and is to ensure the LCD is ready for another command.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
