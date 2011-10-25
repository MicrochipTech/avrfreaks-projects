# ATmega644 MOD/S3M player

Uploaded by smaslan on 2011-10-25 14:11:00 (rating 5 out of 5)

## Summary

This project is simple embeded player for music modules in ProTracker (MOD) and ScreamTracker3 (S3M) formats. It is able to load songs from IDE interface device (HDD od CF card) with FAT16 partition or via serial port. Because most of listenable trackers exeeds size of RAM or FLASH space, song has to be stored in SW addressed external SRAM. This makes HW a bit more complicated and access a bit slower but there is no other way ...  

Since I'm using 16-bit stereo I2S DAC (or similar TDA1545) and linear interpolation resampling it sounds very good even at sampling rates below 20kHz. That's neccessary for more than 8-channel trackers with XTAL=20MHz. With ATmega644 overclocked to 32MHz (I had no problems with that) it is possible to play 12-channel trackers at sampling rates above 20kHz (all channels active at once). 


More informations about this project and its possible updates:  

<http://www.elektronika.kvalitne.cz/ATMEL/S3Mplayer/S3Mplayer_eng.html>


Video demonstration of some S3M files played:  

<http://www.youtube.com/watch?v=j6ijbexoq-M>


[![So called HW: S3M-player with nice big LCD.](http://www.elektronika.kvalitne.cz/ATMEL/S3Mplayer/obr/nahled/foto_LCD_1.jpg "So called HW: S3M-player with nice big LCD.")](http://www.elektronika.kvalitne.cz/ATMEL/S3Mplayer/obr/foto_LCD_1.jpg)

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
