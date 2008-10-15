# AtMega8535 PF Correction Relay

Uploaded by black.kad on 2008-10-15 08:15:00 (rating 0 out of 5)

## Summary

First of all I would like to request the visitor to send me a feedback through message on my AVRFreak account or at least rate it!.  

Vaguely, the Power Factor is COS of angle between Voltage and Current sine waves. The sine  

wave of voltage on the line always remains intact regardless of load connected but the Current  

sine wave shifts its place with respect to the Voltage sine wave. 


[http://www.faqs.org/docs/electri...](http://www.faqs.org/docs/electric/AC/AC_11.html) 


If the load is inductive (induction motors, electric welding, rectifiers, etc) the Current sine wave will  

start after Voltage sine wave. (If you observe on the DSO you'll find the two peaks of Voltage and  

Current sine waves aren't matching but the Current sine wave is shifted right side.)  

Vice versa happens in case of the capacitive load. Please go through the web page linked above.  

It is very informative.  

In three phase Power Factor correction system first you need to balance all three lines i.e. Load  

on each line should be equal and of same nature (inductive or capacitive). This you can say a  

disadvantage of this system. In 3ph line universally PF is sensed in standard way. One of the  

lines is considered for Current and other two are sensed for the Voltage. So connect CT (current  

transformer) in one of the phases and connect step down voltage transformer to the remaining  

phases to take samples of Current and Voltage of the 3ph line respectively. But if you are going  

to utilize this system for 1ph line then connect CT in the phase and connect step down voltage  

transformer between phase and the neutral. The HW diagram will illustrate this thing.  

So now we want to sense this angle of difference between these two sine waves. I'll give some  

vague steps here to describe the process. Again the HW diagram will explain you everything.  

Now, we have on step down sample of Current and one is Voltage. Feed these samples to two  

different Schmitt Trigger circuits. To obtain DC Square waves of the two signals. Now we need to  

detect the difference between these two square waves. Again feed these two square waves as an input to Two input one output Ex-OR gate (IC 4070) you'll get the output as per the table,  

I1 I2 Output  

0 0 0  

0 1 1  

1 0 1  

1 1 0  

As one can see the output of the gate is HIGH only when there is alternate INPUT. Now you have  

a square wave whose on time period is exactly proportional to the difference angle between the  

two sine waves. And that is what we call the Power Factor. Capture the time of this wave using  

input capture pin (Pin20) of Atmega8535. You will need to use two kinds of input capture  

interrupts. At first (program start) keep the interrupt for rising edge of input capture pulse when  

this interrupt (rising edge) occurs start the 16 bit timer(Prescale the timer such that 1 count will  

occur every 1uSec) and change the input capture interrupt type to falling edge. When again this  

second time interrupt (now for falling edge) will occur stop the counter save the count. Now this  

count is proportional to PF. Divide this value by 14H. As we are going to display PF up to two  

decimal digits only. Maximum difference value of PF is COS 90 = 0. The ON time of square wave  

output at this angle = 5000uSec (1388H counter value). We'll make a Look up Table for 255 PF  

values. So divide this 1388H by 14H to get location (FAH) of PF value COS 90 = 00 i.e. place  

00H at location FAH.  

Similarly,  

Make a Look up table in EEPROM and store the PF values for their respective Count value.  

Display this PF value in ASCII on LCD.  

I'll attach the Look Table. I have attached EEP.hex containing Look up table

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
