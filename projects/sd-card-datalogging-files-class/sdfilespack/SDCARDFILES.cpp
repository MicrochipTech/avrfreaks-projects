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

#include "SDCARDFILES.h"

extern unsigned char buffer[512]; //caller defines buffer

SDCARDFILESclass::SDCARDFILESclass()
{
        struct partition
        {
        long bootsector;  //is at offset 454
        long partitionsectors;  //is at offset 0x457
        }; //partition sector information
        partition* pp = (partition*) & buffer[454]; //pointer to structure & with buffer[]

	   int error = SDCARD.readblock(0);  //look at sector 0 it may be the boot sector
         if (error !=0)
        {
        formatted = false; //we definately have no format
        }//end of if we have an error
        else
        {
          if((buffer[0] == 235) && (buffer[11] == 0) && (buffer[12] == 2) && (buffer[16] == 2))
        {  //in a formated card must have first byte = 235, 512 bytes per sector and 2 FAT's     
         pp -> bootsector = 0;
         pp -> partitionsectors = 0;
        }//sector 0 is the boot sector 
        }//end of read without error       	 
          
       long bootsector = pp -> bootsector;
       
       struct bootsectordata
        {
         int bytespersector; //is at offset 11
         char sectorspercluster;  //is at offset 13
         int reservedsectors;  //is at offset 14
         char fatcopies;  //is at offset 16
         int rootdirnumber;  //is at offset 17
         char notneeded1[3];  //we done need these 3
         int sectorsperFAT;  //is at offset 22
         char notneeded2[8];  //we done need these 8
         long totalfilesectors;  //is at offset 32
        };  //boot sector information
       bootsectordata* pb = (bootsectordata*) & buffer[11]; //pointer to structure & with buffer[]
            
         error = SDCARD.readblock(bootsector);  //look at sector 0 it may be the boot sector
         if (error !=0)
        {
         formatted = false; //we definately have no format
        }//end of if we have an error
        else
        {       
        if((buffer[0] == 235) && ((pb -> bytespersector) == 512) && ((pb -> fatcopies) == 2))
       { //in a formated card must have first byte = 235, 512 bytes per sector and 2 FAT's     
       
        sectorspercluster = pb -> sectorspercluster; //find sectors per cluster              
        rootdirnumber = pb -> rootdirnumber;  //find root dir number              
        sectorsperFAT = pb -> sectorsperFAT;   //find sectors per FAT      
        totalfilesectors = pb -> totalfilesectors;  //find total file sectors
        FAT1start = bootsector + (pb -> reservedsectors);  //find the FAT1 start
        rootdirectorystart = bootsector + (pb -> reservedsectors) + (pb -> fatcopies)*(pb -> sectorsperFAT); //find root dir start       
        datastartsector = rootdirectorystart + ((pb -> rootdirnumber)*32 + (pb -> bytespersector) - 1)/(pb -> bytespersector); //find
        
        formatted = true; //we have a formatted card
        startcluster = 1;  //always start at 2 cluster(+1 in create).
        createnumber = -1;//start file 0(+1 in create).
        const char startdirectory[32] = {68,65,84,65,48,32,32,32,84,88,84,32,16,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0};
        //filedirectory is DATA0, number ,no other info, start cluster number, file length in bytes start = 0
        for(char i = 0; i < 32; i++)
        filedirectory[i] = startdirectory[i]; //setup the file directory information
        
       }//end of this card is formatted
      else
      {
        formatted = false; //we definately have no format
      }//end of card is not formatted
     }//end of read without error  
}//end of constructor

char SDCARDFILESclass::createfile()
{ 
     if (!formatted)
     return 10;  //card has no format
     createnumber +=1;  //move on for next create number
     if((createnumber >= rootdirnumber) || (rootdirnumber == 0)) 
     return 11;  //cannot add more than allowed in the root directory
     startcluster +=1;  //move on the next available cluster
     int error = SDCARD.readblock(rootdirectorystart + createnumber / 16);  //16 entries per sector
         if (error !=0)
     {
       return error;  //report error code  
     }//end of if we have an error

        filedirectory[7] = createnumber % 10 + 48;
        filedirectory[6] = (createnumber / 10) % 10 + 48;
        filedirectory[5] = createnumber / 100 + 48;  //add the create number
        
        filedirectory[26] = startcluster;
        filedirectory[27] = startcluster >> 8; //add start cluster number
        currentsector = (startcluster - 2) * sectorspercluster; //the data area (relative to datastartsector)
       
        filedirectory[28] = 0;
        filedirectory[29] = 0 ;
        filedirectory[30] = 0;
        filedirectory[31] = 0;  //add the file length = 0 to start
        filelength = 0;  //start a new file with 0 lenght
        
        for(int i = 0; i < 32; i++)
        buffer[i + (createnumber % 16) * 32] = filedirectory[i];  //add the new directory
       	
	 error = SDCARD.writeblock(rootdirectorystart + createnumber / 16);  //write the new directory sector back	
	 if (error !=0)
     {
      return error;  //report error code     
     }//end of if we have an error
   
         error = fat(false);  //put in end of file 0xffff at start cluster number
         if (error !=0)
     {
      return error;  //report error code     
     }//end of if we have an error
       return 0; //it all went well
}//end of create file

