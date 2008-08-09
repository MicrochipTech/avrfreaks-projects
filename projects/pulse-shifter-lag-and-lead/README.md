# Pulse shifter (lag and lead)

Uploaded by hendyekahardana on 2008-08-09 02:15:00 (rating 0 out of 5)

## Summary

To accomplish a part of request from some friend in their final assignment in my campus (automotive applications), i had made several lines of CodevisionAVR Code. In here an Atmel AVR ATmega8535 used as implementation. As principal, this chip was used to detect pulse logic signal (INT0), measure it's length (thigh and tlow) with Timer1 (16 bit) and then give an identical shape output (in a normal condition) or a lag pulse or a lead pulse (in a special condition as what we want).


For example was ilustrated below:


Input pulse : \_\_|--|\_\_|--|\_\_


Output pulse :  

 \_\_|--|\_\_|--|\_\_ (normal condition)


 \_|--|\_\_|--|\_\_| (lead)


 \_\_\_|--|\_\_|--|\_ (lag)


Depend on their request, this source code is completed also with 16x2 LCD Interface to display rpm (coz it measure rpm too) and system's mode. Completed also with two push button to control the shifting process. Maybe it can be useful for you as a reference when you need a controlled switching system using Atmel AVR microcontroller.


Sorry for my broken English, regards.


[*Update info: December 15th 2007  

There is a bug in the signal measuring schedule timing. When it is measuring, the output signal is discarded (persist it's logic). So there always a "annoying" condition every specific time in the software (discarded pulse input shape). I will fixing this soon....]


[*Update info: January 16th 2008  

Revised version. But still use no input capture unit, just external interrupt 0. More consistency than before of course. :) I will fixing this soon....]


[*Final info: February 5th 2008  

Final version. I had tried with capture unit, but the result was inconvenience and dissapointed. Now for the last tries I use both of the ext interrupt mode. Rise and fall. And now, this algorithm can get the max. accuration for 15 us shift. Shifting process is min 15 us or its factor. Still make me unsatisfiy but i think i had stopped for this one. Maybe I can get more in using > 20 MHz system clock. I glad to see for everybody may can get more in this....  

Thanks for attention, regards.]


[Bug fixed: August 9th 2008  

Recently known there is a mistake in the variable type of waktu1, the correct one is unsigned long int instead of unsigned int. This affection seems to the signal measuring schedule timing. The cycle is less than 5 seconds, so now it had become truly 5 seconds. In addition, if you want the better performance just select an avr with dual timer 16 bit. Thank you, regards. I also upload the correction one.]

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code
- CodeVisionAVR-Standard
