# Atmega Fusebit Doctor HVPP+HVSP

Uploaded by manekinen on 2011-02-27 15:31:00 (rating 0 out of 5)

## Summary

Atmega fusebit doctor, as name says it, device for repairing dead Atmega (and Attiny from v2.04) family AVRs by writing correct fusebits. Most common mistakes or problems are a wrong clock source (CKSEL fusebits), disabled SPI programming (SPIEN fuse) or disabled reset pin (RSTDISBL fuse). This simple and cheap circuit will fix you uC in a fraction of a second. If in first case we can help ourself with clock generator, then in 2nd and 3rd cases bring uC back to life is impossible with standard serial programmer. Most of people do not decide to build parallel programmer because its inconvenient and its cheaper and faster to buy new uC.


This circuit uses the parallel and serial high-voltage programming method. With those methods, we can talk to our â€ždeadâ€ chips which have reset or isp disabled:  

HVPP = high voltage parallel programming.  

HVSP = high voltage serial programming.


Code for this point support 138 chips, but not all have been tested. Report a problem â€“ and i make a fix :)  

1kB:  

AT90s1200, Attiny11, Attiny12, Attiny13/A, Attiny15  

2kB:  

Attiny2313/A, Attiny24/A, Attiny26, Attiny261/A, Attiny28, AT90s2333, Attiny22, Attiny25, AT90s2313, AT90s2323, AT90s2343  

4kB:  

Atmega48, Atmega48P/A, Attiny461/A, Attiny43U, Attiny4313, Attiny44/A, Attiny48, AT90s4433, AT90s4414, AT90s4434, Attiny45  

8kB:  

Atmega8515, Atmega8535, Atmega8, Atmega88, Atmega88P/A, AT90pwm1, AT90pwm2, AT90pwm2B, AT90pwm3, AT90pwm3B, AT90pwm81, AT90usb82, Attiny84, Attiny85, Attiny861/A, Attiny87, Attiny88, AT90s8515, AT90s8535  

16kB:  

Atmega16/A, Atmega16U2, Atmega16U4, Atmega16M1, Atmega161, Atmega162, Atmega163, Atmega164, Atmega164P/A, Atmega165/P/A/PA, Atmega168, Atmega168P/A, Atmega169/P/A/PA, Attiny167, AT90pwm216, AT90pwm316, AT90usb162  

32kB:  

Atmega32/A, Atmega32C1, Atmega323/A, Atmega32U2, Atmega32U4, Atmega32U6, Atmega32M1, Atmega324, Atmega324P/A, Atmega325, Atmega3250, Atmega325P, Atmega3250P, Atmega328, Atmega328P, Atmega329, Atmega3290, Atmega329P, Atmega3290P, AT90can32  

64kB:  

Atmega64/A, Atmega64C1, Atmega64M1, Atmega649, Atmega6490, Atmega640, Atmega644, Atmega644P/A, Atmega645, Atmega6450, AT90usb646, AT90usb647, AT90can64  

128kB:  

Atmega103, Atmega128/A, Atmega1280, Atmega1281, Atmega1284, Atmega1284P, AT90usb1286, AT90usb1287, AT90can128  

256kB:  

Atmega2560, Atmega2561


Just put your dead mega in socket, press the START button, and enjoy your good-as-new processor. There are three slots on board, for most common AVRâ€™s, pins compatible with: Atmega8, Atmega16, Attiny2313. There is also an extra goldpin connector with all signals so you can attach adapters:  

â€ž#1 adapterâ€ as HVPP extension, for 20pin Attiny26 compatible and 40pin Atmega8515 compatible processors.  

â€žHVSP adapterâ€ for 8pin and 14pin HVSP processors.  

Or make your own adapters for other types of processors, in trough-hole or surface-mounted, you can use the breadboard for this â€“ just connect signals to correct pins. How? Check your AVR datasheet, go to â€žmemory programmingâ€ and then â€žparallel programmingâ€ â€“ check the signal names, all signals are described under the DIP40 slot. In doctor memory there is a lot of free space so project may be developed all the time. One sided PCB with 55mm x 92mm dimensions. On top side you need to solder several jumpers, or, make this PCB as double sided â€“ choose yourself. Resistors from R7 to R23 may be in 100ohm to 10K, but i suggest from 470ohm to 1K. You can find extra RS232 output, connecting to this (at 4800 bps) we will receive all information about fixing process â€“ see exemplary printscreen in gallery. Of course terminal is not needed, all we want to know we get from leds.


ATTENTION! While mounting the DIP40 slot, you must to remove it pins from 29 to 37. These pins must not have electrical contact with inserted uC pins.


Leds explanation:


green on â€“ patient successfully cured, fusebits repaired. If lockbits are enabled, just verify fusebits with factory ones â€“ and if they ok â€“ light up green.  

red on â€“ signature problem, canâ€™t read, no device in socket, or no such signature in database.  

green flashing â€“ signature ok, fusebits are wrong. Lockbits enabled, chip erase permission required (read below).  

red flashing â€“ signature ok, no lockbits, but for some reason canâ€™t write new fusebits.


The ALLOW ERASE jumper allows doctor to erase whole flash and eeprom memory, if it is open, doctor will newer erase memory but may not cure device if lockbits are enabled, so you choose. After insert dead uC and press the START button, doctor will initiate the parallel or serial high-voltage programming mode. This is chosen automatically, device will recognize HVSP adapter and start to work in HVSP mode. After that, doctor wait for high state at RDY/BSY line. Then, read device signature and check if it supports it. Next, memory erase is performed if user allows that. Then lockbits are checked, and if they not blocking device, doctor sets all fusebits to fabric, having regard to whether there are extended fusebits or not. Some of older AVR have only one byte of fuses â€“ LOW â€“ and this is also included. After that fusebits are verified, and proper leds are flashed. All the info is send â€žon runâ€ via uart.


Code was written based on high-voltage parallel and serial programming section of datasheet of suitable AVRs.  

If you are searching for attiny family AVRâ€™s fusebits fix device, then check my previous project, the Attiny fusebit doctor. Unfortunately, some bugs show off and this project is no longer updated since the â€žAtmega fusebit doctorâ€ supports Attiny family AVRs. But, source code is attached for this project, and with avr datasheet, it will be easy to understand this programming mechanism. 


Fusebits: Internal 1MHz clock, and enabled EESAVE bit â€“ L:0xE1 H:0xD1  

Use stabilized 12V supply voltage. Higher voltage can damage fixed chip!  

If you use a brand new chips as doctor, you donâ€™t need to change anything â€“ 1MHz clock is already set as default. EESAVE bit is optional.  

You can use the Atmega8L as Atmega8 replacement.


For even more info and details pleas check project site: [http://diy.elektroda.eu/atmega-f...](http://diy.elektroda.eu/atmega-fusebit-doctor-hvpp)


For further updates: twitter.com/manekinen

## Compilers

- BASCOM-AVR

## Tags

- Complete code with HW
- BASCOM-AVR
