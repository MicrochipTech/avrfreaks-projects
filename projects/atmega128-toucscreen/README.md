# ATMega128 + toucscreen

Uploaded by GEA on 2011-10-17 07:40:00 (rating 0 out of 5)

## Summary

This is a test project, it determines the coordinates of the variables (X & Y), and prints. Using these coordinates can be used to determine where you press the on-screen virtual keyboard for the organization and so on.


Used alternately two ADCs (ADC0-X, ADC3-Y). More details in doc8091.pdf.


Toucscreen from PDA ASUS-A696. 


Lcd nokia 6100-series(Philips)controller.


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
