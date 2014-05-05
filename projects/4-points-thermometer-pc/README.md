# 4 points thermometer with PC

Uploaded by laurentppol on 2014-05-04 21:24:00 (rating 0 out of 5)

## Summary

Hi, there, this is not a "masterpiece" but hardware is stable, software too, and is working as expected. Still have to add functions. 4 points thermometer, using DS18B20, LCD 4x16 (sold in Poland by Artronic\_PL on Allegro - PCB is made for this one), connection to PC using USB -> 5V serial converter (PL2303). It is intended to display 4 temperatures (column filling 40cm above boiler, head, liquid in boiler) of Polish (?) "AAbratek" home alcohol rectification system. So what's the difference? - main: LCD AND PC connection, as I've seen many thermometers, but with LCD OR serial / USB connection. - Designed by a person that operates AAbratek himself, with ease of (column) operation in mind. (I hope) it is K.I.S.S. (Keep It Simple, Stupid), at least from operator's point of view. - PC connection, displaying always "raw" temperatures (useful for graphing), - "delta T" mode for 2 channels (column filling and head), with 2 distinct alarms, - it (still) can display 4 lines on 2 line display (H or V scroll), - schematics / PCB under Creative Commons By Attribution Share Alike, - code under GPL 2+. What is to be done (hardware will support): - timers (using rotary encoder for convenience), - conversion of liquid temperature into liquid percentage (in column "stable" state). - nice graphic chars for degrees Centigrade. Please feel free to contact me.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
