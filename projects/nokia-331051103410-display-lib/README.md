# Nokia 3310/5110/3410 display lib

Uploaded by Kimmeridgien on 2011-01-15 17:06:00 (rating 0 out of 5)

## Summary

Joey has adapted this library so that it can alternatively be used for the Nokia 3410 display. It is using the same controller chip and pinout but has a higher resolution: 96*65 pixels (7 rows * 16 characters + 1 additional unused column to the right). 


Versions 1-3 are for 3310 or 5110, while version 4 is for 3410.


The import macro and example file have been updated. It was developed using commercial Bascom AVR 1.11.9.8 with an Atmega 8 running @ 4MHz. 


Joey also included a picture of the lcd, showing the 'bootlogo'. He's going to use this display for his fish tank, controlling lights, pumps, oxygen, pH etc. 


Many thanks to Joey for his contribution! To get in touch with him, please send a private message to LiquidSmoke.


----


Previous versions:


This is version 3.0 with sch and pcb layouts for standalone (master) or slave operation:


* Schematics, pcb layouts, and photos added as a separate download.


* The master board is a complete 5 cm high * 4.5 cm wide * ~1 cm thick proto board for ATMega48/88/168 including the display. The master board is suitable for a measurement / backlit LCD display / I2C / power control / SPI memory attachable device. Please notice that the ATMega48/88/168 only supports 20 MHz when fed by 5.0V. For 3.3V the maximum supported clock frequency is about 12 MHz. The ceramic resonator can be omitted if you need the extra I/O.


* The slave board is intended as a small-size replacement for perhaps the HD44780 displays. (They are not compatible, but this slave board requires fewer pins so it can be used in an existing design with reasonably little code rewriting.) For 5.0V operation you must use all components and LEDs connected as 2+2+resistor. For 3.3V operation only include the 4.7uF capacitor and connect the LEDs as 4+resistor. The LEDs are reused from the scrapped old Nokia phone.


* The BASCOM basic code hasn't changed since v2.0, that includes support for printing bitmap images.


---


The Nokia 3310 display is a cheap and widely available black dot matrix display with 4032 dots arranged as 6 rows * 14 characters or 48*84 pixels.


It is supplied by 2.7 - 3.3 V and only requires an external capacitor for the lcd voltage generation.


Included in the BASCOM basic library v2.0 .zip file are:  

3 library files for the BASCOM-AVR compiler  

1 example main program file  

1 MS Excel simple character generator  

1 MS Excel bmp-to-hex converter also published in its own project  

1 example .bmp file  

1 Philips datasheet for the PCD8544 controller  

2 photos of the code used in a real application.


The BASCOM-AVR basic compiler is available for download at [www.mcselec.com](http://www.mcselec.com). Version 1.0 of this library was created using the free demo version limited to 4 kB program code. Version 2.0 (with added bmp support) requires 4.7 kB program code, so it more or less requires that you buy the commercial version of BASCOM-AVR. (I am not affiliated with this company.)


The file 3310init.bas is thoroughly commented and includes detailed instructions. It is my hope that they will answer all your questions. For more information you could also consult with the BASCOM-AVR help menu. If you have feedback or advice, please send a private message to user name Kimmeridgien here at [www.avrfreaks.net](https://www.avrfreaks.net).


Enjoy!


/Niclas aka Kimmeridgien

## Compilers

- BASCOM-AVR

## Tags

- Complete code with HW
- BASCOM-AVR
