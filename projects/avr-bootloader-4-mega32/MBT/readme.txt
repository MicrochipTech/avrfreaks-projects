*************************************
*                                   *
*  AVR Universal Bootloader(AVRUB)  *
*                                   *
*************************************



Introduction
============
This project is a general AVR bootloader, for different type of AVR device,
all you need to do is modify macro definition, and you don not need to
modify the main program. There are many bootloader program on internet, but
most of them are only available for special types. When we need to use other 
types device, we must do a lot of modification. And many of them are only 
realize the basic functions, and have bugs, hidden problem or may be used
inconveniently, so we have developed this program. This project based on 
ATmega128 bootloader program which developed by Mr ChaoMa, and combined 
experience in bootloader of PIC18 microcontroller I have wrote before, and
used the macro definition skill from AVRUSB, and optimize and test the code 
time after time, finally get this project.
 
This AVR universal bootloader can support most types of AVR microcontrollers
(Mega series), which have self-programable capability, boot section and UART.
If the device have many serial ports, you can use any one of them. This 
bootloader supports RS232, RS485 and RS422. It can also supports USI, SPI, 
I2C interface if you made some modify.

This bootloader will occupy less than 1k words flash maximum, and it may 
occupy less than 300 words minimum. The space occupied by the bootloader is
relative with device type, configuration parameters, funcations you select,
and the optimize grade that the compiler use.



Main feature
============
* Support many types of AVR microcontrollers.
* Support AVR device with multi-uart.
* Support RS232/RS485/RS422 mode.
* Support customize communication baudrate and system clock frequcene.
* Automatically calculate the baudrate error.
* Write with AVR GCC, 100% C code.
* High optimized code, occupy small space.
* Cut out the function conveniently, and can meet different requirements.
* Support Watchdog.
* User may use the LED to show the upgrade status.
* Support to use super terminal as download software on PC.
* Support verification while write to FLASH.
* Can define the size of user program section.
* ...



Version
=======
v3.1  2007.Sep      According to Douglas Hammond's suggest, modify ComInit
                    macro. 

v3.0  2007.Jun      Fix bug on ATmega162.
                    Modify uart port number configuration.
                    Pass test with WinAVR 20070522.

v2.0  2007.Apr.10   Add verification function while write Flash, improve the
                    security of Bootloader.                 

v1.2  2007.Apr.02   Correct a hidden error in baudrate register calculate.
                    (This error is very common. We can find it in many books 
                    and codes in network)
                    
v1.0  2007.Mar.22   This is the first version. It realized all necessary 
                    functions, and extended XModem protocol to fit different 
                    application.



Develop environment
===================
The Develop environment of this project: WinAVR + AVR Studio
WinAVR version:        20070525
AVR Studio version£º   4.13.528

You may use other IDE or software to edit, debug AVR universal bootloader,
such as KamAVR¡¢AtmanAVR¡¢PN.



Project files
=============
Whole project include flowing files.

bootldr.c           Main program
bootldr.h           The header file of main program
bootcfg.h           User configuration of main aprogram
bootldr.aps         AVR Studio's project file of bootloader
test.c              Demo program to test uart communication
testcfg.h           Configuration file of test.c
test.aps            Project file of test
readme.txt          this file
readme.htm          HTML edition of readme.txt

Because many people have AVR Butterfly demo board, so all project file here 
use AVR Butterfly hardware as example. If you use other hardware environment,
all you need is to modify the congiguation parameter in file bootcfg.h.



Usage
=====
1. Creat a new project, and copy all above files to the folder where the new 
   project is.
2. Add bootldr.c¡¢bootldr.h¡¢bootcfg.h to the project.
3. Modify the configuration parameter in bootcfg.h. The meaning of these 
   parameters are introduced in next section.
4. Set the flash section address of the project as the beginning address of 
   bootloader. Here the address is counted as word. Besides that, we need to
   set some parameters, such as the type of device( if you use AVR Studio or
   KamAVR, can set the parameters in IDE, otherwise you need to manual modify
   makefile file yourself).
5. Compile the project, then generate the target HEX file.
6. Program the target HEX file of Bootloader to the mcu.
7. Set lock bits and fuse bits to configurate Boot section size and enable 
   Boot Reset Vector, and select clock source.
8. Connect target board to PC via comport, test bootloader.



Main parameters
===============
The flowing parameters are very important. Please set these parameters 
carefully according to user system's hardware.

BUFFERSIZE     The size of communication buffer. When use the hyper terminal,
               it must be equal to 128. This parmeter must be multiple or 
               submultiple of SPM_PAGESIZE which is the size of mcu's flash
               section page.
F_CPU          Frequency of system clock (in Hz). 
BAUDRATE       Baudrate of uart communication(bps).
LEVELMODE      Bootloader triger mode(0=uart  1= pin level)
timeclk        Basic time interval(ms)
TimeOutCnt     Bootloader maximal timeout count(<=255)
TimeOutCntC    The maximal timeout count when wait for receiving file(<=255)
CONNECTCNT     The length of password that will be used to connect to PC
KEY            The password to connect to PC. It can be hex numeral or string
COMPORTNo      Uart port number. One uart mcu is set to 0 or blank, to multi
               uart mcu, uart0 is 0, uart1 is 1, and so on.
WDGEn          Enable watchdog
RS485          Enable RS485/RS422 mode
RS485PORT      Control port of RS485/RS422 transmission(A/B/C/D...)
RS485TXEn      Control pin of RS485/RS422 transmission(PC0/PC1/PC2...)
LEDEn          Enable use LED to indication status.
LEDPORT        Port which LED used
LEDPORTNo      Pin which LED used
InitDelay      Additional delay after uart initializtion(some early device 
               need this, such as ATmega8)
