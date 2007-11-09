# DIY MP3 Player

Uploaded by bozai on 2007-11-09 04:40:00 (rating 0 out of 5)

## Summary

Pls see the latest changelog at the end.


This is one of my DIY works.  

It uses a mp3/wma/wav/mid decode chip VS1003, ATmega8L and SD card.  

It supports various bitrate music file. Supports both FAT16 and FAT32.  

For the reason of space, resource the screen was not added.  

But a couple of LED were used to indicate the state.  

The upload files contain the whole code which were written by my self and the schematic.  

And some picture of the player.


The platform is AVRStudio4.12 sp4 + WinAVR using -s optimization  

You need to create project or makefile youself.


Sorry for some comment are written in Chinese and for my English.


Hope it is useful to you!


Thank you!


The title was changed to "DIY MP3 Player"  

And ATmega8 use internal 8MHz RC oscillator.


I am very sorry that the schematic in the first file has a mistake.  

Please download the third file(New Schematic) along with first one.


2007-03-07:  

Since the new version AvrStudio (4.13 b528) and WinAVR (20070122) were published, some code should be changed.  

And a bug was fixed.(SD Card which has no bootsect might not work)  

Thank you!  

You should download the fourth file (code and schematic)


2007-05-09:  

Special thanks to Ronald Ausloos (Belgium)'s advice.  

This new version firmware has changed the behavior of random playing. It maks it a real shuffle. And in the shuffle mode, whe whole songs will be played only once in one cycle. In which order it will be played is randomly. Due to the limited RAM space of Mega8, the maximum amout of no repetition songs is 1024(I think that's enough). Songs are more than that will played randomly.  

A bug of FAT intialize is totaly fixed.  

Add support to VS1011 and VS1002 (a macro should be changed in VS1003b.h).


In order to help people find the trouble when making the player, the behavior the the LEDs has been changed and two test file are added.


SineTest.hex is the test hex file of VS10XX, It will generate a 5168HZ sinewave at the line out.  

Mp3StreamTest.hex is the test hex file to test wheter the chip can decode mp3 streams.


Thanks!


2007-05-21:  

Sorry, a serious bug of FAT function was found.  

Now, It was fixed.  

And the older files which are not used has been removed.


2007-06-16:  

Sorry, still the bug of FAT, I found it in doing other project.  

Well, soon i will be graduated, and go to work. Maybe this is the last update.  

Anyhow, just tell me if you find some problem or have good idea of it.  

Thanks to AVRFREAKS, thanks to all of you!


2007-11-09:  

Now, this player support play the music file in any directory. But, in sd card the maximum folder number should be less than 127, for it use mega8's internal EEPROM to store the folder infomation.


Because of this function, the search will take quite long time, so you may feel the switch from one songs to another may slower. And this will getting worse when there is more folder in the sd card.


If you do not want to this function, in FAT.h file there is a macro:  

#define FIX\_DRIECTORY 0  

just change it to 1 and rebuid, then it will use the default PATH "\\music"


Any bug, pls tell me. Thanks!

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
