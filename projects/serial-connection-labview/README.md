# Serial Connection With Labview

Uploaded by lilpacbe on 2008-06-29 12:48:00 (rating 0 out of 5)

## Summary

This project enable's you to read 2 analog value's into National Instrument s Labview (Dataacquisition Program: <http://www.ni.com/labview/>).  

With little adjustments to the labview program you can measure all the adc's of youre µC.


Little information about how i did it:  

My analog value's are constant beeing send to labview via the UART.  

Adc0 & Adc1.  

This are hexadecimal values that are beeing send one by one (whole the time).  

example:  

Adc0=0xFF  

Adc1=0x00  

so my data is: µC > ...FF00FF00FF00FF00 > Labview


With labview i read 4bytes and mask the 2 first bytes to show adc1 and mask byte 3&4 to show adc0.


It took me 2days to complete this project.  

I had never used the uart and wasn't that familiar with NI Labview.  

The only thing i could not get done is to controll the microcontroller via labview. So i decided to share this project with others:  

1. Maybe i could help others that are interested in µC & Labview.  

 To Log sensor data etc...  

2. Maybe someone can find a way to controll the microcontroller via labview.  

So IF someone can get this done, please let me know.

## Links

- [entire project](http://sozen.dommel.be/Daqkaart.rar)

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
