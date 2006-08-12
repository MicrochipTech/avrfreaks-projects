# 3 position rotary switch emulator

Uploaded by Timos_t on 2006-08-12 14:47:00 (rating 0 out of 5)

## Summary

Project:0037  

Purpose:Emulates a 3 position rotary switch  

Status :Working  

Level :Newbie  

I/O :5 


Hello AVR freaks


Well , a little but handy this project will be, (i hope).


The ugly switch...  

This project emulates the function of a 3 position rotary switch with 1 common pole.  

Imagine 3 STATUS leds (anodes) connected with limiting resistors to each switch end pole  

and (cathodes) to the GROUND, and the common of the switch to +5V.


Now the AVR ...  

It uses 3 push buttons instead, and 3 status leds.The two 1N4148 diodes are used to virtually add 1 more  

switch, but using only 2 lines of the AVR uC. That means 2 inputs(PB3,4), 3 buttons, 3 possible program  

excecution choices!!.  

Any feedback or questions/suggestions for this little project , would be great for me.  

Good "coding nights" to all,


Sincerely Tim


Extra: I think this little project is not a crap for experts, but very handy for newbies.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
