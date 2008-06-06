# 2.4 GHz Wireless Development Kit

Uploaded by Decibit on 2008-06-06 01:31:00 (rating 0 out of 5)

## Summary

Now available in the United Kingdom by Kanda:  

[http://kanda.com/products/Decibi...](http://kanda.com/products/Decibit/DCBT-DDK.html)


Decibit Development Kit (DDK) is a combination of HW and SW for the NordicSemi nRF24L01 and Atmel ATmega168 based RF modules operating in the 2.4 GHz ISM band.


Developers no longer need to read through entire Nordic datasheet, because each function is now accessed by a single CALL instruction, which is provided by the firmware residing in the Decibit RF modules.


It's like adding RF periphery to the ATmega168 and accessing it's functions by simple API calls.


RF modules are loaded with the user code direct and they operate stand alone, no external MCU is needed.


FLASH/EEPROM is programmed similar to how usual Atmel AVR's are programmed, serially, via SPI. The USB programmer HW/SW is included in the DDK.


Example user codes are open source, which include Ping-Pong range test, remote control applications etc. After opening the DDK, only 10 minutes maximum is needed for loading user code on modules and to demonstrate a working link.


Other manufacturer/competitor provide ready coded modules with fixed  

application, frequency and RF output power. With Decibit, this is separated. Any application is open source and can be programmed into modules, all features of nRF24L01 can be exhausted.


User codes are pretty short and easy to read, although currently written in AVR assembly.


Maybe we can call this a competitor to ZigBee. Maybe this is  

easier/cheaper/faster/better than ZigBee. Less size, less current  

consumption, less price in every aspect. All what ZigBee claims  

(multi-link, multi-node) can be done with nRF24L01. NordicSemi has  

written a whitepaper to compare both worlds, to be found on their website.


Now that easy access to nRF24L01 is provided by the firmware in the RF  

modules, all rounded up and made easy to start with by the DDK, why  

shouldn't this solution be a hit ?


In (not so) short words, one can program and put in their own code  

inside the MCU on the RF module itself. The tiny module then (with added  

battery) will TX/RX data over RF, nothing more is necessary. Plus the  

user code is so small, as example only 7 assembly code lines to have a  

valid user code. This is because there is the operating system in the  

module which accepts API calls. Variety of open source user code examples are available and hopefully this number will grow further. 


More advanced code examples are to interface a PC via the USB programmer itself (FTDI232RL chip) to your wireless link. 


Former DDK price was 69 USD.  

DDK #2 recommended price is 95 USD. (Includes tiny remote control DCBT-24R6)

## Links

- [Development Kit Contents](http://www.decibit.com/DDK.pdf)
- [Windows Installer for example user codes](http://www.decibit.com/SetupDDK.exe)
- [Datasheet](http://www.decibit.com/dcbt-24.pdf)

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
