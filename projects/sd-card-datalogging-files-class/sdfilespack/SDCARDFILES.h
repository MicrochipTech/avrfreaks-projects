/*
This library is based on the SRAM library :

http://arduino.cc/forum/index.php/topic,52871.0.html

It works on standard SD cards from 256MB to 2GB made between 2007-2011.

Start with a newly formatted(FAT16) card.


The first call of createfile will create a text file in the first root directory location:

DATA0000.TXT         it will have length = 0

Further calls will create sequential number files up to the maximum allowed in
the root directory(usually 512):

DATA00511.TXT         it will have length = 0 

Calls to fileappend will write the 512 byte buffer to the end of the currently
opened file.
Further calls will append until the data sectors are used up.

Both createfile and fileappend use the 512 byte buffer.
It will be changed after the call.

You should use multiples of the cluster size to use the card efficiently.

The class uses 70 bytes of RAM, 2960 bytes of program FLASH.
Of course you must use 512 bytes of RAM for the buffer.

On average a file append takes 60 ms.
*/


#ifndef SDCARDFILES_h
#define SDCARDFILES_h

#include "SDCARD.h"

class SDCARDFILESclass
{
public:

SDCARDFILESclass(); //the constructor gets boot record values
char createfile(void); //create files in sequence
char fileappend(void); //append 512 bytes to open file


private:

long FAT1start ;
int sectorsperFAT ;
long rootdirectorystart ;
int rootdirnumber ;
long datastartsector ;
unsigned char sectorspercluster ;
long totalfilesectors ;

char filedirectory[32]; // contains directory info for root directory

bool formatted;  //to flag sdcard formatted
unsigned long startcluster; // points to cluster in use 
unsigned long filelength; //start with 0 length
unsigned int createnumber;  //points to file number in use
unsigned long currentsector; //start at 0. points to sector in which to store the 512 byte buffer

char updatefilelength(void); //update file length in root directory
char fat(bool  nextfilecluster);  //update the FAT

};//end of class SDCARDFILESclass

 extern SDCARDFILESclass SDCARDFILES;

#endif
