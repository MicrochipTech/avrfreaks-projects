# LCD Nokia6100+SD(FATFS)

Uploaded by GEA on 2011-10-17 07:39:00 (rating 0 out of 5)

## Summary

working draft LCD Nokia6100+SD(FATFS).


Have the opportunity to view the BMP (to 128x128, 8bit color) files from card SD (SKIN/bmp0.bmp).


Lcd nokia 6100-series(Philips PCF8833)controller  

ATMega128 - PORTA (SOFT Protocol):  

CS -PINA3  

CLK -PINA2  

SDA -PINA1  

RESET -PINA0


Lcd pinout:  

1 VDD 3,3V  

2 /Reset  

3 SDATA  

4 SCLK  

5 /CS  

6 VLCD 3,3V  

7 NC  

8 GND  

9 LED-  

10 LED+ (6V) 


For SD used port SPI, pinout is in the description avr\_ffsample.zip.  

I used fat-file system "Fatfs" from [http://elm-chan.org/fsw/ff/00ind...](http://elm-chan.org/fsw/ff/00index_e.html)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