VERBOSE        Hint mode. Show more prompt information, interactive input will
               be convenient while using hyper terminal.
CRCMODE        Communication check sum method(0=XMODEM CRC 1=simple accumulated
               summation)
BootStart      The start adrress of Boot section(counted by byte)
ChipCheck      Enable verify data after write data to flash. This parameter 
               will only valid when Bootstart used 



Tested device
=============
The AVR universal bootloader has benn test in several AVR device, below devcie
are tested by myself:

  ATmega8
  ATmeag64
  ATmega128
  ATmega168
  ATmega169(Bufferfly)

below are tested by other people:

  ATmega16
  ATmega32
  ATmega162

If you have success use this bootloader in device not list above, please send
EMAIL to me.



Frequently asked questions
==========================
1.Q: How to protect boot section?
  A: In order to prevent boot section avoid overwrite, user must set lock bits
     of mcu. Commonly we set Bootloader protection mode to 2 or 3 by set BLB11
     and BLB12, disable boot section write operation.
2.Q: Why use watchdog in bootloader?
  A: In order to increase system stability, I suggest use watchdog, to avoid 
     bootloader enter dead loop.
3.Q: Is hyper terminal may use to download target HEX file?
  A: Yes, you can use hyper terminal as PC download software. In order to use
     hyper terminal as download software, macro BUFFERSIZE must equal 128, and
     KEY will not set to long(1-2 characters), macro TimeOutCnt/TimeOutCntC/
     timeclk must define large enough to let you have enough time to input. 
     And macro CRCMODE must equal 0, and you need convert target HEX file to 
     BIN format before download.
     In fact, AVRUBD(AVR universal bootloader download) is a better software
     than terminal, AVRUBD support HEX/BIN format, after you load target file,
     all you need is click download button.
4.Q: How to confirm uart work fine(comport line link correct)?
  A: Demo program test.c may use to test uart. If link correct, compute will
     receive a '>' periodically from AVR mcu. If you send a character to mcu
     via Hyper terminal, send data will become to what you send.
5.Q: Why bootloader can't jump to user's application after upgrade?
  A: If this happen, there are most likely to two reasons: one is compiler's
     bug, made jump incorrect. You may see disassemble code to analyse jump
     codes, or try to update you compiler to new version. The other reason is
     not set fuse bits and lock bits correctly.
6.Q: How to cutdown unused function?
  A: In file bootcfg.h, there are many macro definition, set macro to 0 will
     disable corresponding fuction. For example, not use LED in bootloader,
     set macro LEDEn to 0.
7.Q: How to reduce bootloader target size, leave more flash space to user?
  A: Cutdown unnecessary functions, such as verbose hint, LED indication, use
     add up instead of CRC as check sum... And you can also set optimization
     of compiler, these will reduce target file size. Due to indeterminacy of
     optimization, code size optimize will not get the smallest size in some
     condition.
8.Q: If AVRUB support other AVR C compiler?
  A: AVR universal bootloader use AVR GCC as compiler, other C compiler have
     not been test. IAR C compiler is very close to AVR GCC, I think there
     are only few place need to be modify(AVRUSB is compatible with AVR GCC
     and IAR C compiler). Other C compiler may be use too, because I write
     code according to ANSI standard. The code may need to modify is head
     file, some register name may be different.
9.Q: Where can user get last version of AVRUB?
  A: In http://shaoziyang.googlepages.com or http://shaoziyang.bloger.com.cn
     you will get last version or AVRUB. If you have any suggestion, or link
     above can't be access, please send email to me shaoziyang@gmail.com.



Special thanks
==============
There are many people given me help in develop this project.

  Áõº£ÌÎ    give many help in coding and debuging
  tda1552   Tested ATmega32
  ÐìÑÓ¿µ    Found bug in HEX format convert
  Äß¶÷Î°    give some useful suggestion
  ³ÌÏè      help to translate this document to English

And many people who have not left names.



License
=======

Base on GPL license.



2007.6 Shao ziyang in WuHan



                        e Y8b    Y8b YV3.08P888 88e
                       d8b Y8b    Y8b Y888P 888 888D
                      d888b Y8b    Y8b Y8P  888 88"
                     d888WuHan8b    Y8b Y   888 b,
                    d8888888b Y8b    Y8P    888 88b,
        8888 8888       ,e,                                  888
        8888 888820078e  " Y8b Y888P ,e e, 888,8, dP"Y ,"Y88b888
        8888 8888888 88b888 Y8b Y8P d88 88b888 " C88b "8" 888888
        8888 8888888 888888  Y8b "  888   ,888    Y88D,ee 888888
        'Y88 88P'888 888888   Y8P    "YeeP"888   d,dP "88 888888
888 88b,                    d8  888                     888
888 88P' e88 88e  e88 88e  d88  888 e88 88e  ,"Y88b e88 888 ,e e, 888,8,
888 8K  d888 888bd888 8Shaoziyang88d888 888b"8" 888d888 888d88 88b888 "
888 88b,Y888 888PY888 888P 888  888Y888 888P,ee 888Y888 888888   ,888
888 88P' "88 88"  "88 88"  888  888 "88 88" "88 888 "88 888 "YeeP"888



       __
      / _)
     /(_)(/                                    __
         /                                    (_ |_  _. _  _ o   _.._  _
                                              __)| |(_|(_) /_|\/(_|| |(_|
                                                              /        _|


