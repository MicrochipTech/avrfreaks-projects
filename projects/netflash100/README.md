# NetFlash100

Uploaded by ishraq86 on 2011-11-13 08:01:00 (rating 0 out of 5)

## Summary

*** FOR SOME PROBLEMS ON THIS SITE COULDN'T POST THE FILES OR ANY LINKS SO I'M GOING TO POST THE PROJECT LINK HERE. ***


[https://sourceforge.net/projects...](https://sourceforge.net/projects/netflash100/)


[DESCRIPTION]


NetFlash100 contains a full system image with a Linux kernel, root file system and the web interface to access flash memory of ATMEL ATNGW100 boards.


To be able to use NetFlash100 you need:


1. ATNGW100 board with a working u-boot.


2. Ethernet and serial terminal connections.


3. A SD card (must be more than 40MB)


Once you successfully boot NetFlash100 image, it provides you with a web interface on IP 10.0.0.1, with this interface one can read, write and erase the available flash devices on the system. It can also add, modify or delete u-boot parameters. NetFlash100 also provides information about the system and it's flash devices that might be useful.


[YOU NEED]


1. Functional ATNGW100 board with a working u-boot bootloader.


2. MMC card (more than 40MB capacity).


[YOU HAVE TO DO]


1. Copy the image (NetFlash100.img) on your MMC card. To do this,  

unmount your MMC card from your system if it's mounted and issue  

the following command from your unix like system as root,


dd if=NetFlash100.img of=/dev/mmcblk0


change /dev/mmcblk0 according to your system. But it should be your  

MMC card.


2. Once image copy is done put the MMC card in your ATNGW100 slot and boot  

using the following u-boot boot parameters,


bootargs=> console=ttyS0 root=/dev/mmcblk0 rw rootfstype=ext2 rootwait  

bootcmd=> mmc init; ext2load mmc 0:0 0x10400000 /boot/uImage; bootm 0x10400000


NOTE: the command "mmc init" is "mmcinit" in older versions of u-boot.  

So change according to your system.


3. Once the system boots up connect ehternet with the LAN port of the board.  

Do IP configuration on your system as,


IP: 10.0.0.2  

NETMASK: 255.255.255.0


then connect to the NetFlash100 GUI by typing 10.0.0.1 on your browser.


4. Now you have an easy to use GUI to read, write or erase your ATNGW100 flash  

memory. You can even update your bootcode this way!


5. You can login to the NetFlash100 linux shell as root as well.


User: root  

Pass: root

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
