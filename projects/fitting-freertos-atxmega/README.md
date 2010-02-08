# fitting freeRTOS to ATxmega

Uploaded by hy6r0t3d on 2010-02-08 07:18:00 (rating 0 out of 5)

## Summary

Just this small mod, if somebody is interested in. :)


*10/02/08  

CORRECTION: v100205  

~~~~~~~~~~~~~~~~~~~~  

Due to some difficulties with Xmegas providing large program memory (>0x00FFFF) I've corrected my modification.


Till now there will be a problem, if a task function is stored in higher program memory sections (task parts behind 0x00FFFF).  

But I'm working on it. Just for now try to store your task functions in lower sections.  

Thats all, only the task functions are affected. Functions called by the task functions can be stored where ever is enough memory....


*10/01/05  

CORRECTION: v100104  

~~~~~~~~~~~~~~~~~~~~  

The ticking interrupt priority is set to low at common. In the version v100104 from Jan 4, 2010 it was set to high.  

Sorry, I've forgot to reset it from high (extreme testing purposes).


If you're using, for example, TWI interrupts in lower priority the ticking interrupt might disrupt the transmission interrupt in an unfavourable moment. Which will result in transmission errors.


-> secure the critical transmission sections from interrupts or use a higher priority!


*10/01/04  

UPDATE: v100104  

~~~~~~~~~~~~~~~~~  

* MOD all interrupt priorities of the new ATxmega devices usable without restrictions.


!!I wish you all a happy new year!!


*09/12/10  

ROOT: v091209  

~~~~~~~~~~  

* MOD ticking timer modification to new structure of xmega timer/counter "C0"  

* MOD portSTACK\_TYPE *pxPortInitialiseStackt(..) to handle 3 byte program addresses  

 -> REQUIREMENTS:  

 - completely compatible with freeRTOS structure and principle.  

 - fit to folder structure of freeRTOS/Source/..


(these modifications resulted by some issues I had with getting freeRTOS run at an ATxmega256A3.  

-> For further information you can visit my old thread concerning the problems:  

[https://www.avrfreaks.net/index.p...](https://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=87163)  

)


I'm glad about your feedback, suggestions and what else is in your mind concerning this work!

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
