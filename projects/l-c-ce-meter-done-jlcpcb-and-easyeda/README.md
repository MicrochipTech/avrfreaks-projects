# L-C-CE METER done in JLCPCB and EASYEDA

Uploaded by wegi on 2019-03-04 23:04:00 (rating 0 out of 5)

## Summary

![](https://community.atmel.com/sites/default/files/project_files/_0000%20assembled.jpeg)![](https://community.atmel.com/sites/default/files/project_files/000%20SCHEME.png)


 


**THANKS TO JLCPCB**


 


![](https://community.atmel.com/sites/default/files/project_files/001%20PCB.jpeg)


 


![](https://community.atmel.com/sites/default/files/project_files/002%20METER.jpeg)


 


**L/C METER**


 


**Theory**


A capacitance + inductance they form a resonant circuit. He is vibrating. The frequency of vibrations depends on the capacity and inductance. When we add to the resonant LC circuit, the coil **(in series to the coil)** - or the capacitor **(parallel to the capacitor),** then the resonant frequency will decrease.


 


**Practice (resonant method)**


 


L/C circuit is cooperates with the LM393D comparator, from which the output is fed back loop to the LC circuit. Thanks to this, the vibrations do not expire. The comparator output also gives a rectangular signal to the input pin of the microcontroller. This signal has the resonance frequency of the LC circuit. The frequency of this signal is measured by the microcontroller.


            Knowing the inductance values and capacities of the LC generator based on the frequency difference, we can calculate the value of the attached capacitance or inductance to the LC circuit.


 


Used formulas:


 


![](https://community.atmel.com/sites/default/files/project_files/003%20formula%20cown.png)


 


 


Where:


 


***Cown*** – capacitance of LC circuit [pF] 


***Cref***   - a referential knowing value of capacitance which was added to LC circuit (generator) [pF]


***F1*** – a resonant frequency of LC circuit **before** add ***Cref*** capacitor [Hz]


***F2*** – a resonant frequency of LC circuit **after** add ***Cref*** capacitor [Hz]


 


 


 


![](https://community.atmel.com/sites/default/files/project_files/004%20formula%20Lown.png)


 


 


 


Where:


 


***Lown*** – a own inductance of LC circuit (**without** added any other inductor)  [uH]


***Cown*** – a own capacitance of LC circuit (**without** added any other capacitor) [uF]


***F1*** – a frequency of resonant LC circuit [Hz]


 


 


 


![](https://community.atmel.com/sites/default/files/project_files/005%20formula%20cx.png)


 


 


 


Where:


 


***Cx***  - is a unknown value of capacitance which was added to LC circuit (generator) [pF]


***Cown*** – a own capacitance of LC circuit [pF] 


***F1*** – a resonant frequency of LC circuit **before** add ***Cx*** capacitor [Hz]


***F2*** – a resonant frequency of LC circuit **after** add ***Cx*** capacitor [Hz]


 


 


![](https://community.atmel.com/sites/default/files/project_files/006%20formulaLx.png)


 


 


 


 


 


Where:


 


***Lx***  - is a unknown value of inductance which was added to LC circuit (generator) [uH]


***Lown*** – a own inductance of LC circuit [uH] 


***F1*** – a resonant frequency of LC circuit **before** add ***Lx*** inductor [Hz]


***F2*** – a resonant frequency of LC circuit **after** add ***Lx*** inductor [Hz]


 


 


**MEASURE FOR BIGGER CAPACITORS - ELECTROLYTIC**


 


**Charging time to capacitance by constant voltage.**


 


            When we charging capacitor by constant voltage we can observed a curve of charge:


 


 


![](https://community.atmel.com/sites/default/files/project_files/007%20exponent.png)


 


 


 


We can see the charging time if depend from resistance and capacitance. And these two values form a time constant called tau **τ**  which units is the seconds [s]


So from above diagram we can get the known:


After one **τ**  the voltage on the capacitor get the 0.632 E value


So if our E have 5V, after the **τ**  time on the capacitor voltage will present 3.16V.


**AND FINALLY – when we charge the capacitor up to 0.632E, knowing the value of R and measuring the charging time, we can very easily calculate the capacity of the capacitor.**


 


**![](https://community.atmel.com/sites/default/files/project_files/008%20heureca.png)**


 


If we look at the diagram, notice that the capacitor can be charged by one or two parallel resistors RES1 and RES2.


 


Therefore, the program uses one more formula:


 


 


 


![](https://community.atmel.com/sites/default/files/project_files/009%20res3.png)


 


 


 


WHAT YOU MUST DO TO GET A HIGH ACCURACY OF MEASUREMENT.


 


Start with RES1 and RES3 soldering, and then measure and store their values. After assembling the whole and connecting the power supply, the VCC supply value should be measured, then the PR1 potentiometer should be set to 0.632 VCC at TP (arround 3.16V).


 


 


**The FIRMWARE**


Wrote in assembler in two options – for **ATmega8A or ATmega328P** – why 328P? Cause I got a few unused Arduino J so I don’t need to purchase M8A J


 


After power on we can see three screens:


 


![](https://community.atmel.com/sites/default/files/project_files/010%20AFTER%20RESET.PNG)


 


 


 


Honestly, after the first power on third screen going to calibrate menu, which I’ll describe later.


 


After reset we can put capacitor into the socket and measure capacitance.


After pressed the left button we can see the L measure screen or the second one after put inductor **into the left socket.**


 


 


![](https://community.atmel.com/sites/default/files/project_files/011%20L%20measure.png)


 


 


 


 


Next one press the left button invoke the third menu for CE measure:


In the **right** **socket** we can do measure of electrolytic capacitors.


 


![](https://community.atmel.com/sites/default/files/project_files/012%20CE%20measure.png)


 


 


 


After this, the left button go back to small capacitors measure menu.


 


 


 


How the program calculate capacitance:


 


1. Simply start charge of the capacitor and counting the clocks edges.


 


2. After charging capacitor program multiply by 125 value of counted clock edges, cause crystal have 8MHZ for translate to decimal relation to seconds. And divide by 10.


 


3. After that value is divided by RES2*100 (for better precision) or by RES3 (two parallely connected resistors RES3=RES1*RES2/[RES1+RES2] ) – result in [uF] or in 0.01 [uF]


 


 


 


Al the time by press right button we can invoke 7 specials menu. In this mode the right button select menu, and after the select any one by pressing left button execute it and go inside.


 


 


 


![](https://community.atmel.com/sites/default/files/project_files/013%20menus.png)


 


 


 


**MENU COMPENSATE:**


 


**![](https://community.atmel.com/sites/default/files/project_files/014%20MENU%20COMPENSATE.png)**


 


 


After select his menu program compensate the frequency value, which can be a bit floating depend from thermical values. And go back to last used menu of measure.


 


 


 


**MENU RESET:**


 


![](https://community.atmel.com/sites/default/files/project_files/015%20MENU%20RESET.png)


 


 


Simply restart of firmware, and lose the compensate value.


 


 


 


**MENU CALIBRATE:**


 


![](https://community.atmel.com/sites/default/files/project_files/016%20MENU%20CALIBRATE.png)


 


 


Most probably don’t need describe this menu, everything is on the following step by step screens. 


**Note: You can choose in the next menu the reference value of the calibrate capacitor, which you’ll use.**


 


**MENU SET  CREF VALUE – you can change the referential value your capacitor for calibrate if you need.**


 


![](https://community.atmel.com/sites/default/files/project_files/017%20MENU%20CREF.png)


 


 


 


**MENU SET COWN VALUE:**


You can change the value of capacitor in LC generator circuit, after this the **coil value will be recalculated.**


**In the last 4 menu you can set the value by pressing left button and right button for confirm.**


 


![](https://community.atmel.com/sites/default/files/project_files/018%20MENU%20COWN.png)


 


 


 


**MENU SET RES1 VALUE:**


Here you should set the RES1 value, which you measured after soldered RES1.


 


![](https://community.atmel.com/sites/default/files/project_files/019%20MENU%20RES1.png)


 


 


 


 


**MENU SET RES2 VALUE:**


Here you should set the RES2 value, which you measured after soldered RES2.


 


![](https://community.atmel.com/sites/default/files/project_files/020%20MENU%20RES2.png)


 


 


 


**A bit unrolling the second one formula for clarity and understand how to do it on the integers numbers.**


 


 


![](https://community.atmel.com/sites/default/files/project_files/021%20formula%20Lown%20constant.png)


 


 


 


 


We got here a constant 1/4PI^2, also we dividing by [pF] so we can do sth like that:


 


100 000 000 000 000 000 000/4/Pi/Pi = 2 533 029 591 058 444 286 


 


And now:


2 533 029 591 058 444 286 /F1/F1/C


 


**BECAUSE the 2 533 029 591 058 444 286 value is oversized by a few decades the result is in  0.01 [uH] what gave resolution in 10 [nH].**


 


Just for example how program calculate Cx or Lx and don’t going to to lower values using integers:


 


Cx = (Cown * [(F1*F1)-(F2*F2)])/F2/F2


 


So the first result = (Cown * [(F1*F1)-(F2*F2)]) is **huge enough** to divide by square of F2…


 


All in the attached source code please J


 


**KNOWN PROBLEMS:**


 


C9 10uF doesn’t work properly, breaking oscillations – solution – replaced to unpolarized SMD


LM393D damaged under soldering probably – breaking oscillations – replaced to new.


 


 


 


**REFERENCES:**


 


[https://easyeda.com/wegi1/KM409-...](https://easyeda.com/wegi1/KM409-LC-meter)


 


[ALL PROJECT](https://easyeda.com/wegi1/KM409-LC-meter)


 


**FIRMWARE SOURCE**


 


[SOURCE](https://community.atmel.com/sites/default/files/project_files/FIRMWARE%20SOURCE.ZIP)


 


**BINARIES**


 


[ATMEGA8A BIN](https://community.atmel.com/sites/default/files/project_files/KM409%20M8A.bin)


 


[ATMEGA 328P BIN](https://community.atmel.com/sites/default/files/project_files/KM409%20M328P.bin)


 


 


**ALL OF THIS IN PDF FILE**


 


[PDF DESCRIBE](https://community.atmel.com/sites/default/files/project_files/DESCRIBE.pdf)

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
- Electricity Meters
