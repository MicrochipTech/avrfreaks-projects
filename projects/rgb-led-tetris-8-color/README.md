# RGB LED Tetris 8-color

Uploaded by Hexorg on 2011-01-03 16:08:00 (rating 0 out of 5)

## Summary

Hello everyone, this is my DIY tetris game that's implemented using  

 256 RGB LEDs that I bought on eBay,  

 3 "74ACT04PC" - HEX Inverters,  

 1 "SN74154N" - 4-to-16 decoder,  

 1 "CD4520BE" - 4-bit Up-counter,  

 1 "ATmega168P" - AVR's microcontroller,  

 16 NPN transistors,  

 6 "M74HC4094B1R" - 8-bit shift register with latches,  

 1 "LM34" - Precision Fahrenheit Temperature Sensor, and  

 7 push-buttons - for controls


In the schematics I showed connection of 8 rows of 16 RGB LEDs, because  

my SPICE program started getting laggy with so many connections. But the  

pattern is the same. LEDs I bought have common anode and 3 cathods for  

Red, Green, and Blue. Essentially 1 RGB LED is like 3 LEDs that share an  

anode, so all anodes connected in rows to transistors, and all cathods  

are connected in columns to shift-registers.


// Pin definitions of the microcontroller ATmega168P  

// PB1 (pin 15) is the "next line" onRisingEdge clock. for the upCounter  

// PB2 (pin 16) in the "Strobe" line for the shift registers to latch the data  

// PC4 (pin 23) in the "Output enable" line for the shift registers to output the data  

// PC5 (pin 24) in the "Reset" line for upCounter. keep low  

// PB5 (pin 19) in the "clock" line of the shift-register  

// PB3 (pin 17) in the "Serial In" of the shift-registers


// PB0 (pin 14) - 7th input button  

// PD7 (pin 13) - 6th input button  

// PD6 (pin 12) - 5th input button  

// PD5 (pin 11) - 4th input button  

// PD4 (pin 6) - 3rd input button  

// PD3 (pin 5) - 2nd input button  

// PD2 (pin 4) - 1st input button


As far as the code goes, I tried to make the game as simple as possible, trying to use more code and  

less variables, since ATmega168P has only 1024 bytes of SRAM, and 16kb of flash. Also font is created  

from "Terminus" font for linux, and is stored in EEPROM.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
