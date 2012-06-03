# AVR Universal Bootloader v4.5

Uploaded by shaoziyang on 2012-06-03 12:50:00 (rating 0 out of 5)

## Summary

I have move AVRUB website to new place, and I plan to rewrite AVRUBD use Lazarus, so it may run in multi platform, such as win32, linux, MacOS etc.  

[https://sites.google.com/site/sh...](https://sites.google.com/site/shaoziyang/Home/avr/avr-universal-bootloadere)


[Bug fix]  

There two serious bugs found in V4.5, because new version has not publish, so I fix these bugs here.


1. In file bootldr.c, add below in line 475 and 499:  

 bufptr -= BUFFERSIZE;  

It will cause download fail when CRC error. Thank Ben Wilson found and fix this bug.


2. In file bootldr.c, replace lines  

(*((void(*)(void))(BootStart)))();  

to  

(*((void(*)(void))(BootStart/2)))();


[v4.5]  

 Add SafeMode


*Due to some reason(network filter in China), I can't visit Googlepages to update my web, so files in <http://avrubd.googlepages.com> are still old version.


[v4.2]  

 Add AES crypt routine (128/256 bits key)  

 Fix a clerical error in WDG initialize  

 Fix a bug in LED initialize, may cause RS485 not work  

 Optimize code, reduce code size


[v4,1]  

 fixed two bugs. Many thanks to seu gab.


[v4.0]  

 add PC1 encrypt function.


[v3.5]  

 fix bug of Flash check error above 64K.  

 Auto create batch file 'avrub.bat' in AVRUBD's autocode, you don not need to set segment offset in AVR Studio, more easy to use.


[v3.2]  

 fixed a uart bug of URSEL.


--------------------------------------------------------------------------


A universal Bootloader for all AVR mcu with boot section, with many feature.


*easy to use  

*100% C code, easy to modify  

*support almost all AVR mcu with boot function  

*highly optimized  

*Support RS232/RS422/RS485 communications  

*support multi frequency and baudrate  

*Auto calculate baudrate  

*Support watch dog  

*Support Window's hypertrm to download file  

*LED indicate bootloader status  

*support function cutdown to reduce code size  

*support write verify


and more...


Please note that AVRUB's webpage moveto  

<http://avrubd.googlepages.com>


Discuss group:  

[http://groups.google.com/group/a...](http://groups.google.com/group/avrub?hl=en)

## Links

- [AVRUB v4.1 English version (Googlepages mirror)](http://avrubd.googlepages.com/avrub_en.zip)
- [AVRUB v4.1 Chinese version (Googlepages mirror)](http://avrubd.googlepages.com/avrub_cn.zip)
- [AVRUBD v4.1 (googlepages mirror)](http://avrubd.googlepages.com/avrubd.zip)
- [AVRUBD 4.1 sources (googlepages mirror)](http://avrubd.googlepages.com/avrubd_src.zip)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
