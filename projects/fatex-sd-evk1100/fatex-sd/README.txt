FATEX-SD v0.42 4/26/2008 initial release
FATEX-SD v0.43 4/27/2008 no change to binary output, but corrected some source formatting in
                         fat_example.c, in other files- changed the "//!!!" to not change the
                         line count where possible (so line #s match Atmel's docs)

This was created with the EVK1100 (32UC3A0512-UES CPU), JTAGICE mkII, and AVR Studio 2.0.
Tested with a Lexar 1GB 60X SD card and a Sandisk 2GB microSD with adapter. Tested both with FAT16 and FAT32. I don't know if it can handle SDHC cards.

It is based off of Atmel's fat_example.c code written for the onboard dataflash located at:
..\1.2.1ES-AT32UC3A\SERVICES\FAT\FAT_EXAMPLE\fat_example.c
and parts of Atmel's sd_mmc_example.c (from the SD/MMC Card example in AVR32 Studio 2.0).

Framework url:
http://www.atmel.com/dyn/resources/prod_documents/AVR32-SoftwareFramework-1.2.1ES-AT32UC3A.zip

It uses all the files from the 1.2.1ES-AT32UC3A release. I did have to rename exception.S to exception.x because the compiler wouldn't find it. It was named exception.x in the 1.2.0ES-AT32UC3A release that is with AVR Studio 2.0 (how come the autoupdater doesn't update the framework? hmmm..).

In conf_access.h (lines 59 and 60), I disabled the onboard dataflash and enabled the SD/MMC card. I also enabled ACCESS_STREAM and ACCESS_MEM_TO_MEM (lines 154 and 156). I removed all the dataflash stuff from fat_example.c and copied various routines from sd_mmc_example.c to fat_example.c. I added a few needed includes also. In fsaccess.c I replaced the references for the dataflash to the SD/MMC (lines 121 and 406). The transplanted/changed code is surrounded by a set of "//!!!".

<soapbox>Atmel's includes system with multi-nested folders and many paths needing to be defined is just a mess... I couldn't get anything to compile until I did Windows Searches on the top level project folders for the .c and .h files, copied all of them to one new folder, created a new project, defined just one path, then renamed stuff one by one, then built, rinse-repeated, until I determined which ones weren't needed (and deleted those). Then I replaced each with the versions from the new framework. Whew!</soapbox>

Documentation is in the ..\1.2.1ES-AT32UC3A\SERVICES\FAT\FAT_EXAMPLE\readme.html file.

Commands (copied and pasted from the readme):
    * a:, b:... goto selected drive
    * mount drivename(a, b...): mount selected drive
    * cd dirname: change path to "dirname"
    * ls: display the content of the current directory
    * mkdir dirname: create "dirname"
    * cat filename: display "filename" content
    * touch filename: create "filename"
    * disk: get number of drives
    * append filename: append text to a file
    * mark: bookmark current directory
    * goto: goto bookmark
    * cp filename: copy filename to bookmark
    * df: get free space information
    * fat: get FAT type for current drive
    * rm filename: erase file or EMPTY directory
    * format drivename(a, b...): Format the selected disk (erase all data on it)
    * format32 drivename(a, b...): Force to format the selected disk in FAT32 (erase all data on it) [only possible if disk size allow it]
    * mv src dst: move file or directory
    * help: Display command helper

Example to copy-paste a file: Bookmark the destination location directory using the command "mark". Then go to the source location of the wanted copied file. (the destination directory should be different from the source directory) Type "cp thefileiwanttocopy.txt". Go back to the destination location, type "ls", you should see the copied file now.

Typing just cd will take you back to the root (pulled my hair for a few minutes before figuring this out..hehe..).

I tested copying a 1 MB file to a new folder. It took 20 seconds or 50,000 bytes/sec. Keep in mind it also has to read the same amount of data, so actual write speeds might be around 80,000 bytes/sec. I'm sure there are ways to improve the speed, which I'll be investigating.

Afterwards I checked the copied file against the original with Windows command line fc command and it matched, so ok there.

One thing I noticed, it takes much longer to do the freespace command on FAT32 cards (took 30 seconds on the 2 GB card). FAT16 is quick though. IIRC, space checking on FAT32 requires reading the whole FAT.

Long filenames seems to be supported (displays ok with the ls command, but I didn't try any operations on them).

I modded the free and partition space commands output to make it a bit clearer which is which and made it run at card insertion too (I added some card removed/inserted code too).

I'm not clear on how the SD card init is working. I'm guessing it's running on an interrupt because I'm not doing anything special when inserting/removing. It just works. :)

Installation:
Close AVR32 Studio.
In your workspace folder, via Windows Explorer create a folder called "fatex-sd".
Unzip the zip file there.
Start AVR32 Studio.
Click File -> New -> AVR32 C Project
Type "fatex-sd" (without quotes).
Use Default Location should be checked.
Change the Target MCU to UC3A0512ES.
Project type should fill in as AVR32 Standalone Executable.
Click Finish.
If you expand the fatex-sd tree (Project Exp pane), all the files should be there.
Right click the fatex-sd and click Properties.
In the left pane, expand the C/C++ Build tree.
In the left pane, click Settings.
In the middle pane under AVR32/GNU C Compiler, click Symbols.
In the toolbar just above the right pane (Defined symbols) click the little Add button.
Type "BOARD=EVK1100" (without quotes) and click Ok.
In the middle pane under AVR32/GNU C Compiler, click Directories.
In the toolbar just above the right pane (Include paths) click the little Add button.
Click the Workspace button.
Click the fatex-sd entry.
Click Ok, Ok.
Click Ok to close the Properties window.
Right click fatex-sd and click Build Project. Hopefully it is successful. :)

Plug a serial cable into UART1, start the Windows Hyperterminal, create a connection, standard 57600, 8, none, 1, none for flow control.

If you already have your JTAG setup, skip down to the Run As part.
Right click in the background of the ARV32 target pane (lower right of the screen).
Click Scan for targets.
Select the JTAGICE mkII.
In the lower middle pane, click the Properties tab.
Click the Board button.
Change Board dropdown to the EVK1100.
Change MCU dropdown to UC3A0512ES.
MCU clock source dropdown can be set to either Internal RC oscillator or crystal (the code does change to 12000000 after startup I believe).

In AVR32 Studio, right click fatex-sd and click Run As -> AVR32 Application.
Go back to Hyperterminal and have fun!

There's probably an easier way to install but read below:

I just got my EVK1100 a week ago and this is the first time I've played with C, and I don't have a clue how to do anything useful with this project yet... my programming background is Delphi for Windows, PIC, SX, and ATMega128 assembly, and BASCOM for AVR. And C seems umm.. unlike anything I've used before.. time will tell.. :)
