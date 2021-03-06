# EEPROM Structure Library 1.03

Uploaded by alank2 on 2011-08-19 11:49:00 (rating 5 out of 5)

## Summary

Title : EEPROM Structure Library  

Author : SA Development  

Version: 1.03


INSTALLATION:  

-------------


Two files are provided:


eestruct.c - Main code file, you must add this to your project under "Source Files".


eestruct.h - Main include file, you must include this in any files you wish to use the library.


Two ways you can implement this:


Non-shared method:


1. Copy these files into your project directory.  

2. Add the eestruct.c to your project.  

3. Put "#include "eestruct.h" in any of your C files that need to use the functions.


Shared method:


1. Create a shared directory.  

2. Copy these files into this directory.


To use it with a project:


3. Add the eestruct.c to your project.  

4. Put "#include "..\shared\eestruct.h" in any of your C files that need to use the functions. You may have to modify this to point to your shared directory.


HOW TO USE:  

----------- 


I designed this library to address a couple of issues when using EEPROM for data storage:


Data corruption is one of this big issues with EEPROM. From what I've read, the most common reason is a power issue where power drops out and/or brown out detection is not properly configured. The big problem with data corruption is that when the data is read back from the EEPROM, it may be corrupt and the uC will use corrupt data with can lead to all sorts of problems.The second issue is that the EEPROM design limit of 100,000 write cycles. If you are trying to save values periodically, this will be a concern if you are writing the data to the same spot of EEPROM each time. The EEPROM Structure Library solves both of these problems by using these techniques:


The data you save is:


- Protected with a crc16 and will only loaded when the crc16 is valid.  

- Saved twice so that hopefully at least one copy will be able to be read back successfully.  

- Automatically stored using a round robin approach to the full area of the EEPROM so that it will wear more evenly.  

- Automatically recovered without any effort on your part.


It does all this with four functions:


unsigned char SetEEPROMStructureParameters(unsigned char ASegment, unsigned int AOffset, unsigned int ASize, unsigned AStructureSize);  

void ClearEEPROM(unsigned char ASegment);  

unsigned char LoadEEPROMStructure(unsigned char ASegment, void *AStructure);  

unsigned char SaveEEPROMStructure(unsigned char ASegment, void *AStructure);


The first IMPORTANT thing I want to mention is the ORDER these commands must be used.


You must call SetEEPROMStructureParameters() to set the parameters, then you must call LoadEEPROMStructure() or ClearEEPROM() . After that you can call SaveEEPROMStructure() as many times as you like. The reason for this order is that (1) The parameters must be set and (2) Save requires variables that Clear or Load set to work properly. This shouldn't be a problem because the general flow of usage for these commands should follow this order:


SetEEPROMStructureParameters(0,0,1024,8)


if (DetectMasterEEPROMClearCondition)  

 ClearEEPROM(0);


if (!LoadEEPROMStructure(0,s1))  

 {  

 //No structure available, set structure to defaults  

 }


AsNeeded  

 SaveEEPROMStructure(0,s1);


Parameter Explanation:


ASegment - This version adds support for more than one type of structure stored in EEPROM. By default the MAX\_EEPROM\_SEGMENTS define is set to 2 in the C file and this means you have two segments available, 0 or 1. This allows you to define an area of EEPROM for one structure and a secondary area of EEPROM for another. In my case, the reason the feature was added is because I have some statistics I need to constantly save and I wanted to separate them from settings that hardly ever change. This way I could devote more EEPROM space to the smaller statistics structure and it will have more slots and more EEPROM design life. You must call the SetEEPROMStructureParameters function for each segment you want to setup.


AOffset - This defines the beginning of the area available for a segment.  

ASize - This defines the size of the area available for a segment.


In my example of two segments, the first segment is called with a offset=0, size=160 and the second with a offset=160,size=864. This devotes 84% of my EEPROM to the second structure.


AStructureSize - IT IS CRITICAL THAT THIS IS 4 BYTES LARGER THAN WHAT YOU ACTUALLY NEED IN THE STRUCTURE. This is because the last 4 bytes are used for versioning and a crc16. In the above example of 8 bytes, the last 4 bytes are reseved, so you have 4 bytes to work with in your structure.


You could always use this method for your structure if you like:


struct  

 {  

 unsigned long MyLong;  

 int MyInt;  

 //etc  

 //ending with:  

 unsigned int ver,crc16;  

 } mystruct;


SetEEPROMStructureParameters(0,0,1024,sizeof(mystruct))


and then call:


LoadEEPROMStructure(&mystruct);


-or-


You can just use a static value and a string or memory block:


SetEEPROMStructureParameters(0,0,1024,8)


