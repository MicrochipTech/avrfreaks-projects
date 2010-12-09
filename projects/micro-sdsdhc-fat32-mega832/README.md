# micro SD/SDHC  FAT32 & Mega8/32

Uploaded by DHARMANI on 2010-12-09 04:08:00 (rating 0 out of 5)

## Summary

Hi friends,


here is my project of SD (microSD) card interfacing, which I had started to learn the SD card functionality.  

Initially, a 1 GB microSD was used with ATmega8 and RS232 communication (currently, it is tested for 2GB, 4GB & 8GB cards as well). The Rs232 provides user interface to monitor the SD data. Using HyperTerminal, following different options are provided where one can test the raw data transfer by reading or writing to SD card:


0: Erase specified number of blocks  

1: Write to a single block  

2: Read a single block  

3: Write to multiple blocks  

4: Read multiple blocks


Once familiar with raw data transfer, one can move to FAT32 file system where following options are provided:


5: Get list of files/directories stored in the root directory of the card, with size in bytes  

6: Read a file with entered name  

7: Create file with specified name  

8: Delete an existing file with specified name  

9: Get total memory and free memory of the card


(Now options for date/time are also added)


Complete hardware schematic and a cheap microSD adapter design are available. The source code for ATmega8 & ATmega32 can also be downloaded.


Hope you enjoy it!!


**Update1 (08-Feb-2009):**  

--------------------  

Code in winAVR (AVR-GCC)added (Earlier code is in ICCAVR)


The GCC code is compressed with optimizer set to maximum level. 


**Update2 (03-Mar-2009): Source Code Version 2.0 released**  

------------------------------------------------------  

Following are the source code updates in Version 2.0:  

- Support added for SD cards having first sector as MBR rather than the boot sector  

- createFile and deleteFile functions added  

- A bug fixed in reading files stord at far locations in memory also correction made to accept 8+3 char file name (by mistake, it was taking 7+3 earlier)  

- FSinfo sector used for string total free cluster count & next free cluster number for faster file access  

- Instant freeMemory display (earlier it was taking more than 30secs) using FSinfo sector. FSinfo sector is updated now whenever a file is created or deleted  

- File memory size display in decimal, like windows (earlier it was in hex)  

- Raw SD functions multile block read and write, which are not required for FAT32, are disabled using FAT3\_TESTING\_ONLY macro for getting extra space required by createFile & deleteFile changes (you can activate it if you have more than 8k flash) Right now flash is 99.9% full  

- Clock speed raised from 1Mhz to 8 MHz, new SPI speed (after SD initialization): 4MHz instead of 500K & baudrate: 19200 (for HyperTerminal) instead of 4800


**Update3 (21-April-2009): Version 2.0 for Atmega32**  

----------------------------------------------------  

Many of the visitor of my blog have asked me to send them the AVRStudio project folder, to avoid copying individual files from my blog. Also, most people prefer mega32 for this application.  

So, I've now added the zipped folder for mega32 (running at 8MHz) on my webpage. Here all the 10 options mentioned above are enabled.  

- My blog also still contains the source code & schematics for mega8:  

[http://www.dharmanitech.com/2009...](http://www.dharmanitech.com/2009/01/sd-card-interfacing-with-atmega8-fat32.html)


I've measured the data transfer rate for reading from the microSD(by disabling the HyperTerminal transfers). The rates are for 1GB Transcend card.(with AVR running at 8 MHz and SPI clock at 4MHz)  

-for raw data transfer, speed is: 123.37 KBytes/sec  

-During FAT32 file transfer, speed varies from 78 KBytes/sec to 91 KBytes/sec (it depends on the location of the file as well as on whether the file is stored in continuous clusters or the other way)  

(I haven't measured the writing speed here as I need to enter data thru HyperTerminal for writing to SD, which adds delay)


**Update4 (21-june-2009): Version 2.1**   

--------------------------------------------  

This is an update specially for all those who are already using this library, small bugs which don't create problem in most applications, but better to fix them.  

- A bug removed from the createFile function which stopped creating new files after 32*8 files in the root directory  

- The root directory was unnecessarily getting expanded by one cluster whenever a file was created. Fixed in the new version  

- also, the fixed cluster size of 8 sectors is removed, this version will support other cluster sizes as well


**Update5 (19-July-2009): Version 2.2 for mega32**  

-----------------------------------------------------  

- **Append file feature added.**: Many of the users asked for the append feature, which was not there so far in the 'createFile' function. Now, the 'createFile' function is replaced with 'writeFile', which looks for the filename first, if the given file name doesn't exist then it creates new file and writes data, but if the file already exists, then it opens it and appends the entered data.  

- This update is only for the ATmega32 code, which is uploaded in my blog; the ATmega8 flash was insufficient for this. May be if some of the features are not required, the code can fit into mega8, too!!


**Update6 (13-Sep-2009): In Ver2.1 & Ver2.2**  

------------------------------------------------  

Fixed two bugs:  

- A bug which was giving error related to the use of 'LONG' in FAT32.c  

- There was a mistake in a function declaration in FAT32.c which was affecting the creation of file with size more than 32MB, corrected here.


**Update7 (09-May-2010): Code Ver2.3**  

------------------------------------------  

- Support for **SDHC** cards added (tested with SanDisk & Transcend microSD and microSDHC cards). The initialization sequence and command formats are modified.  

- Fixed a bug which was causing the program flow to go into infinite loop if the character number 512 in a sector was a CR (Carriage Return, '\r'), in the writeFile function  

- ATmega8 schematic is modified a bit. Also, shcematic for ATmega32 is uploaded, on request of some users.


**Update8 (17-May-2010): Code Ver2.4**  

------------------------------------------  

- Real Time Clock DS1307 circuit is added in the schematic and relevant libraries added in the code, for time & date entries in the files. Now the current date of file creation and file update will be entered in the FAT table (can be viewed by checking file 'properties' using a PC). This will also be useful for data-logging with time-stamp  

- Three more options added in the hyper terminal menu for displaying or updating the RTC date & time


**08-Dec-2010**  

-------------------  

- A new post on the blog is added which presents a software only approach for testing & learning about microSD/SDHC cards & FAT32 format. This code uses PC & USB card reader. For detailed procedure, you can visit the project page:


[microSD-FAT32 with Visual C++!](http://www.dharmanitech.com/2010/12/microsd-fat32-testing-using-visual-c.html)


09-Dec-2010: Added step-by-step procedure on how to test the code in VC++


Thanks to all readers for feedback which has kept the updates going!!

## Links

- [SD card interface](http://www.dharmanitech.com/2009/01/sd-card-interfacing-with-atmega8-fat32.html)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
