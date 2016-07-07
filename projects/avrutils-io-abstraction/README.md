# AVRUtils - I/O Abstraction

Uploaded by dthreel on 2016-07-07 11:00:00 (rating 0 out of 5)

## Summary

Hello guys,


 


I'm really excited to show you my first attempt on how to solve hardware abstraction of AVR 8 bit microcontrollers:


I have created a C library which lets you abstract away I/Os in a clean, efficient and straightforward manner.


Please have a look at the example code provided. (example.c and example\_bus.c)


 


*The name (AVRUtils) may be a bit misleading at first but I'm planning on abstracting other things as well (TWI, UART etc.).*


 


Please keep in mind that I yet only had time to test it on an ATMega2560t but I see no problems for other AVR micro controllers (ATMega8, ATMega16 etc..).


 


The library with all informations can be found on my github: <https://github.com/marco-a/AVRUtils> .


 


As implied at the time there is only an I/O module available which takes care of abstracting away the manual need of writing into DDR, PORT etc. registers. 


 


I'm now looking for first impressions, so if you have any feedback please let me know. :)


 


d3L

## Tags

- General Lib.functions
- avr
- Library
- c
- abstraction
