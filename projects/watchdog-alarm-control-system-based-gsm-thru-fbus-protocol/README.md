# WatchDog Alarm & Control System based on GSM thru FBUS protocol

Uploaded by nppc on 2005-08-29 04:51:00 (rating 0 out of 5)

## Summary

This System works on AT90S2313 and can receive SMS messages for: On/Off WatchDog, control 3 erlays (10A) and for sending Info message about system state.  

It's tested with NOKIA 5110/6110 and 3310 thru FBUS. Fixed commands to send (total 11): "kute on 1", "kute on 2", "kute on 3", "kute on 12", "kute on 13", "kute on 23", "kute off", "watchdog on", "watchdog off", "info". Commands "kute..." swithes 3 relays sepparattely, commands "watchdog..." used for arm/disarm alarm system, and command "info" get information about system state (relays and watchdog state).  

Also system sends some messages to GSM: "Power!" message if system is armed, and main power switched off; "Alien!" if recives signal from 1st group of sensors and "Burning!" if signal from 2nd group of sensors (basicaly move sensors and smoke sensors). Sensors can give logic value from 4 to 12 volts. Also provided power for sensors (9 and 12 volts).  

If system armed and some of sensors goes active, then siren driven by 500ma transistor going on. (signals from second group of sensors switch siren on also in disarmed mode. System have an arm/disarm button and control LEDs.  

Code not well documented, but your can send any questions to author. Also schematic is not good enaf, because this is my one of the first projects with hardware. And if you have some comments on hardware design, then you are welcome. May be later I write some good explanation for my project.  

I forgot to mension about crystal speed. Speed is 1.8432 Mhz.


My E-mail: [nppc@hot.ee](mailto:nppc@hot.ee)

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
