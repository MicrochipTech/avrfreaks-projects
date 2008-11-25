# DIY ATmega32 Starter's Kit

Uploaded by DHARMANI on 2008-11-25 16:17:00 (rating 0 out of 5)

## Summary

The project is a DIY kit based on ATmega32 interfacing with verious peripherals included on a home-made single-layer PCB.  

The PCB includes LCD interfcacing, RTC DS1307, ADC connector, LED, 6 push-buttons and RS232 interface ic MAX232. 


Details can be viewed at:  

[http://www.dharmanitech.com/2008...](http://www.dharmanitech.com/2008/08/make-yourself-atmega32-starters-kit.html)


Attached files:  

- The individual pdf files are for those who want to try the LCD, UART, ADC separately.  

- A zip file is attached which containes complete project source code file along with the ICCAVR project file. The project contains C library for UART, ADC functions, RTC DS1307 library, i2c library, LCD library and the main function.  

- The compiled project HEX file is also attached


When the kit (I call it M32\_Card) is ready, load the hex file in the ATmega32 (set at 16 MHz crystal frequency, with JTAG DISABLED). Connect RS232 port of PC with the card, run the Hyper terminal at 19200 Baud (No Parity, Hardware flow control -None), and then power the kit. It will send a welcome message to PC and a welcome message will be displayed on LCD screen, too.  

You can test functionality of each hardware unit on the M32\_Card by selecting the options displayed on Hyper Terminal window (like reading- writing to RTC, checking push buttons, checking LED and checking ADC)


If any doubt, post your comments here:  

[http://www.dharmanitech.com/2008...](http://www.dharmanitech.com/2008/08/make-yourself-atmega32-starters-kit.html)


Thank you!


Dharmani,  

[www.dharmanitech.com](http://www.dharmanitech.com)

## Links

- [DIY ATmega32 Starter's Kit Details](http://www.dharmanitech.com/2008/08/make-yourself-atmega32-starters-kit.html)

## Compilers

- ImageCraft ICCAVR V6

## Tags

- Complete code with HW
- ImageCraft ICCAVR V6
