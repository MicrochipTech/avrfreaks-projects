# linux bootloader uploader

Uploaded by iggie01 on 2009-08-25 14:47:00 (rating 0 out of 5)

## Summary

Danni's bootloader (fast tiny & mega UART bootloader) is fast and tiny, and works on many AVRs just by including the part's ".INC" assembly description file.  

[https://www.avrfreaks.net/index.p...](https://www.avrfreaks.net/index.php?module=Freaks%20Academy&func=viewItem&item_id=1008&item_type=project)


Its only short-coming is that the program that talks to the bootloader is written for DOS, with at least one Windows version floating around the net.


Then, Andreas Butti contributed an almost-working linux version here:  

[http://www.mikrocontroller.net/a...](http://www.mikrocontroller.net/attachment/35618/Bootloader21_20080510.tar.gz)


Finally yours truly finished it off and made it work on OS X and Linux.


Speed:  

ATmega164P @ 18.432 MHz; 200 MHz Linux/ARM9 SBC @ 115200 baud  

Flash/Verify: 13563 bytes in 2.51 seconds  

Verify: 13563 bytes in 1.57 seconds


Options added in new version (1.1):  

* Optional character sequence to send to AVR to initiate reboot  

* Optional limit on retries to connect to AVR's bootloader  

* Optional silencing of all output except errors  

* Errors to stderr, exit 0 on success and -1 on failure


Enjoy!

## Tags

- Complete code
