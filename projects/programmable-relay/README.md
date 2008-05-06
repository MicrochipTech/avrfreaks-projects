# Programmable Relay

Uploaded by eskoilola on 2008-05-05 23:33:00 (rating 0 out of 5)

## Summary

Programmable Relay  

==================


The device is intended to be used in applications which require timed  

on-off sequencing. Such applications might be sequencing a power supply  

or perhaps automating Your call sign.


The device can be programmed to operate automatically after power-up reset  

and the sequence can also be looped. The sequence can also be started by a  

simple switch and terminated by another switch.


Up to 126 short steps can be programmed. If longer time than 12.5 seconds is  

needed for a step then each long step will occupy the space of two short steps.  

The long steps can take up to 3276.5 seconds which is about 54 minutes. The  

timing resolution is 0.1 seconds. The sequence is stored in an EEPROM and  

no additional power is needed to retain the stored sequence.


The device needs a power supply 9...18 volts. There is a brownout detector  

which will put the device dormant if the voltage falls below 9 volts.  

The idle current is fairly small and most of it will be taken by the relay  

coil. There is protection against reverse polarity (diode + fuse) and power  

supply shorts. The device will tolerate up to 32 volts without being  

destructed but prolonged overvoltage will eventually fry the relay coil and/or  

blow the regulator. Immediate destruction occurs at approximately 40 volts.


The relay used can handle up to 10A @ 250VAC. However, care must be take not to  

use this in humid environment with high voltages as the insulation gap between  

the relay output and the rest of the board is only 4 millimeters.


Two switches are needed to program the device. One for switching the relay on/off  

and another to control the programming. These are named KEY and PROG  

respectively. If the device is used in automatic mode then these switches  

are not needed during normal operation.


The KEY simply switches the relay on/off. It will also terminate any sequence  

playback and restore the device in KEY follow (idle) mode. During recording  

mode the KEY is used to feed in the desired on/off sequence.


The PROG is a multifunction key which either starts playback or recording mode.  

It is also used to terminate recording and set one of the playback modes.


If the PROG is pushed (made contact) less than 1 second in KEY follow mode then  

the device will start playing back the saved sequence. Another push while  

playing the sequence will restart the sequence. The play is stopped if  

the sequence ends (not looped) or if the KEY is pushed.


If the PROG is pushed more than 1 second in KEY follow mode the device enters  

recording mode. Recording starts when You release the switch. At this point  

the state of KEY is saved as starting point. It may be open or closed. After  

this each change in KEY is recorded along with the transition times. Recording  

mode ends by pushing the PROG button.


The sequence can be saved with three attributes.  

autorun: The sequence starts automatically after power on  

loop: The sequence executes in a loop repeating itself  

endstop: The relay is left to the last state after playback


Following is a list of PROG key usage:


Mode Action Function  

=============== ================ ========================  

KEY Follow short (< 1 s) push Start Playback  

KEY Follow long (> 1 s) push Start Recording  

Playback Any push Restarts playback  

Recording 1 Push Terminates recording  

Recording 2 Pushes Terminate + autorun (on power on)  

Recording 3 Pushes Terminate + loop  

Recording 4 Pushes Terminate + endstop  

Recording 5 Pushes Terminate + autorun + loop  

Recording 6 Pushes Terminate + autorun + endstop  

Recording 7 or more Terminate + erase sequence

## Links

- [Programmable Relay @ furpile.com](http://furpile.com/Projects/programmablerelay/)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
