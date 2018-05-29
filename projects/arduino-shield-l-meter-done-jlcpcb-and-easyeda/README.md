# Arduino shield L-meter done in JLCPCB and EasyEDA

Uploaded by wegi on 2018-05-28 21:52:00 (rating 5 out of 5)

## Summary

 


any movies:


 


<https://www.youtube.com/watch?v=RK2iNt9aHoI&feature=youtu.be>


 


![](https://community.atmel.com/sites/default/files/project_files/04cover.jpg)


 


Really easy work in **EasyEDA**, which have a complex service from making project ( <https://easyeda.com> ), to ordering high quality PCB ( <https://jlcpcb.com> ) - **JLCPCB** - even to components order ( <http://lcsc.com> ) - **LCSC** -, which can be connected with PCB order package. From order PCB date we can expecting about 7 days to delivery by DHL Experss at our home. 


 


So I started this project from PCB order in **JLCPCB** and after 9 days I had a quite finished Arduino shield L meter project.


 


What I can say - **I'm really glad from 48h building time  and very high quality of PCB.**


 


![](https://community.atmel.com/sites/default/files/project_files/01TOP.jpg)


 


 


![](https://community.atmel.com/sites/default/files/project_files/02BOTTOM.jpg)


 


 


after soldering


 


![](https://community.atmel.com/sites/default/files/project_files/03MOUNTED.jpg)


 


and connected to Arduino UNO R3


 


![](https://community.atmel.com/sites/default/files/project_files/05manifest.jpg)


 


 


 


This is the adaptation for Arduino UNO R3 of **Adrzej Janeczek project** “**AVT-2813**” - Multimeter addition for L measurement.


 


 


<https://community.atmel.com/sites/default/files/project_files/AVT2813.pdf>


 


 


 


**Small and cheap shield for Arduino**


 


Now we can think, how we can measure the inductance of coil…


Can we for example counting a magnetic fields lines around the coil ? – Of course NO :-) BUT… but people think and think and created indirectly methods for this…


So we can connect the coil into the circuit and we can check how she (IT) work. How fast she (IT) get the saturation and de-saturation. Simple scheme first:


 


![](https://community.atmel.com/sites/default/files/project_files/06scheme%201.png)


 


 


This scheme is a bit low readable, we can look at this from easy side like this:


 


![](https://community.atmel.com/sites/default/files/project_files/07L%20meter%20small.png)


 


On the first gate we gave rectangular signal (about 128kHZ) from R1/C1 oscillator, which is separated by 3 next other gates. Now signal going to R3/R4/R5/Lx and we have the SAW signal, which going on the next gate. From this gate signal is converted to rectangular signal, where the fill factor is proportional to coil inductance. Last gate reversing this signal phase and send signal to R6/C3 integral circuit. **This circuit get the average voltage level of this rectangular signal, and this is the value of inductance where any uH is after every one 10 mV (1uH/10mV).**


Finally we using Arduino as V-meter, and additionally as F-meter for adjust signal from R1/C1 to 128 kHZ. For this we have one tactile switch, which we should pressing by about 1sec. and we can see something like this and using PR1 rotation resistor (this long one):


 


 


![](https://community.atmel.com/sites/default/files/project_files/08F-METER.jpg)


 


 


Theory we can check on the oscilloscope diagrams:


 


![](https://community.atmel.com/sites/default/files/project_files/09diagrams.png)


And now Lx = 40uH, the fill factor is going up:


 


![](https://community.atmel.com/sites/default/files/project_files/10logic%20level3%2040uh.jpg)


 


 


We can look how logic levels gate work:


 


![](https://community.atmel.com/sites/default/files/project_files/11logic%20level4%2040uh.jpg)


 


 


This simple way gave possibility for us how catch the coil inductance.


 


Here you can get firmware for arduino hex and asm file 


 


<https://community.atmel.com/sites/default/files/project_files/V_328.ZIP>


 


 


 


 


Enjoy


wegi

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
