# home automation - 6LowPAN

Uploaded by inooi on 2011-06-26 15:36:00 (rating 0 out of 5)

## Summary

-------------- HOME AUTOMATION ---------------  

==== O W B O X ====


- 6LowPAN wireless sensors based on ATmega1281 or ATmega1284P  

- reduce and optimize the cost of the energy  

- small autonomous devices  

- NEW WEB SITE <http://www.owbox.com/> (french only)


The system is composed of several wireless modules (idealy 1 per room + special modules). Each module can have several sensors (1-wire or other) and can drive several outputs (1-wire or static relais).


All hardware modules are composed of several software modules. Some of them are sensors depending of what is connected(AC current, temperature, lightning, open/close button, ...) and some of them are controller modules. Those controller modules can receive informations from all other sensors in the network and can control several outputs.  

To avoid complex bugs, we limit the output of controllers to outputs directly connected to the hardware module.


It should not be posssible in this network to ask for a remote control. All software modules are master of their outputs and decide what they have to do by themselves. This mean that all modules are intelligent enought.


Hardware : Zigbit modules (newly manufactured by atmel)


MS-402 : PIR motion detector from FUJI & CO


Software (NEW): contiki with own application based on UDP broadcast


The first wired release is working fine and can be seen at <http://www.owbox.com>


Status of the project : Wireless network is running. Some features are already developped like AC consumption, temperature, humidity, motion monitoring, optimized power plug, light optimized system, AC consumption optimizer, ...


Thanks,  

Frederic

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
