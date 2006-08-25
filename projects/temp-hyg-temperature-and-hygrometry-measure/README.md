# Temp-Hyg: temperature and hygrometry measure

Uploaded by wibauxl on 2006-08-25 15:18:00 (rating 0 out of 5)

## Summary

Functions  

---------  

An event based temperature and hygrometry application.  

Logs temperature and hygrometry and draws day (last 24 hour measures), month (last 31 day measures), year (last 12 month measure) and full years as graphs -> see screen.xls in the archive.  

Has also got a home screen with instant, max and min values for temperature and hygrometry; and a screen to setup date and time.


Hardware  

--------  

Loosely based on a stripboard (can be seen back and front in the temp-hyg.mdi). The image was created with stripboarder (also posted on this forum).  

Has been working for more than 2 month.  

Uses SENSIRION SHTX to get the temperature and hygrometry values.  

Used a DMX973B 100x32 graphical display (but that can be changed in the screen.c and dmx973.c.)  

Uses a AtMega168 for the full application (graphs of logged values, home and date screens) or a AtMega8 for the simplified one (home and date screens only). 


Software  

--------  

Uses a modular approach.  

Each module posts events that are managed by the event.c file. e.g.: enter presses create EVENT\_BUTTON\_ENTER, sensor mesures create SENSOR\_EVENT.  

Those events are then handled by event\_handlers. e.g.: an EVENT\_SENSOR creates a screen refresh and a logging operation in the eeprom.  

The modules are completely independant and therefore change of hardware is pretty simple.


Enjoy!


Laurent Wibaux  

email: please post on the forum

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
