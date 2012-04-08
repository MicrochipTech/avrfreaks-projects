# IRmega

Uploaded by vaton4 on 2012-04-07 17:16:00 (rating 0 out of 5)

## Summary

Combined infrared receiver and baseband transmitter build using MattairTech module MT-DB-U4 (MCU ATmega32U4) and LUFA-111009 library. Device has been developped to solve two problems: 1. add the IR remote receiver to my PC without using external infrared sensor 2. make my Yamaha HTR-6230 AV Receiver controllable from PC over USB To solve both problems at once, I have build the MT-DB-U4 module into receiver and connected to the receiver circuits by simple logic adapter (74HC00 + diode + resistor + 2 capacitors). Then I have spent a lot of nice days programming ATmega firmware and Linux driver. Now my HTPC can read infrared signals from the receiver's build-in infrared sensor and fully control receiver by sending simulated infrared signals to receiver's remote control circuits, while control of my reciver using IR remote works as before, even if PC is switched off. Software consists of the LUFA-based firmware and user-space LIRC driver. Additionally, the firmware supports remote wakeup, which can be configured over USB to any remote/any button. Wakeup code reference data are stored in EEPROM. After simple modulator and infrared LED is added, the device can be used as a true infrared transmitter.


Additional info you can find on the project page [http://vaton4.web2001.cz/IRmega....](http://vaton4.web2001.cz/IRmega.html)


Follow this thread if looking for fixes and adds:


<https://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=116432>.


For MattairTech MT-DB-U4 module details see <http://www.mattairtech.com/docs/MT-DB-U4_User_Guide.pdf>

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
- Home Entertainment
