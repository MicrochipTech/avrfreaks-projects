# ATMega128 + LCD Nokia6100

Uploaded by GEA on 2011-10-17 07:37:00 (rating 0 out of 5)

## Summary

working with ATMega128 + LCD Nokia6100. PALITRA for picture BMP 8bit,  

BMP receives from UART,CPU clock 14745600Hz baud-128000,to receive used port - UART1 ATMega128, i used (USB-UART FT232R),from the PC to use hyper-terminal program.


If you want use receive BMP(128x128\_8Bit color) from UART - you must uncomment in file nokia\_6100\_test.c:


//while(1){  

// bmp\_rcv\_uart();  

//}


Lcd nokia 6100-series(Philips PCF8833 132x132 pixels)controller.  

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

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
