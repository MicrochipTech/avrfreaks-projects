# binary counter AVRISP test - first attempt at AVR anything...

Uploaded by rentzu on 2005-07-20 23:07:00 (rating 0 out of 5)

## Summary

:: overview


testing a low cost AVR programming setup. code functionality is secondary to the ability to load the hex into the AVR's flash and eeprom with ghetto budget techniques and tools. parts were sourced from digkey.com and local Fry's Electronics and RadioShack. an ATmega16L was chosen for the MCU. because it looked badass and was $7. this is just how i did it so surely there are better and more informed ways, so dont be all like, "YOUR WAY SUCKS!!" [shrug] photos, the .asm used for the test, and the AVRISP manual .pdf (harder to find than the hateful .chm) files included.


:: parts


1x ATmega16L 40dip ($7)  

1x AVRISP serial dongle ($30)  

1x LiteON dual blue LED 7seg .40" display ($8)  

10x RadioCrap random NPN switching transistors ($2 for 15, ha)  

1x 10K resistor ($1)  

10x 1K resistor ($1)  

8x 220R resistor ($1)  

1x 5x2pin .1" spaced PCB header ($2 for a 72pin strip, cut to length)  

1x small .1" spaced prototype PCB (like $2 worth...)  

1x 3-5v PSU (steal a wallwart from something, FREE)  

1x spool ~20awg solid core wire ($3 if you dont have it...)  

