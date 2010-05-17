# NIXIE clock with many features

Uploaded by horace on 2010-05-17 06:45:00 (rating 5 out of 5)

## Summary

Features


-Displays time ;-) with seconds (if 6-Tubes mounted)


-User selectable 12/24 Hour mode with AM/PM indicator (on at AM or PM)


-Blinking colons with adjustable duration


-Selectable date display (from second...to second of every minute)


-Adjustable date format (DDMMYY, MMDDYY or YYMMDD)


-Selectable blanking of leading zeros


-Notice to leap years


-Automatic daylight saving time correction (configurable)


-Selectable digit cross fading for the NIXIEs


-Selectable â€žSlot machineâ€œ every n minutes to prevent cathode-poisoning


-â€œNight modeâ€œ â€“ NIXIEs (and LEDs) can be switched off for a selectable time 


-Alarm clock 


-NIXIEs can be dimmed by key-press: full+fading, full, half, off) 


-NIXIEs can be switched on by key-press during â€žNight modeâ€œ (slot machine and date display are suppressed)


-NIXIEs are switched on during alarm in â€žNight modeâ€œ (for a selectable time) 


-All settings are saved in the EEPROM and will be kept in case of disconnection to mains


-Alarm output is configurable to connect a speaker (output of alarm sound) or an external device (output is switched from low to high for a sound module or relays ...)


-Connector for a PIR/LDR/switch..) - triggering the display for a few (selectable) minutes


-Clock is synchronous to mains frequency or any other external clock source (configurable from 1Hz up to 64Hz).


-Connector for power failure backup (battery or accu) â€“ the clock will run with the internal oscillator (may be calibrated)


All options (start/end times, intervals etc.) can be configured with 2 switches (the third is for the alarm handling) during operation.


Hardware:


-Minimalistic circuit, cheap Attiny2313


-Many types of NIXIEs usable


-Uses a 9...12V AC wall wart â€“ circuit is not directly connected to mains!


-Software controlled PWM (high voltage generation) â€“ only a few parts necessary


-Because of the multiplexing of all 6 tubes , only one 74141 NIXIE driver (you can get it from EBAY or from several internet sellers) is needed


-All functions are controlled with 3 keys, all options can be configured by editing a parameter list during operation


-If you omit key 3, all alarm functions are hidden and you get a â€žnormalâ€œ NIXIE clock

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
