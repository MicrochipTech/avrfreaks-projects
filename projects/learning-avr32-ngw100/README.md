# Learning AVR32 with NGW100

Uploaded by desmondzeng on 2009-10-18 06:37:00 (rating 0 out of 5)

## Summary

Learning to use AVR32 Studio with NGW100.


Learning to use AP7000/AP7001 for future projects 


Using Vmware with Ubuntu 9.0.4 on Window XP SP3  

with avr32 toolchain, ckermit


To get ckermit, type sudo apt-get install ckermit


Item No.1 - How to install Linux kernel image into NGW100


1. Download the linux kernel and store onto your desktop  

Here is using openwrt-avr32-2.6-squashfs.img from [http://downloads.openwrt.org/kam...](http://downloads.openwrt.org/kamikaze/7.09/avr32-2.6/)


2. Open the terminal and cd Desktop


3. type sudo kermit in your terminal


4. set your kermit serial comm to the following, here i will be using ATEN usb to serial cable tied to USB0


set line /dev/ttyUSB0  

set speed 115200  

set carrier-watch off  

set handshake none  

set flow-control none  

robust  

set file type bin  

set file name lit  

set rec pack 1000  

set send pack 1000  

set window 5  

connect


Now you will be connect to NGW100 serial com.  

Press the reset button on NGW100 to see your Uboot.


If you do not have uboot loaded in the NGW100 do the following  

NOTE!!! You will need to get hold of a JTAGICE mkII before you proceed


a. Download u-boot.bin from [http://www.atmel.no/buildroot/bi...](http://www.atmel.no/buildroot/binaries/v2.3.0/atngw100/)  

b. go to Terminal and type  

avr32program erase -fcfi@0  

avr32program program -F bin -fcfi@0 u-boot.bin


5. Now that you have both kermit and Uboot ready, now go to Uboot console and type the following


uboot> protect off 0x20000 0x7EFFFF  

uboot> erase 0x20000 0x7EFFFF  

uboot> loadb 0x90000000


This will erase the flash rom to allow us to copy the image over from the SDRAM to Flash Rom.


6. CTRL-\ C to exit uboot console back to kermit console and send the image file  

ckermit> send /bin openwrt-avr32-2.6-squashfs.img


This will take a few minutes


7. Now to copy the image from the SDRAM location to the Flash ROM


ckermit> connect  

uboot> cp.b 0x90000000 0x20000 0x"size of image file"  

uboot> protect on all


8. Now set the bootcmd and bootargs


UBoot> setenv bootcmd 'bootm 0x20000'  

UBoot> setenv bootargs 'console=/dev/ttyS0,115200 root=/dev/mtdblock2 rootfstype=squashfs,jffs2 init=/etc/preinit'  

UBoot> saveenv


9. Now Enjoy your newly loaded Linux Kernel :D


Great Thanks to all the contributers in various forum and blog especially the following below


[https://forum.openwrt.org/viewto...](https://forum.openwrt.org/viewtopic.php?pid=60020)
