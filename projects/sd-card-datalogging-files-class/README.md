# SD card datalogging files class

Uploaded by carl47 on 2011-03-30 00:13:00 (rating 5 out of 5)

## Summary

This class was written to simplify datalogging using a standard sd card.


It creates text files and appends data to them.  

It's use is for mass storage on an atmel processor.  

This data is then loaded onto and used on a PC. 


It has only two functions:


SDCARDFILES.createfile(); //create a text file with 0 length  

SDCARDFILES.fileappend(); //append 512 bytes to the open file


The 512 bytes are written from a buffer which must be declared as global:


unsigned char buffer[512] ; //contains the 512 bytes to write to file


Operational notes:


I first wrote this class for an arduino platform. 


[http://arduino.cc/forum/index.ph...](http://arduino.cc/forum/index.php/topic,54014.0.html)


It works on standard SD cards from 256MB to 2GB made between 2007-2011.


Start with a newly formatted(FAT16) card.  

You must dedicate this card to your Atmel project.


The first call of createfile will create a text file in the first root directory location:


DATA0000.TXT it will have length = 0


Further calls will create sequential number files up to the maximum allowed in  

the root directory(usually 512):


DATA00511.TXT it will have length = 0 


Calls to fileappend will write the 512 byte buffer to the end of the currently  

opened file.  

Further calls will append until the data sectors are used up.


Both createfile and fileappend use the 512 byte buffer.  

It will be changed after the call.


You should use multiples of the cluster size to use the card efficiently.


Open the APS file and a example program will open with the SDCARDFILES class.


This example will write 512 files one sector long to a formatted(FAT 16) sd card

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