2x breadboard ($12 at Fry's, you should really have these tho...)  

1x soldering setup ($20 at RadioCrap but like you should have this stuff already, no?)


Total: $54 ($89 if you just found out what electronics is yesterday, you probabaly learn quick tho if youre reading this kinda stuff so you might be okay. keep up.)


:: the dongle to AVR interface


first the AVRISP <-> AVR interface. solder the 5x2 header to the small prototype PCB. use the AVRISP manual for the pinout, pin1 is upper left. remember thats backwards when youre soldering from the other side. solder the wires passing thru the PCB to the pins, remember the GND wire is 5 of the pins, so do that one first. check for bridged solder gaps, and that the AVRISP cable fits. THE RED WIRE SHOULD BE AT THE TOP OF YOUR HEADER, by the Vcc and MOSI pins. youre done with that. you should have 6 wires coming from your interface board now:


Vcc GND MISO MOSI SCK RST


:: get the AVR ready


now for the AVR. get one of those breadboards. get power to it (you got a wallwart and a soldering iron and a spool of wire, be creative). put the AVR in it. (i put the ATmega16L into a 40dip socket, then stuck it in the breadboard. broke pins suck, but whatever floats yours...). use the pinout from *your* AVR datasheet and wire all the Vcc and GND connections. use the wire for this. im big into PSU decoupling. especially the little caps for the high freq EMI right at the device. so maybe a little mylar or ceramic cap bridging the AVR from GND to Vcc. It didnt seem to matter tho, i pulled it while the AVR was running and it didnt care. stick the 10K resistor from the RST pin to Vcc (pullup R, its active low). btw, you shouldn't have had the wallwart plugged in while doing this, but i know you already knew that. wire the AVRISP interface into the appropriate ISP programming pins on the device. dont forget Vcc and GND, its how the AVRISP is powered. okay now you're done with that.


:: LiteON blue LED display driver circuit


the display comes in a 10dip pkg. 8 pins go to the 7 segments and dot point on BOTH displays. the remaining 2 pins provide a current path for their respective digit's 7 segments and dot point. so the 8 pins control what is displayed while the 2 pins control which digit its displayed on. the 8 LEDs in both digits will display an 8 bit binary downcount in parallel.


do i know how much current an AVR can push on its ouputs? nope. do i care? not really. thats what cheap transistors are for. all 10 transistor's Bases are driven thru a 1K resistor. For the 8 segment drivers, put Vcc to the Collector, and route the Emitter though a 220R resistor to its segment pin. For the digit select pin drivers, wire the pin to the Collector, then the Emitter to GND. okay youre done with your display circuit.


:: AVR Studio 4 and your new Atmel serial dongle


install AVR Studio 4 on your windows OS. if you cant install a windows program, i cant (wont) help you. screw the AVRISP serial dongle onto the butt of your PC. Connect your AVRISP to your AVR interface board and power the AVR board up. The AVRISP status LED should light red orange green then stay steady green. PAY ATTENTION, this is the tricky part. Start AVR studio. When it prompts you to update the device, say yes, because updated firmware is good, right? WRONG. but do it anyway. itll lock up the AVRISP during the firmware update, the little status LED will disappear, and AVR studio will complain no device found. neat, huh?


now, use the manual firmware update technique described in [www.avrfreaks.net](https://www.avrfreaks.net)'s Design Note #10. basically, power the thing down, pop the top off the AVRISP, short two specified holes in the PCB, power up (the LED status shouldnt be lit), wait 5sec, un-short the PCB holes, and use AVR Stduio 4 to manually program your AVRISP with the stk500 firmware. when its done, there shouldnt be errors. power the unit down. it should work with AVR studio normally now, the LED status should behave as normal next powerup.


:: the thinking part with the code


start an new project, use the AVR simulator as the debug platform (cuz the AVRISP doesnt support realtime debugging afaik), and get your code on. i adjusted the beginners tutorial here to make the count visible to the naked eye on the LED display (binary counting 8 bits a >1MHz probabaly just looks like shades of blur) by adding delay loops that do absolutely nothing. came out like this:


;blued7s\_drv.asm v0.04  

;LiteON Blue LED Dual 7-Segment Display Driver  

;(Binary Counter AVRISP Test Code)  

;rentzu 20050718 ATmega16L


.include "m16def.inc" ;ATmega16.part def 


.def Temp=R16 ;general use reg  

.def Count1=R17 ;counter stage1 reg  

.def Count2=R18 ;counter stage2 reg  

.def Count3=R19 ;counter stage3 reg


.org 0x0000 ;place next opcode at addy 0x0000


 rjmp RESET ;jump to RESET


RESET: ;device init...


 ldi Temp, 0xFF ;load 0xFF into Temp  

 out DDRB, Temp ;output Temp to PORTB data dir reg


LOOP: ;main deathloop


 out PORTB, Temp ;output Temp to PORTB (LED 7seg driver)


 ldi Count1, 0x01 ;init timekill bigloop (~.25sec)


WASTE3: ;start timekill bigloop


 ldi Count2, 0xFF ;init timekill midloop


WASTE2: ;start timekill midloop


 ldi Count3, 0xFF ;init timekill tinyloop


WASTE1: ;start timekill tinyloop


 dec Count3 ;decrement Count3  

 brne Waste1 ;jump to WASTE1 if Count3 != 0


 dec Count2 ;decrement Count2  

 brne Waste2 ;jump to WASTE2 if Count2 != 0


 dec Count1 ;decrement Count1  

 brne Waste3 ;jump to WASTE3 if Count3 != 0


 dec Temp ;decrement Temp reg (binary downcount)  

 rjmp LOOP ;restart deathloop


code that or something like that, Build and Run in AVR Studio to make sure you get no errors. your AVRISP should be connected to your AVR and your PC and the circuit should be powered up. now program the thing. when it writes, verifys, youre done with this part. power down the AVR circuit, detach the AVR from the AVRISP interface.


:: driving the display with your AVR


okay, put your LED display circuit next to your AVR circuit. route the port B outputs to the Base resistors of the segment drivers. it doesnt really matter which goes where. route the selector transistor Base resistors to Vcc (so they are always on). power the circuit up. if you did everything right, the segments of the LED should be pulsing a binary downcount in parallel. mine did. now youre done.


:: conclusion


these AVR things seem to work. pretty spiffy stuff. a microcontroller programming environment to experiment in for ~$60, a very good thing. now everyone and their mom can learn to program AVRs. also a good thing.


an atmel 1st party programming dongle broken by an atmel application during update, kinda sucky. a bit uninspiring. but it works now, i guess i learned stuff fixing it. [again shrug]

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
