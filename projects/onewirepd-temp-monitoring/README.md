# OneWirePD, Temp Monitoring

Uploaded by tirpitz on 2002-11-11 22:10:00 (rating 0 out of 5)

## Summary

Connect up to 20 DS1820 to Mega16 on the same pin. Output temp(s). at serialport.:  

01: 100008001EDF2110 23.0 C  

00: 290008001EE05C10 22.9 C


The low level routine is written for 1MHz Int RC  

This is the tempm example from owpd300b2\apps\temp, ported to AVR.(slightly modified)  

which can be found at : [http://www.ibutton.com/software/...](http://www.ibutton.com/software/1wire/wirekit.html)  

AVR: Connect a pullup resistor 1K between Vcc and DQ-pin. (portA.7)  

DS1820's: Connect Vcc to Gnd. (Parasitic power mode) and DQ to AVR

## Compilers

- IAR Embedded Workbench for AVR

## Tags

- Complete code
- IAR Embedded Workbench for AVR