But realize that of these 8 bytes, you can only use the first 4. IT IS IMPORTANT HOWEVER THAT THE VARIABLE YOU USE TO HOLD THE STRUCTURE CAN HOLD ALL 8 HOWEVER:


unsigned long l1;  

unsigned char s1[8]; //MAKE SURE THIS IS ALLOCATED FOR ALL 8  

LoadEEPROMStructure(0,s1); //BECAUSE LOAD/SAVE WILL USE ALL 8  

memcpy(&l1,s1,4); //Put the first 4 bytes into a long integer


To find out how many slots will be used to store structures, use this formula:


((ASize/2)/AStructureSize)


The ASize is divided by 2 because structures are always written twice. In my example with a structure size of 8, ((1024/2)/8)=64 slots. The first time you save a structure, it goes in the first slot. The second time, the second slot. This will repeat until it runs out of slots and starts at the beginning again.


It also makes sense to align your structure size based on the EEPROM page size. In the 328P I have been using, page size is 4 bytes. If you change one byte, it must erase and write the 4 bytes in that entire page. So, you could use a structure size of 7 (3+4), but depending on how many pages it crosses, you are probably wearing out the EEPROM faster because it is not aligned. There is some confusion about this and documentation from Atmel isn't clear. I would play it safe and align. Also, the ClearEEPROM function will not work properly unless the AOffset and ASize are aligned by 4 bytes.


It is entirely possible to define a large enough structure size so that only 1 slot is available. For example, if I defined a structure size of 400 bytes, and have an eeprom size of 1024, ((1024/2)/400)=1 slot. In this case there will be no rotation, but you still have two copies written with a crc16 and if one is corrupt, the other will be used automatically. This still yields the benefit of crc16 protection and two copies to recover from.


Obviously a smaller, tightly packed structure is better because you can get more slots and the number of slots affects how many saves you have before hitting the 100,000 design limit. In my first example of 64 slots, now you have 64*100000 or 6.4 million saves before having to be concerned about the 100,000 cycle write limit. For this reason, I prefer not to use the struct method above because the variables I work with in C may not be packed as tightly as what I want in eeprom. In C I would probably use an unsigned char for a yes/no flag of 1 or 0. In EEPROM, I would go the extra effort to stuff that single bit in an unsigned char with other bits that might fit. You might think it uses a pointer of some sort to figure out which slot to use when loading values, but this would require storing the pointer in the same spot on EEPROM and that would be an issue with that is the 100,000 design limit again. It would make no sense to increase the design limit by rotating where you store the data only to have a single place where the pointer is stored running into that design limit much sooner.


Instead the library uses a version technique and each time a structure is saved, it gets a new version. The version is a 2 byte unsigned integer so it counts from 0-65535 and then repeats. The library handles the rollover just fine and realizes that 0 is a newer structure than 65535. It does this by assuming that 0-16383 is newer than 49152-65535. This would present an issue if you were able to store more than 49152 slots, but given EEPROM sizes that is quite unlikely.When reading values, it scans the entire EEPROM area looking for the newest version structure to load. Hopefully that will be one of the two copies of the latest structure written, but if both of those copies are corrupted, it will use the highest valid version it finds. In my example of 64 slots, that is 128 structures on the EEPROM that it could recover from if some were corrupt. The downside to this full eeprom scan is that it takes time. On my 328P running at 16.384mhz with 1024 bytes of eeprom and a 12 byte structure, it takes 4.4ms for the Load command. EEPROM isn't known for its speed anyway, so I suppose this really isn't too bad. It may be possible that you do not want an older structure's data if both copies of the latest one are not able to be recovered. In this case, you need to adjust the ASize or AStructureSize so you only have 1 slot. You will not get the round robin wear leveling, but this will force your structure to be saved twice in the same spot each time and it will load that exact structure or fail if both copies are bad. The LoadEEPROMStructure function will return 0 if it is unable to find or load a structure.


Make sure you don't just call save to save the same data as this is wasteful on EEPROM design life. Keep a flag that indicates when the data has changed and also a flag indicating how long it has been since that last save. Only save if the data has changed AND it has been a long enough period of time.


It is probably not a bad idea to give the device a master reset feature. In this case, watch a few inputs and when combined call the ClearEEPROM() function.


VERSION HISTORY:  

----------------


1.00 - Initial version.


1.01 - I added a void ClearEEPROM() function. It requires a 4 byte aligned EEPROM\_SIZE to work properly. It resets the EEPROM\_SIZE area in EEPROM back to 0xFF's.


1.02 - Reorganized all code to follow the standard C and H file techniques.


1.03 - Expanded to support multiple structures in one EEPROM area.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
