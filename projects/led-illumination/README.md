# LED illumination

Uploaded by VDierkes on 2003-02-06 21:14:00 (rating 0 out of 5)

## Summary

Funny AVR demonstration. Controls 8 LEDs which are directly  

connected to VCC. Additional to the nice illumination this  

program demonstrates the usage of the async timer and the  

sleep modes to save power.


The port pins for the LEDs are in a way selected, that it is  

easy to put these around the chip. If you want to change the  

port pins (maybe for another mcu), simply change it in the file  

hardware.h. It doesn't matter which port pins you select. You  

can control the brightness by changing the values in the section  

Timings in globals.h.  

Additional to the LEDs you can connect 2 keys to the mcu. The  

function of the keys is either to select the mode of illumination  

and to switch it on and off. If you hold the mode-key for about  

1,5 seconds an auto mode will be selected. In this mode all  

illumination effects are automatic periodically selected.  

The shutdown is realised through the power down mode of the mcu.  

While the program is running, the power save mode is used to  

reduce the power assumption.


The build\_*.* files are usefull for calculating build numbers,  

but it is written for win2k. I didn't try it on other win os's.  

And it didn't work on other OS I assume.


To the hardware:  

Connect the LEDs to the pins which are defined in hardware.h, the  

same for the keys. No external pull-ups are required. But you need  

a 32.768KHz oscillator on the TOSC1 and TOSC2 pins.


Compiled with WinAVR January 2003 distribution. You need avr-sizex  

 too. You find it here:  

[http://8515.avrfreaks.net/phorum...](http://8515.avrfreaks.net/phorum/read.php?f=2&i=4978&t=4909#4978)  

 Otherwise you can delete the sizex usage in the makefile.


Inlcuded are the schematic and an example for a board (not verified ;-)


Have fun  

Volkmar

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