char SDCARDFILESclass::fileappend()
{ 
  if (!formatted)
  return 10;  //card has no format
  if((datastartsector + currentsector >= totalfilesectors) || (totalfilesectors == 0)) 
  return 12;  //we cannot add any more data -run out of card sectors
  int error = SDCARD.writeblock(datastartsector + currentsector);  //write data to current data sector	
	 if (error !=0)
     {
       return error;  //report error code  
     }//end of if we have an error
    
    if((currentsector % sectorspercluster == 0) && (filelength != 0))  //have we filled a cluster
   {
   startcluster +=1;  //move on to next cluster
   fat(true); //direct on to next cluster 
   }//we have filled one more cluster
   
   filelength += 512; //record the extra 512 bytes 
    error = updatefilelength(); //update the root direct file length
    if (error !=0)
     {
      return error;  //report error code   
     }//end of if we have an error
   currentsector += 1; //we have filled one sector
   //if(filelength % 16384 == 0)
   // Serial.println(filelength);
   return 0; //it all went well
}//end of append file

char SDCARDFILESclass::updatefilelength()
{ 
   int error = SDCARD.readblock(rootdirectorystart + createnumber / 16);  //16 entries per sector
         if (error !=0)
     {
      return error;  //report error code     
     }//end of if we have an error

        filedirectory[28] = filelength;
        filedirectory[29] = filelength >> 8 ;
        filedirectory[30] = filelength >> 16;
        filedirectory[31] = filelength >> 24;  //add the file length
        
        for(int i = 0; i < 32; i++)
        buffer[i + (createnumber % 16) * 32] = filedirectory[i];  //add the new directory
       
     	
	 error = SDCARD.writeblock(rootdirectorystart + createnumber / 16);  //replace the directory sector 	
	 if (error !=0)
     {
      return error;  //report error code   
     }//end of if we have an error
     return 0;  //all worked ok
}//end of updatefilelength

char SDCARDFILESclass::fat(bool  nextfilecluster)
{ 
  int FATsector = FAT1start + startcluster / 256;  //256 entries per sector
  int error = SDCARD.readblock(FATsector);  
         if (error !=0)
     {
      return error;  //report error code  
     }//end of if we have an error
     if(startcluster == 2)  //is this the very first entry
     {
     buffer[startcluster - 2] = 248; 
     buffer[startcluster - 1] = 255;  //the very first(0) cluster
     buffer[startcluster] = 255;  
     buffer[startcluster + 1] = 255; //cluster 1 not used
     buffer[startcluster + 2] = 255;
     buffer[startcluster + 3] = 255; //cluster 2 is the end cluster
     //FAT bytes = 248 255_255 255_255 255_ ...2nd integer entry is 0xfff(end cluster)   
     }//end of this is the first cluster
     else
     {
     buffer[(startcluster % 256) * 2] = 255;     //this is an end cluster.
     buffer[(startcluster % 256) * 2 + 1] = 255; // and the current cluster in use 
     if(nextfilecluster)  //have we filled a cluster in a file
     {
      if(startcluster % 256 == 0)  //are we on a page boundary
      {
        
        error = SDCARD.writeblock(FATsector);  //write the new directory sector back	
	 if (error !=0)
     {
      return error;  //report error code   
     }//end of if we have an error 
     
      error = SDCARD.writeblock(FATsector + sectorsperFAT);  //write the new directory sector back	
	 if (error !=0)
     {
      return error;  //report error code    
     }//end of if we have an error	 
      
       FATsector -=1; //need to look at the previous sector
       int error = SDCARD.readblock(FATsector);  //256 entries per sector
         if (error !=0)
       {
         return error;  //report error code     
       }//end of if we have an error
        buffer[510] = startcluster;
        buffer[511] = startcluster >> 8; //direct to the end cluster 
         //Serial.print("startcluster =  ");
         //Serial.println(startcluster);
      }//end of we are on a page boundary
      else  //direct to next cluster on this page
      {
        buffer[((startcluster % 256) - 1) * 2] = startcluster;
        buffer[((startcluster % 256) - 1) * 2 + 1] = startcluster >> 8;
      }//end of we are not on a page boundary 
     }//end of we have added a cluster to this file
     }//end of this is not start cluster 2
     
      error = SDCARD.writeblock(FATsector);  //write the new directory sector back	
	 if (error !=0)
     {
      return error;  //report error code    
     }//end of if we have an error
          
      error = SDCARD.writeblock(FATsector + sectorsperFAT);  //write the new directory sector back	
	 if (error !=0)
     {
       return error;  //report error code    
     }//end of if we have an error
   return 0;  //all went ok 
}//end of update FAT


SDCARDFILESclass SDCARDFILES;
