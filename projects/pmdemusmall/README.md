# PMDEmu_small

Uploaded by wurmi on 2003-09-09 17:00:00 (rating 0 out of 5)

## Summary

Continue to project [#118](https://www.avrfreaks.net/Freaks/freakshow.php?keyword_is_id=1&keywords=118) (emulator of Czechoslovak microcomputer PMD-85). 


I started work on new board based on ATmega8515-16, high speed 64 KB SRAM (from old 486 cache system) and 512 KB FLASH memory. ATmega8515 offers extra 3 bits portE, which is very useful to generating composite PAL video signal in high resolution 288x256. PE0 is used as video data output and PE2 (OC1B) is used to generate sychronization signal (fast PWM mode). Mixing this two signals in resistor network gives full composite video. I overclock Atmega to 20 MHz, because lot of processor time is spent in video generation routine and main program is running in blanking areas of TV lines. Video memory is approx. 16 KB and each byte is interpreted as 6 pixels with 2 bits attributes (one is blink attributte and one is not used now). Human also interact to microcomputer with PC/AT keyboard and speaker.


Software is stored in monster external FLASH memory. Games as Manic Miner,Boulder Dash, Tetris, Saboter, Raiders,Flappy, Labyrithm, Willy and some BASIC games (with BASIC interpretter).


UPDATED 22/09/2003 - added RLE compression games - save up to 64 KB (12.5 % ratio). Additional info you can found [here](http://www.kmit.sk/~peto/AVR/smallpmd/index.php).  
  
  

[![Build it, its cool!](http://www.kmit.sk/~peto/AVR/smallpmd/up1.jpg)](http://www.kmit.sk/~peto/AVR/smallpmd/up1.jpg)

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
