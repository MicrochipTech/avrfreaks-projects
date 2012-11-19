# USB HID demo

Uploaded by dzairo on 2012-11-18 17:11:00 (rating 0 out of 5)

## Summary

Hi. There is it. Few USB HID example demo for ATmega32U4.  

I'm searching easy code for AVR MCU with hardware USB. I use ARDUINO Leonardo dev. board with internal 8Mhz RC oscillator. On this site <http://www.pjrc.com> is Teensy project and there is easy RAW HID project. I download it , read and read again for better understanding. Then rewrite some part of code for less flash used and for better understand. In Delphi 7 write some user software for communication with my dev board.  

Example demo code is:


AVR:  

HID\_max\_send\_receive:  

Is easy demo code for communication with PC and MCU with maximum data size.Use Endpoint 0 - maximum is 64 user data bytes.  

Function:  

GetReport : read demo data from MCU and send to PC -> PC request  

SetReport : send demo data from PC to MCU -> PC send  

SendData: MCU send demo data to PC -> MCU request, if change state of predefined pin.


HID\_eeprom :  

Is easy code for send or receive data from to internal EEprom memory.User button is used for define where store data , if from 0 position or 128 position.  

Function:  

GetReport: read data from MCU internal eeprom and send to PC-> PC request  

SetReport: send data from PC and write to MCU internal eeprom -> PC request  

SendData: not used


HID\_usart:  

Is easy demo code for USB <-> USART communication. MCU use 9600bps speed. timeout for UART receive is 32ms (TIMER0 free running).  

Function:  

GetReport: not used (read demo data)  

SetReport: send data from PC to MCU and MCU send it to USART.In one step only 64bytes  

SendData: MCU receive data from UART and fill buffer. IF buffer is full then send to PC.  

IF not fill buffer and not receive data more then 32ms then send also buffer(all other bytes are 0).  

This is for simple communication with serial port devices. I tested with GL865 dual GSM module.


Delphi:  

HID\_max\_send\_receive:  

Easy demo code for communicate with my AVR demo code.After start then list all HID devices. If select one then enumerate and we can communicate with device.  

Grid is for editing user buffer (64 bytes).  

Function:  

GetHex: read data from MCU and show in grid and memo.  

Sethex: send data from grid to MCU  

SetStr: send data from memo to MCU.  

Simple communication.


HID\_usart:  

Easy demo code for PC(USB) <-> MCU(UART) communication to work together with AVR HID\_usart demo code.


All this demo code is easy and tested with my ARDUINO Leonardo dev board with internal RC 8Mhz oscillator. Not check report size. Do not use report ID only raw communication.


Big thanks to Paul Stoffregen from PJRC and Dean from LUFA,  

of course all other what help me too.


I'm beginner for AVR MCU with hardware USB and I want rewrite my older project used V-USB. Searching any easy demo code and find RAW HID in PJRC site.  

This is for all beginner , hobby and enthusiasts.


PS:  

Now testing HID USB communication using report ID. Work but got some problem.  

I test more GetReport ID in report descriptor and SetReport ID in same time with different size and work good. But I don't know how to send data from MCU with out PC request with predefined report ID. If any one know it pls write to me.In communication use use Endpoint 0 for maximum 64bytes size of data user if know any one how to send receive max 256bytes will be glad, pls write to me .


One more thanks to all for help.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
