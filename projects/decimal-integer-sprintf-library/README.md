# Decimal Integer Sprintf Library

Uploaded by alank2 on 2012-02-07 00:15:00 (rating 5 out of 5)

## Summary

Title : Decimal Integer Sprintf Library  

Author : SA Development  

Version: 1.10


INSTALLATION:  

-------------


Three files are provided:


dsprintf.c - Main code file, you must add this to your project under "Source Files".


dsprintf.h - Main include file, you must include this in any files you wish to use the library.


dsprintf\_settings\_example.h - This is an example of the dsprintf\_settings.h file that the library requires (and will try to include). The settings that are intended to be customized for each project are located in this file.


The advantage to this is that the main C/H files are unmodified and can be updated to a new version without losing custom per project settings. Another advantage is that since they are unmodified, you can put them in a shared or library directory and use them in multiple separate projects. Then you only have one place to update them instead of multiple project directories.


Two ways you can implement this:


Non-shared method:


1. Copy these files into your project directory.  

2. Rename "dsprintf\_settings\_example.h" to "dsprintf\_settings.h".  

3. Set the values appropriate to your project in "dsprintf\_settings.h".  

4. Add the dsprintf.c to your project.  

5. Put "#include "dsprintf.h" in any of your C files that need to use the functions.


Shared method:


1. Create a shared directory.  

2. Copy these files into this directory.


To use it with a project:


1. Copy "dsprintf\_settings\_example.h" to your project directory as "dsprintf\_settings.h". NOTE THE "\_example" was dropped from the filename.  

2. Set the values appropriate to your project in "dsprintf\_settings.h".  

3. Add the dsprintf.c to your project.  

4. Put "#include "..\shared\dsprintf.h" in any of your C files that need to use the functions. You may have to modify this to point to your shared directory.  

5. Project -> Configuration Options -> Include Directories -> New -> Add your project directory. It should put a ".\" in the list. This step is necessary because when the library tries to include "dsprintf\_settings.h", it will look in your project directory and grab the one customized for that particular project. This is why it is important NOT to have a dsprintf\_settings.h in your shared directory and why I have this file named dsprintf\_settings\_example.h instead. You can leave the example file in the shared directory as a file to copy and rename when starting a new project.


HOW TO USE:  

----------- 


I designed this library to avoid the memory consumption of linking in the floating point library and floating point enabled printf functions.


Starting with a fresh empty program that does nothing more that assign a floating point value and then uses sprintf to output it to a string, it increases code size to 3286 bytes on a 328P. This assumes you have also specified the linker options to enable the floating point sprintf which are a pain to find.


Instead of going that route and using a 16bit or 32bit integer with this dsprintf library, code size is only 1588 bytes, for a savings of 1698 bytes which can be huge if you need the program space or want to fit your program on a smaller chip.


How this works is that you use the integer as a normal integer except that you consider it to have a specific number of decimal places. An integer of 123 with two decimal places would be 1.23. All math will need to be performed on it normally without the use of decimal places.


An easy example is dollars and cents.


i1=123; //if we assume i1 has 2 decimal places, this would be 1.23  

dsprintf(string,"%.2ld",i1) //will output "1.23" to the string  

//note that we are using a .2 with a %ld just like we would use a .2 with a floating point. dsprintf knows how to render this properly.


One thing I really like about using integers is that they are always precise. You will never be a penny off or see something like 1.23111111111 like you may with a floating point that isn't decimal place limited and rounded.


You must keep in mind however that integers are limited in their maximum size and you need to stay within that size:


16 bit signed : -32767 to 32767  

16 bit unsigned : 0 to 65535  

32 bit signed : -2147483648 to 2147483647  

32 bit unsigned : 0 to 4294967295  

64 bit signed : -9223372036854775808 to 9223372036854775807  

64 bit unsigned : 0 to 18446744073709551615


A special note about 64 bit is that the 64 bit routines that are included in GCC by default are extremely wasteful with flash space. The 64 bit mode is disabled by default in dsprintf\_settings\_example.h for this reason. If you truly need 64 bit integers, see this post at avrfreaks for improved 64 bit support:


[https://www.avrfreaks.net/index.p...](https://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=113673)


With a floating point, you can multiply and divide without any concern to how big or how small the number is because floating point will keep track of the precision no matter how far you get from the above limits. Integers will not do this. You must make sure the order the calculations will be performed do not at any time overflow or reduce precision in your value.


Also, keep in mind that when multiplying or dividing values, you must account for the number of decimal places in each value. Here is an example:


i1 = 123 (2 decimal places, 1.23)  

i2 = 234 (2 decimal places, 2.34)


You can certainly multiply these together, but you will end up with


i1*i2 = 28782 (4 decimal places, 2.8782)


Note that the number of decimal places ADDED. If you want the final answer to only have 2 digits of precision you must do this:


i1*i2/100 = 287 (2 decimal places, 2.87)


If you decided you wanted less precision, only 1 decimal place:


i1*i2/100/10 = 28 (1 decimal place, 2.8)


Probably best to wrap these so you know for sure the order:


(i1*i2)/1000 = 28 (1 decimal place, 2.8)


You need to make sure that i1*i2 does not exceed their maximum integer size. You could cast them to a 32 bit if necessary to prevent overflowing.


This is more difficult than working with floating points because you must make sure the calculation fits in the integer and that it doesn't lose its precision, but the benefits are saving 1698 bytes and having precise integer values that support decimal points.


In a program that heavily relies on floating point values, it may be better to just use floating point, but if you only have a need for some decimal place output, this library may save you some bytes!


Dsprintf supports:


%% = a real percentage sign %


Flags:  

0 = zero fill flag  

- = left justify flag  

+ = show plus sign


Width and Precision:  

#.#


Size Modifier:  

l (32bit)  

ll (64bit)  

L (64bit)


Conversion Types:  

s (string)  

width, precision, and left justify supported


S (PGM string)  

same as string, but comes from PGM memory


c (char)  

%c expects a character in the form of an int  

dsprintf(s1,"%c",65) will output an A


q (repeat char)  

%q repeats a character based on the width  

dsprintf(s1,"%10q",65) will output AAAAAAAAAA  

You can also specify the character as the position  

dsprintf(s1,"%10.65q") will output AAAAAAAAAA


w (repeat until position char)  

%w works like %q except that a position is specified not the numbr of chars  

dsprintf(s1,"Hello%10w",65) will output "HelloAAAAA"  

Note that it filled until the 10th character with the letter A  

You can also specify the character as the position  

dsprintf(s1,"Hi%10.65q") will output HiAAAAAAAA  

This is obviously useful with spaces, and something you can't do with regular sprintf such as  

dsprintf(s1,"%d, %d, %d%20.32wDone",1,2,3) will output 1, 2, 3, Done  

Even if any of the values go to a double or triple digit, Done is still kept at the 20th position


d (signed integer)  

width, precision, left justify, zero fill, show plus sign, l/ll/L size modifier supported  

The precision specifies the number of decimal places the integer is assumed to have  

dsprintf("%.3d",-123) outputs -0.123


u (unsigned integer)  

same as d but unsigned


x (hexidecimal)  

width, left justify, zero fill, l/ll/L size modifier supported


X (uppercase hexidecimal)  

same as x but uppercase A-F


VERSION HISTORY:  

----------------


1.10 - Reduced flash size.  

 New cleaner state machine design.  

 Added hexidecimal output (%x, %X)  

 Added 64 bit (long long) support (%lld, %llu, %Ld, %Lu)  

 Added tunable settings (dsprintf\_settings.h)


1.01 - Integer conversion is now faster.


1.00 - Initial version.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
