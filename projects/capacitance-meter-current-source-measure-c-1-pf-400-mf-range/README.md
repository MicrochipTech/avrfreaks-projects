# Capacitance meter with a current source to measure C from 1 pF to 400+ mF range

Uploaded by wegi on 2016-04-15 17:11:00 (rating 5 out of 5)

## Summary

Hi ppl - simple led flashing bored me. I did this one C meter.


 


 


![](https://community.atmel.com/sites/default/files/project_files/Cruel_C_meter.jpg)


 


 


PCB project below: 


 


[https://easyeda.com/wegi1/C\_mete...](https://easyeda.com/wegi1/C_meter_ATtiny2313-uLzx7tdaU)


 


 


Source code


 


[https://bitbucket.org/wegi1/wegi...](https://bitbucket.org/wegi1/wegi_cruel_c_meter_attiny2313/src/dce1a9059790d2597c27958d4482f1b00d0e8cc8/C_meter.asm?fileviewer=file-view-default)


 


 


little movies


 


[https://www.youtube.com/watch?v=...](https://www.youtube.com/watch?v=98XM_6EsB6E)


 


 


**Capacitance meter with a current source to measure C from 1 pF to 400+ mF range**


 


 


At the beginning of a huge thank to you for the Dillon from EasyEDA Team for helping to make this documentation. Without him, this description could not have been made.


Thank you very much Dillon. I'm owe You.


 


 


![](https://community.atmel.com/sites/default/files/project_files/C-meter_scheme.png)


 


 


**Features:**


This scheme is the result of many of my thoughts. For me, its beauty lies in its simplicity and effectiveness.


The range of measured capacity from 1pF to over than 400 mF - However, be careful - such a measure as 400 mF would last over 500 seconds.


Power supply AC or DC in the range of 9-12V - Yes. I specially added a diode bridge, not mistaken poles. Power supply is possible through the DC jack and installed a second connector for direct connection cables (just in case).


Automatic switching range.


Presentation of the measurement on the LCD display.


Can be programmed in the circuit ISP. Pins are led out to the connector LCD display and also reset by the 100 ohm resistor.


 


 


**How is it working (electrical)**


 


I looked through such projects and noticed that often cooperates with the microcontroller 555. Here it functions as a whole taking a microcontroller.


 


**Power block** is simple. Used the bridge of diodes and stabilizer 7805. It is strongly recommended the installation of the heatsink for 7805. Filtering the supply voltage VCC is standard (C3, C4,C5,C6 - 100 nF and uF in pairs). Also standard processor has connected 100 nF capacitor on VCC (C7).


Also as in catalog is connected to the **oscillator circuit**. No any comment we need here. **Reset** is led to power by the 10K resistor. In addition, it is outputted to the microswitch by 1k resistor, as well it is led to pin header by the 100 ohm resistor for ISP possiblity.


 


**The brain** all of them is microcontroller ATtiny2313.


 


**interface functions**


PR5, PR6 - to determine the contrast of the display.


LCD connection:  

PORTB.4,5,6,7 as data (btw. here is MOSI, MISO and SCK for ISP)  

RS = PORTB.2  

EN = PORTB.3


 


a **measure block**


 


PR1 - multi-turn potentiometer 1k to determine Vref at 4/5 Vcc. Brought to the end microcontroller PB1 (AIN1) analog comparator.


PR2,3,4 They are intended to calibrate each individual measuring channel. They are also multi-turn.


Transistors Q1 and Q2 operate in a current mirror. They are responsible for charging the tested capacitor constant current. To improve their thermal stability, joined them by heat-shrink.


Q3 transistor discharges capacitor, which was measured. It is controlled from the output of the processor PD4.


If you wonder why in the series to GND does not have with him 100 ohm resistor, note that the current base controls the output PD4 through resistor 3K that appropriate limits the current flowing through Q3. And that's why the resistor has not been chosen to use the next element in the circuit.


Transistors Q4, Q5, Q6 merely act as switches to select the measurement channel.  

They are controlled by PORTD 5,3,2 of the processor. These ports are connected to the base through a resistor 1k. This time these transistors are connected in series with a resistor fixed and variable to make calibration channel. Then the selected one of these three channels charges the capacitor.  

A charging time of the capacitor from 0V to Vref is the basis for the calculation of its capacity.  

Thus, this simple circuit allows the measurement of even very small capacity.


 


////////////////////////  

**Start-up and calibration**  

////////////////////////


 


You need to program ATtiny2313 file attached to the firmware Cmeter.hex


 


After soldering, use the PR1 Vref set at 4/5 VCC.


Use the PR5, PR6 to regulate LCD contrast.


Then for each channel to be carried out to measure capacitors.


For this you can use a capacitor model or a known (measured) capacity.


For the first channel capacitor within 1000pF - 2200pF. Use the PR2.


Channel within a second capacitor 1000nF - 2200 nF. Use the PR3.


For the channel of the third capacitor within 220uF - 1000uF should be enough. Use the PR4.


 


**Now You can use it.  

Enjoy**


 


 


 


**A bit deeper look...**


 


If you are not bored and you are interested in a deeper look at this, then let me explain what is this and how it work.


This is a capacity meter which works on the ATtiny2313 (probably is possible do it on the lot of AVR's). Range from 1pF to over 400 000 uF.


 


Firstly You must remeberanced this ver simply equation from school:


 


**Uc = I*t/C**


 


where


 


Uc - voltage on the cappacitor [V]  

I - charging current [A]  

t - charging current [s]  

C - cappacity [F]


 


From this equation we can do ordinary transform for moving out of "t" first dividing both sides by "t" Uc/t = I/C.


OK now let's divide both sides by Uc so we gotta 1/t = I/C*Uc from this following that my friend:


 


**t = CUc/I**


 


 


Now you may wonder, what purpose I subscribe to here the transformation equations.


Had he wanted to boast that I know math? The reason is the opposite. I want to bypass the complicated mathematics in this project. And in a moment I'll prove it.


 


 


Okay now we must understand what exactly it is apparent from this equation.


 


This results in four things:


 


1. It is a linear relationship. IF YOU CHARGE THE CAPACITOR WITH A CONSTANT CURRENT, THE INCREASE IN CHARGE IN THE CAPACITOR IS PROPORTIONAL TO THE CHARGING TIME.


2. As a result, I could understand that the capacitor 100nF / 100V has ten times more cargo than the capacitor 100 nF / 10 V at the time when they are charged to their limits tensions.


3. And this also shows that raising the charge voltage proportionally increases the loading time. This is very important !!!


 


 


And... So... finally... the our KEY arising from this:


If you know the charging voltage (from 0V to our known value Vref)  

If you know the charging current (MUST BE A CONSTANT !!! )  

If you know the charging time (from 0V to Vref)


Then you can determine what capacity is tested capacitor.


One condition is only here:


Charging MUST BE A CONSTANT CURRENT.


NOT CONSTANT VOLTAGE !!!


 


 


Well. Now let's get back to this simple equation and let me prove to you how I here miss complicated mathematics.


Our simple equation sounds exactly:  

t = C*Uc/I


and that's all my mate.


 


Here we sea a oscilloscope diagram from this


Measurement and plot charging.


 


 


![](https://community.atmel.com/sites/default/files/project_files/330nF%20IIIch.jpg)


 


 


 


Typically, the capacitor is charged with voltage. So take a look at how mathematically described the process of this load.


 


 


Vc = Vcc*(1-exp^(-t/RC))


UPS... and don't even I'll try to describe this equation... This equation itself looks daunting enough.


 


Bellow a diagram from this (stolen from internet)


curve charging


 


 


 


![](data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAMgAAACWCAMAAACsAjcrAAADAFBMVEUAAAABAQEAAgADAwMEBAQGBgYACgAICAgKCgoMDAwPDw8QEBATExMUFBQVFRUXFxcYGBgZGRkbGxsZHBkfHx8VMRUgICAjIyMnJycqKiorKystLS0uLi4wMDAyMjI2NjY4ODg5OTk6Ojo+Pj4ATgAPWQ8fXh8DZgMAfwAnRCdAQEBCQkJGRkZKSkpLS0tNTU1OTk5RUVFVVVVZWVldXV1eXl5hYWFlZWVpaWlwcHBxcXF0dHR4eHh5eXl8fHwIiwgCsAIquSoAwAABwAEDwQMEwQQGwQYIwggLwQsKwgoLwgsNww0Oww4QxBASxBIVwRUXxRcZxhkaxhocxhwexx4gxyAhxyEiyCIjyCMnyScqySorySssyiwvyy8wyzAzyzM0zDQ2zDY3zDc6zTo9zj0+zj5esV5Hw0dAzkBCz0JI0EhK0EpN0U1O0U5Q0lBR0lFT01NU01RW01ZX01dY1Fhb1Fte1V5g1WBi1mJj1mNl1mVm12Zp12lq2Gpr2Gtt2G1x2XF02nR323eAgICCgoKDg4OEhISGhoaLi4uPj4+QkJCTk5OXl5eYmJiZmZmbm5ufn5+goKCjo6OkpKSlpaWmpqanp6eqqqqsrKytra2urq6ysrK1tbW2tra5ubm6urq9vb2+vr6/v7+Lx4uD3YOF3oWK34qM34yO4I6R4JGS4ZKU4ZSV4ZWW4ZaY4piZ4pmc45yoyKi9yb2g5KCh5KGj5KOk5KSq5qqs5qyu566y6LK16LW36be56bm66bq96r3AwMDCwsLDw8PGxsbHx8fCysLHysfJycnKysrNzc3R0dHT09PU1NTV1dXZ2dnb29vc3Nzd3d3f39/A68DB68HD68PH7MfI7MjK7crN7s3O7s7Q7tDS79LU79TW8Nbb8dvd8d3f8t/g4ODh4eHk5OTn5+fo6Ojp6enq6urs7Ozt7e3v7+/g8uDj8uPk8+Tn8+fp9Onq9Ors9ezt9e3v9e/w8PDx8fHz8/Px9vHz9vP09PT09vT29vb29/b39/f39/fdbFGoAAABAHRSTlP///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////8AU/cHJQAACBlJREFUeNrt3XtcU1UcAPBjQjqcoWsFkUpgbzQCNSXQ6g5mEzSwfIRgllkoZZFGGqauGT5QIUwwyxAjRS2x1MRE0LSEqctXilFTGxATEgiWc7rP6dw70G3csYF3j3O7vz92t3Pv9fP77tzzuI8hgCwJwEE4iIMgI2Tkq+8B7CEUoZJPvb++4Tq2kGpP8jUzmPpw9NFfsIUYCIPTqQ8ruq7AFvIERRCoyNdLASDwX1whXifQi9yHen+oB+hxCFcI9TmE6rlgKADgI1wh3ks06iXCavJtVV8E6VuFKSTXB/CCTlBvs90RpPvn+I/sh1eO8Bix8jAbpiilvUvZMdfiIByEg3AQDsJBOAgH4SAcxAgCaFEchINwENZBeNUQVvMgXw01fAhzBbwMCI+Xru65+jhmkMClavXSIDhQ0S9nIIQ+B44JIVzk18e9zyJym5a4uZN5SZsC4zJgdS9gYY92/hFLEFVgt26BKhiWGrw2zLA1hLU1+3vtr8VzQMz2PijLhtB7He4j+wl/OPQYaiNewNuZEH0jCmV5efnZXQVUSJNRlOAxRWmmMi9EWefPSYqPi4uLiUQhIm6G1JUhZPqF1FeeQGUe0ZL0SPR+zCTESSDrIo+qFleskeaGitPbCwryqPQjWnOPQYm//G5y8pz8giJUOxUN6ABrdtHZ79VG5entKP9JY0YZ0o9A6SdQX3pRuRIl/o/rT+P1jcoiaWJczChD/uPik5KT531diNJvZr7XshNEf/H0dmlSfMxIghBFxsQnkflfbNTjdWJ1VVmYP3siVQ2RU5Pzdp1VMixwBERbUZiXGIMa8yhUDdLtvzbgeKqrv7wnL3FMBDqWxiV8eNpO1WB/SN3O2bFighC/mLDp7MVm6ICwB6R+d0osOp4kU+ftvuwQhF0gV89vnCwmIsZMz9/XAB0YDEPqdr45TkRIZuysaML5utaFjeiQkqC6cLSCWQhiiIioxD1OUDAJIRkRL0lLnMNgDNK0lWR887dj82Mcoi2eKRbZgwFprzPYDfL7rChC8kGd448Y8w1lfDfy1RsIsyDM4PMzoC7QdkjTjjhCkrhHC50OyfClngYKksOcbhDyFAoezEq3GVL3vjh8yvd2a+IeCtsh/rk3rgYFtECCNTZC9KemicQpl+3XGaXzPGyG8PoDAWUBfFQ1aTxemjyktaFZidvujyai853V45pDQBnMEFIfMn0NhSFyqqVYrRHt5kji1VP2mqAfLzWK4zZAPNStbUpnWGiCDQeYNYhyliT8YaVdvl6y013kZxSLbIAMDtUt94dw0DGNzFAj6Vk2QZQJhGT+7beSb7asvbW1NUZBXoMmr+a2B1EFgH5ytJkQ9JdTZYE6qqVYgSBH5GYtaNNVtl9iHMuDqEUuz3K9kAMCTBUCL4octNwOA2LddCJ6yzV4CxAFX3XjMLKUWy4aEFJ9y2AZZVZ5KhiHKN8STdirh7cCkQ2jyUEX6uE2QNVarsvuB6Gw7MbaYTKmIY1vE9F7LaZ942YMPeRI6PBKtAjIoclBNkKtCQluPbTcwqpNVucEMAxpTAmP3KK3/v3TQ0JBl2Vo4VltvFULXYi6UHXLGAh0vmQTEdysEcNvKZiDXJOGSzZfg52FDO/inmlc3PaGXWv5MSGquuX+ZVAebC3dzkD0WyLD52vb7i8gm6JCYHRotZaY3GSFlZmfXGpTIy3hWWmS21KykecKgHCJPWpkX3R4SiPN/lkDFVAxMJumxOQma2vQtpHQVBU8HGQpIWbbiHIKMaOedv80T+Bp2tW3lJjcZDXrtUxv0upkAtA/y1JCjPZaDTOI8eUdPF8wu8lqMo50IFR8JseRHWLx1o6e+JjdZDUd2W0PRkf24glEclMnzuA6PtdqO4JmM3jOXj+NiFNChiBOvByk/yr8hSLIAsipaNFGyAKINomIvcAGyA9RkgLIAkjdK8RcLRsgG0Vjz0EWQH4bL8rTswCiX2De0jGFnBtr1vViCtHOpakQHCHFUcaTRXwhaCyc2cQGCKqQ3ZAFEP17xLQGNkDOjRXvgmyASInJ9WyAXIilGUNwhGwS0YwhGEIaphJ5kA2QXeLoc2yAaN8g3rnKBsiZ0VHFkA0QKTG9iQ2QuljRNsgGyM6IiefZAGmYRizQswGyV2Kp78ULgua9lvpevCB/PE97IoIfZJuIft6LGwSN6gshGyDFUVElbIBokyyP6lhBSqIsj+pYQRYSlkd1nCDfTSbm69kA+VIs2QdZALnzdaK9QQQfyF3PWTpXxwzygGj0GTZAfn6KSNK6LmRVdpWNiXz79LPF0HUhQ9z7hh66RJ+IydPPpa8Rz/xU2n6AUudFIACgR8CKo9dpICZPP/s98tCDflYC+Dkv7qAe9ep63wYaiMnTz+Rf4aixEqDGeTGonRqhvxyEYRvpMMQZcbifYbn+U1t7LReFpIZ18rqWi8UAAPjMQ5zQRtQeOsgKyNogyA7I0DSWQLzknYJcOZqxinZl7bI1J684A+JxsOOQXh8vfqx3lyG0K2vudbv78RXA8f9nQZqbT4chPa3/FPEkHufsP65/8h63lRZmCN39F5dhdO33zzVH6Neu/+IvfB9gxvcCndkWqFH4UL/rzQnoBnyyXGji0i6kKrPKHAJhugAt1wmyNJqcYbhAWuoAmGyuI3/F7+9KlWHjMQ6gyQ8LNankVMdNgyfEuH6C1BhDjGok11uF6aHlWWa6+YFgdDrwmWemRpM7ACtIlvFfXSA3l5Nnmuv8UfcrwwqCSXAQDsJB/ieQ/wC3sT/61PlSdQAAAABJRU5ErkJggg==)


 


 


 


**And here is my prove for You** why I exhibit off before - I can do translate a simple equation.


From the above considerations, although it does not result directly - it shows how you can measure the capacitance of the capacitor and how can we do it.


In other words - we can measure time capacitor charging from constant current source.


 


Can we ?  

**Yes, we can !**


 


 


For this purpose we need:


- analog comparator - is contained in the microcontroller ATtiny2313 (and in many other AVR)  

- a referential voltage for comparator - Vref - PR1 providing this  

- the current source - for this task are the transistors Q1 and Q2 that operate in current mirror  

- measuring time of load - Yeah - we gotta quartz and TIMERS !


...Okay what we must do in the microcontroller program tasks - what HE must do?


- he must be able discharge measure capacitor  

- charge capacitor  

- counting time of load capacitor  

- serviced LCD 16*2 display  

- some times divide and multiply 32bit unsigned value by 8 bit unsigned value  

- convert the result into ASCII fixed point value  

- present on LCD our result  

- automatically change range of measure for bigger/smaller capacitors


 


 


That's why I do this in assembler, because this is a lot of tasks and translating this to C would be impossible to do in 2048 bytes flash ram only. The program at this moment have bellowed 1100 bytes from available 2048 bytes.


 


Okay - now I'll tell You how does it work from electrical side and from a program side.


 


We need once again a transformation:


t = C*Uc/I => t/C =Uc/I => C/t = I/Uc =>


=> **C = It/Uc**


Here we gain some benefits from properties in our circuits, because we have two times a constant values. We know before "I" and "Uc" and this dependence is important for the ease of calculations in the future.


The reference voltage was set at 4V. In the case of the first range, this gives 4 of clock ticks per one period (for 8MHZ quartz). Thus, the measurement takes place in this case with a resolution of 0.25 pF


If the reference voltage was 1V, then it would be one of clock ticks per period (for 8MHZ quartz). Here, the measurement would be made with a resolution of 1 pF.


 


Now do you get it ?  

Now do you know why I set UP Vref till 4/5 Vcc ?


What does the program do with this hardware


 


 


**After reset**


 


The meter starts pulls the output to ground for stability, the test is set to enter the waiting and unloading, and then begins the calibration measurement sequentially for all three channels to compensate for own capacitance and delays the switching transistors.


If the calibration was successful, offsets are saved for each channel and displayed once on the display, as triplet (24bit), although they did so 3-8-bit values. If the switched capacitor was connected to greater than about 30 pF, then the offset values are stored data determined empirically.


Each time the measurement offset is checked and adjusted if necessary, if the result is negative - that automatically compensates for differences due to temperature changes on the transistors.


Making the measurement takes place when inserted into the terminals of the capacitor legs. Needless to say, that you should put a capacitor previously discharged, because you can end up badly for the microcontroller. :-)


During measuring it is checked whether the size is not too small and that the measure does not last too long, for too long the measurement (about 0.5 sec), if this is not the highest range, the measurement is automatically restarted on another channel.


After the measurement is checked if the value is too low for a given range, and if it is too low, the measuring range is adjusted.


If the value is not considered too low, it is transformed number of "clicks" the clock on the result comprehensible to us and is displayed on the LCD in two versions - as measured established capacity for the channel (and are picofarads for the first channel and appropriately nano farads and microfarads) and the second score line is converted over three decades, which may be helpful for large values. With a picofarads displayed below are nanofarads etc.


After the presentation of the result range is switched to the next measuring cycle, after 3 seconds it starts again. 3 seconds is long enough to read the results of our perception, further discharges at this time the capacitor and stabilizes the measuring system.


The range of measured capacity can be from 1 pF to 4294967295 "ticks" clock (32bit unsigned value), which after transformation gives 452,101 uF (such measurement would take 536 seconds). Of course, I do not have such large capacitors (nay - the measure did not have time to unload it!).


The epilog


I think it is worth mentioning that is notable in the procedure for obtaining the (conversion) number as a sequence of ASCII from 32bit unsigned value. It is a small, clever algorithm to divide by 10, and received the rest of the presentation. Is short, it is important. Procedures multiplication and division are done via software, because not everyone AVR (including ATtiny2313) has built-in commands divide and multiply. In this way it will work on any AVR.


If you want to know more about the operation of the system, the rest of the search for the source ASM file. In the comments and how I named labels.


 


**Thank you for your attention dear reader.  

Best regards - wegi**


 


 


//----  

//todo  

// Function for large capacitors, which is taken into account their discharge time.  

// ATmega 8,16, 32 versions  

// Adapter for Arduino UNO to measure capacity.  

//---


 


 


First steps in phase test:


 


![](https://community.atmel.com/sites/default/files/project_files/phase_test.jpg)


 


Proto C\_meter


 


![](https://community.atmel.com/sites/default/files/project_files/proto.jpg)


 


Measures...


 


![](https://community.atmel.com/sites/default/files/project_files/diagrams.jpg)


 


 


 


PCB done in  the EasyEDA:


 


![](https://community.atmel.com/sites/default/files/project_files/maked_side_a.jpg)


 


And final product :)


 


![](https://community.atmel.com/sites/default/files/project_files/Cruel_C_meter.jpg)


 


 


If you no bored below is source code:



```

//--------------------------------
//--- C meter firmware for AVR ---
//---      2016-03-17          ---
//--------------------------------

//---------------------
// Lcd = PORTB.4,5,6,7
// RS=PORTB.2
// EN=PORTB.3
// LCD\_TYPE = 16 * 2
//---------------------

//-------------------------------------
//--- for AT90S2313 and ATtiny2313 ----
//-------------------------------------

///-------------------------
//--- our private deines ---
//--------------------------
.equ     RAMSTART   = $60
.equ     RAMEND     = $DF
//-
.equ     ACSR       = $08

.equ     DDRD       = $11
.equ     PORTD      = $12

.equ     DDRB       = $17
.equ     PORTB      = $18

.equ     ICR1H      = $25
.equ     ICR1L      = $24

.equ     TCNT1L     = $2c
.equ     TCNT1H     = $2d
.equ     TCCR1B     = $2e
.equ     TCCR1A     = $2f

.equ     TIFR       = $38
.equ     TIMSK      = $39

.equ     SPL        = $3d

//-
.def	XL	=	r26
.def	XH	=	r27
.def	YL	=	r28
.def	YH	=	r29
.def	ZL	=	r30
.def	ZH	=	r31
//---------------------------------------------
//--- controll for change microcontroller's ---
//---------------------------------------------

//--------
.equ     CLEAR\_FLAG\_VALUE    = 0b11000000  // a value of clear interrupt flag from TOV1 and COMPARE\_TC1
.equ     ACSR\_START\_VALUE    = 0b00000111  // start value for ACSR
.equ     DDRB\_START\_VALUE    = 0b11111100  // initial inout/output value of PORTB
.equ     TIMSK\_START\_VALUE   = 0b00001000  // IRQ CAPTURE TC1 enable value
.equ     TIMER1\_START\_VALUE  = 0b01000001  // value for run TC1
//---------------------------------------------------------
//--- charge/discharge  = PD4 (0 = charge ; 1 = DIS\_charge)
//--- channel 1:        = PD5 (1 = enable ; 0 = disable)
//--- channel 2:        = PD3 (1 = enable ; 0 = disable)
//--- channel 3         = PD2 (1 = enable ; 0 = disable)
//---------------------------------------------------------
.equ     start\_value\_DDRD  = 0b00111100  // our bits to manipulate, charge, discharge, range - as OUTPUT
.equ     start\_value\_PORTD = 0b00010000  // a start value - everything in hi except a discharge bit

.equ     pre\_value\_1       = 0b00110000  // discharge + charge chanel 1
.equ     load\_value\_1      = 0b00100000  // only charge   by  channel 1

.equ     pre\_value\_2       = 0b00011000  // discharge + charge chanel 2
.equ     load\_value\_2      = 0b00001000  // only charge   by  channel 2

.equ     pre\_value\_3       = 0b00010100  // discharge + charge chanel 3
.equ     load\_value\_3      = 0b00000100  // only charge   by  channel 3
//--------
.equ     OUT\_ASCII\_BUFF    = RAMSTART+1            // 10 bytes for ASCII decimal string 32bit unsigned value
.equ     LEN\_ASCII\_BUFF    = OUT\_ASCII\_BUFF + 11   // length of ASCII digits
.equ     MY\_LONG\_INT       = OUT\_ASCII\_BUFF + 12   // 4 bytes LONG INTEGER - 32bit unsigned value
.equ     offs1             = OUT\_ASCII\_BUFF + 16   // calibrate value of channel 1
.equ     offs2             = OUT\_ASCII\_BUFF + 17   // calibrate value of channel 2
.equ     offs3             = OUT\_ASCII\_BUFF + 18   // calibrate value of channel 3
.equ     calibrate         = OUT\_ASCII\_BUFF + 19   // calibrate indicator (0 = do calibrate ; <> 0  = no calibrate)
.equ     range             = OUT\_ASCII\_BUFF + 20   // a range of meassure 0,1,2 = channel 1,2,3
.equ     pre\_val           = OUT\_ASCII\_BUFF + 21   // for actual used channel PRE\_charged value
.equ     load\_val          = OUT\_ASCII\_BUFF + 22   // for actual used channel ENABLE (charge by channel) value
.equ     offset            = OUT\_ASCII\_BUFF + 23   // calibrate value for actual used channel

//--------
.def     var1          =     r1   // divide | multiply - our long integer is here
.def     var2          =     r2
.def     var3          =     r3
.def     var4          =     r4

.def     mod10         =     r16  // helps variable for divide | multiply
.def     cntr\_1        =     r17  // helps variable for divide | multiply
.def     divisor       =     r18  // disor for divide proc
.def     multiplier    =     r18  // multiplier for multiply routine
//--------

          .CSEG
.ORG     0x0000

//------------------------------
//-- vectors of interrupts -----
//------------------------------

               rjmp     \_RESET\_PROC
               reti     // \_IRQ\_INT0  - not used
               reti     // \_IRQ\_INT1  - not used
               cli      // \_IRQ\_CAPTURE\_TC1 - ok block all IRQ
               rjmp     \_IRQ\_CAPTURE\_TC1  // go to IRQ service

               // rest of INTERRUPTS - NOT used

//==============================================================
\_RESET\_PROC:
               cli      // not necessary but... maybe other models of AVR's...
               ldi      yl,low(RAMEND)
               out      SPL,yl  // stack init
               ldi      yh,high(RAMEND)
               sbiw     yl,32
//====================================================
               rcall    stop\_meassure  // set up pinout to ground - discharge...

               rcall    LcdIni

               ldi      zl, low(welcome * 2)
               ldi      zh, high(welcome * 2)
               rcall    \_LSc  // print welcome info from 1st line

               ldi      r24,1 // print calibrate info from 2nd line
               rcall    \_from\_line2
               ldi      zl, low(info\_txt * 2)
               ldi      zh, high(info\_txt * 2)
               rcall    \_LSc

               rcall    \_Wsec  // wait 1sec. for stabilize pins
set\_calibrate:
               ldi      zl, low(RAMSTART+1)
               ldi      zh, high(RAMSTART+1)

//---
               ldi      r16, 0x20   // clear data ram 32 bytes of variables, string etc...
               clr      xh
setcall2:
               st       Z+, XH
               dec      r16
               brne     setcall2
//---
               ldi      r16, 120    // pre calibrate value channel 1
               sts      offs1, r16
               ldi      r16, 0x13   // pre calibrate value channel 2
               sts      offs2, r16
               ldi      r16, 0x11   // pre calibrate value channel 3
               sts      offs3, r16

//=====================================
//==========   repeat main loop =======
//=====================================
starter:
               ldi      yl,low(RAMEND)
               out      SPL,yl          // stack reinit
               ldi      yh,high(RAMEND)
               sbiw     yl,32

               rcall    start\_meassure  // OK - let's go rocket :)
//====================================
selfee:
               in       r20, TIFR
               andi     r20, 0x80
               breq     selfee
               cli                              // block interrupts and counting overflow
               ldi      r20, CLEAR\_FLAG\_VALUE   // clear interrupts flag from TOV and COMPARE\_TC1
               out      TIFR, r20
               sts      calibrate, r20  // if OVERFLOW was raeched - calibrate is impossible
                                        // calibrate <> 0 = DON'T calibrate
               rcall    read\_hi\_data    // r22 = range, after this rcall

//-------------------------------------------------------------
//--- IF TC1 is overflowed extend 16bit to 32 counter value ---
//-------------------------------------------------------------
               inc      r20
               sts      MY\_LONG\_INT+2,r20
               brne     no\_increase
               inc      r21
               sts      MY\_LONG\_INT+3,r21
no\_increase:
               cpi      r22, 0
               brne     second\_range
               cpi      r20, 0x30   // first range after reached 3145728 tick's is change onto next range
               brne     out\_call
               rcall    increment\_range
               rjmp     chg\_range

second\_range:
               cpi      r22, 1
               brne     out\_call
               cpi      r20, 0x40  // 4194304 - about half seconds
               brne     out\_call
               rcall    increment\_range
               rjmp     chg\_range
//-----
out\_call:
               sei               // unlock interrupts
               rjmp     selfee   // OK repeat over and over again

//----------------------------------------------------
//--- after CAPTURED TIMER1 perform calc and print ---
//----------------------------------------------------
continuum:
               ldi      yl,low(RAMEND)
               out      SPL,yl // stack re-init
               ldi      yh,high(RAMEND)
               sbiw     yl,32
//---
               rcall    stop\_meassure  // ok - stop timer, disable IRQ allowed, let's start discharge etc...
               rcall    SUBSTRACT\_MY\_LONG\_INT // substract CALIBRATE OFFSET and repair offset if RESULT < 0
               lds      r16, range
               cpi      r16, 0
               brne     continuum\_2
               rcall    make\_picofarads  // prepare data and string for channel 1 range
               rjmp     continuum\_4
continuum\_2:
               cpi      r16, 1
               brne     continuum\_3
               rcall    make\_nanofarads  // prepare data and string for channel 2 range
               rjmp     continuum\_4
continuum\_3:
               rcall    make\_microfarads  // prepare data and string for channel 3 range

continuum\_4:
               rcall    LcdIni  // for visible effect screen refresh

//--------------------------------------------------
//------     print string to LCD display     -------
//--------------------------------------------------
//-----
               rcall    get\_string\_pointers  // make ASCII string and get address of variables/registers

               cpi      r16,0
               brne     pr\_N\_F

               ldi      r24, 'p'
               rcall    print\_meassure   // pF
               ldi      r24, 'n'         // additional print as nF
               rjmp     noprt

pr\_N\_F:
//-----
               cpi      r16,1
               brne     pr\_U\_F

               ldi      r24, 'n'
               rcall    print\_meassure   // nF
               ldi      r24, 'u'         // additional print as uF
               rjmp     noprt
pr\_U\_F:
               ldi      r24, 'u'
               rcall    print\_meassure  // uF
               ldi      r24, 'm'        // additional print as mF
noprt:
               rcall    three\_decades\_low  // convert string for additional value
               rcall    print\_big\_number   // and print our fixed point

               ldi      zl, 0x00
               out      PORTB,zl  // lines to GND for stabilize

               rcall    \_Wsec    // 3 sec wait and stabilize circuit
               rcall    \_Wsec
               rcall    \_Wsec

//----------------------------------
//--------------- adt chck ---------
//----------------------------------

			   lds      r16, range
               cpi      r16,2
               brne     no\_adt\_disch

               rcall    divide\_by\_100  // divide to 100 uF per one
               ldi      divisor, 50    // divide to 5mF per one
               rcall    make\_divide    // 5 mF per second discharge

			   rcall    copy\_long\_to\_registers
dischg\_loop:
			   rcall    copy\_registers\_to\_long
               rcall    clear\_top\_line
               ldi      zl, low(dischg\_info  * 2)
               ldi      zh, high(dischg\_info * 2)
               rcall    \_LSc  // print discharge time info from 1st line
               rcall    get\_string\_pointers
print\_buff:
               ld       r24, Z+
               rcall    \_Lch
               dec      r17
			   cpi      r17,0
               brne     print\_buff

               rcall    \_Wsec

               rcall    clear\_top\_line

               rcall    copy\_long\_to\_registers               

               ldi      r16, 1       // decrement long integer
               clr      r20

               sub      var1, r16
               sbc      var2, r20
               sbc      var3, r20
               sbc      var4, r20
               brcc     dischg\_loop

no\_adt\_disch:
//-------------------------------------
               ldi      r24, 15
               rcall    \_from\_line1
               ldi      r24, '*'     // meassuring sign for reader
               rcall    \_Lch

               ldi      zl, 0x00
               out      PORTB,zl  //// lines to GND for stabilize

               rcall    increment\_range  // change range - try meassure in next range

               rjmp     starter
//-------------------------
three\_decades\_low:
               push     r24  // save r24
               ldi      r24, 1
               rcall    \_from\_line2
               rcall    divide\_by\_100  // delete 2 decades
               rcall    get\_string\_pointers
               pop      r24  // restore r24
               ret
//---
get\_string\_pointers:
               rcall    BIN32u\_2\_ASCII   // first make the properly string
               lds      r16, range       // and get address of variables/registers
               lds      r17, LEN\_ASCII\_BUFF
               ldi      zl,low(OUT\_ASCII\_BUFF+10)
               ldi      zh,high(OUT\_ASCII\_BUFF+10)
               sub      zl, r17
               clr      r0
               sbc      zh, r0
               ret
//---
print\_range:
               ldi      r24, 32
               rcall    \_Lch
               pop      r24   // in r24 on the stack was "p" | "n" | "u" | "m"
               rcall    \_Lch
               ldi      r24, 'F'
               rjmp     \_Lch
//---
print\_three\_last\_digit:
               cpi      r18, 2
               breq     print\_two\_last\_digit

               lds      r24, (OUT\_ASCII\_BUFF+7)
               rcall    \_Lch
print\_two\_last\_digit:
               lds      r24, (OUT\_ASCII\_BUFF+8)
               rcall    \_Lch
print\_last\_digit:
               lds      r24, (OUT\_ASCII\_BUFF+9)
               rcall    \_Lch
               rjmp     print\_range
//---
print\_0\_dot:
               rcall    print\_0
print\_dot:
               ldi      r24, '.'
               rjmp     \_Lch
//-
print\_0:
               ldi      r24, 0x30
               rjmp     \_Lch
//-------------------------
print\_meassure:
               ldi      r18, 2
               ldi      r19, 3
               rjmp     print\_fixed\_point
print\_big\_number:
               ldi      r18, 3
               ldi      r19, 4
print\_fixed\_point:
               push     r24
               cp       r17, r19
               brcc     print\_from\_buff
               rcall    print\_0\_dot
               cp       r17, r18
               breq     print\_three\_last\_digit
               rcall    print\_0
               cpi      r17, 2
               breq     print\_two\_last\_digit
               rcall    print\_0
               rjmp     print\_last\_digit
//---
print\_from\_buff:
               ld       r24, Z+
               rcall    \_Lch
               dec      r17
               cp       r17,r18
               brne     print\_from\_buff

               rcall    print\_dot
               rjmp     print\_three\_last\_digit
//-------------------------
read\_hi\_data:
               lds      r22, range
               lds      r20, MY\_LONG\_INT+2
               lds      r21, MY\_LONG\_INT+3
               ret
;---------------------------
clear\_top\_line:
               rcall    set\_top\_sursor
			   ldi      r17, 0x10
clear\_line:
               ldi      r24, 0x20     // clear top line
               rcall    \_Lch
			   dec      r17
			   cpi      r17, 0
			   brne     clear\_line
set\_top\_sursor:
               ldi      r24, 1
               rjmp    \_from\_line1
//================================================
stop\_meassure:

               rcall    discharge
//--------------------------
//--- disable interrupts ---
//---    stop   TIMER1   ---
//--------------------------
               clr      zl
               out      TIMSK, zl
               out      TCCR1B, zl
               out      TCCR1A, zl
               out      ACSR, zl

               in       r0, TIFR
               out      TIFR, r0  // clear all IRQ flags
               ret
//------------------------------------------------
start\_meassure:
//------------------------
//--- block interrupts ---
//------------------------
               cli
               rcall    stop\_meassure
               rcall    set\_params
//--------------------------------------------------------------------------
//-----   PORTB 6 lines as output and ain0 ain1 = input (comparator)   -----
//--------------------------------------------------------------------------

               ldi      r23, DDRB\_START\_VALUE
               out      DDRB,r23
               clr      r23
               out      PORTB,r23  // pin to GND 

//---------------------------------
//-----        Timer1 = 0    ------
//---------------------------------
               rcall    CLEAR\_MY\_LONG\_INT
               clr      zh
               out      TCNT1H,zh
               out      TCNT1L,zh
               out      TCCR1A, zh
//--------------------------------------------------------
//----  allowed for IRQ capture TIMER1 by comparator  ----
//--------------------------------------------------------
               ldi      r20, TIMSK\_START\_VALUE
               out      TIMSK, r20
//---------------------------------------------------------------
//---------           setup prescaler = 1             -----------
//------       by bits CS12 CS11 CS10 in TCCR1B          --------
//---------              0    0    1                  -----------
//----------         PHISICAL START TIMER1           ------------
//---------------------------------------------------------------

               lds      zl, pre\_val  // discharge and enable actual "range - channel"
               out      PORTD, zl
               ldi      zl, 0x06    // a few cycles delay for switch a transistor
ctt0:          dec      zl
               brne     ctt0

               lds      r21, load\_val

               ldi      r20,TIMER1\_START\_VALUE      // CS12 CS11 CS10 = 0 1 0
               out      TCCR1B, r20                 // START TIMER1

               out      PORTD,  r21                 // stop discharge stard charging \_C\_appacitor via "channel - range"

//--------------------------------------------------------
//-----             SETUP ACSR REGISTER              -----
//--------------------------------------------------------
//-----         allowed trigging of capture          -----
//-----        TIMER1  via comparator  ACIC = 1      -----
//-----   - - - - - - - - - - - - - - - - - - - - -  -----
//-----         event for starting interrupt         -----
//-----    rising edge on the out of comparator      -----
//-----       by bit ACIS1 ACIS0 in ACSR register    -----
//-----                1     1                       -----
//--------------------------------------------------------
               ldi      r24, ACSR\_START\_VALUE
               out      ACSR,r24

//-----------------------
//--- clear IRQ flags ---
//-----------------------
               in       R24,TIFR
               out      TIFR, R24
//---------

//-------------------------
//--- unlock interrupts ---
//-------------------------
               sei  // ok - wait for capture TC1
               ret
//***********************************************

//==================================
//========    discharge     ========
//==================================
discharge:
               ldi      r23, start\_value\_DDRD
               out      DDRD, r23
               ldi      r23, start\_value\_PORTD
               out      PORTD, r23
               ret
//------------------------
//------ IRQ SERVICE -----
//------------------------
\_IRQ\_CAPTURE\_TC1:
//-----------------------------------------------
//-----      read captured TIMER1 value     -----
//-----      and store her to variable      -----
//-----------------------------------------------
               in       r0, ICR1L
               sts      MY\_LONG\_INT, r0

               in       r16, ICR1H
               sts      MY\_LONG\_INT+1, r16  // OK TC1 value is SAVED

               lds      r17, calibrate  // now check - we are in a calibrate mode or no ?
               cpi      r17,0
               brne     no\_calibrate
//============================================
//=== IF calibrate is true so do it bellow ===
//============================================
               rcall    store\_offset\_value  // stor and next range do or finish
chg\_range:
               rcall    discharge
               ldi      zl,50
               rcall    \_Wms
               rjmp     starter   // meassure again next range
//---------------------------------------------------
//--- IF calibrate = false then we gotta meassure ---
//---------------------------------------------------
no\_calibrate:
               rcall    read\_hi\_data
               cpi      r22, 1        // in R22 actual range
               brne     third\_range

               cpi      r20, 0       // here we are in 1st range
               brne     first\_range
               cpi      r16, 0
               brne     first\_range
               mov      r16, r0
               subi     r16, 30     // check the minimal value for range and change it if necesseary
               brcc     first\_range

set\_first\_range:
               sts      range, r20
               rjmp     chg\_range

third\_range:
               cpi      r22, 2
               brne     first\_range
               ldi      divisor, 100
               rcall    copy\_long\_to\_registers
               rcall    div32u\_by\_8u

               ldi      r16, 8       // check minimal value for 3rd range
               clr      r20

               sub      var1, r16
               sbc      var2, r20
               sbc      var3, r20
               sbc      var4, r20
               brcs     set\_first\_range
first\_range:
               rjmp     continuum
//-----------------------------------------------------------
//---  save from R0 offset value for actual  tested range ---
//-----------------------------------------------------------
store\_offset\_value:
               cpi      r16,0
               breq     store\_2
               sts      calibrate, r16
               ret
store\_2:
               mov     r24, r0
               rcall   print\_hex  // print offset value for actual tested range

               lds     r16, range
               cpi     r16, 0
               brne    store\_3
               sts     offs1, r0
increment\_range:
               lds     r16, range
               inc     r16
               cpi     r16, 3
               brne    incr\_r\_2
               clr     r16
incr\_r\_2:      sts     range, r16
               ret
store\_3:       cpi     r16,1
               brne    store\_4
               sts     offs2, r0
               rjmp    increment\_range
store\_4:
               sts     offs3,r0
               sts     calibrate, r16  // r16 <> 0 so end of calibrate sign
store\_5:
               rcall   discharge  // after store discharge, stabilize circuit and next range do...
               rcall   \_Wsec
               rjmp    increment\_range
//-----------------------------------

//----------------------------
//------ IRQ SERVICE END -----
//----------------------------

//-----------------------------------------------------
//--- store parameters for actual range to meassure ---
//-----------------------------------------------------
set\_params:
               lds     r19, range
               andi    r19, 0x03
               breq    set\_first
               cpi     r19, 0x01
               breq    set\_second
set\_third:
               ldi     r16, pre\_value\_3
               ldi     r17, load\_value\_3
               lds     r18 , offs3
               rjmp    write\_params
set\_second:
               ldi     r16, pre\_value\_2
               ldi     r17, load\_value\_2
               lds     r18 , offs2
               rjmp    write\_params
set\_first:
               ldi     r16, pre\_value\_1
               ldi     r17, load\_value\_1
               lds     r18 , offs1
write\_params:
               sts     pre\_val, r16
               sts     load\_val, r17
               sts     offset, r18
               ret
//---------------------------
//-----------------------------------------------------------------------------:
// 32bit Unsigned convert BIN to ASCII
//
// Register Variables
//  Call:
//         var[1:4]           = 32bit unsigned value (0x00000000..0xffffffff)
//         mod10              = <don't care> (high register must be allocated)
//         cntr\_1             = <don't care> (high register must be allocated)
//
//
//  Result:
//         OUT\_ASCII\_BUFF[0:9] = 10 bytes will be overwrite as ASCII decimal bin representation
//         var[1:4]            = (0x00000000)
//         mod10               = length of ASCII numbers
//         cntr\_1              = 0
//         xl,xh               = @OUT\_ASCII\_BUFF first ASCII number
//
//---------------

BIN32u\_2\_ASCII:
               rcall   copy\_long\_to\_registers

               ldi     xl, low(OUT\_ASCII\_BUFF+10)
               ldi     xh, high(OUT\_ASCII\_BUFF+10)

               ldi     divisor, 10   //divide by 10

BIN\_TO\_ASCII:
               rcall   div32u\_by\_8u  //make divide by 10
//---------------------------
               ori     mod10, 0x30   // O\_o HERE we gotta our the rest of div10 :] o\_O
               st      -x,mod10      // store as ASCII NUMBER
               cpi     xl, low(OUT\_ASCII\_BUFF)
               brne    BIN\_TO\_ASCII

               ldi     mod10,9

clr\_lead:      ld      cntr\_1, x
               cpi     cntr\_1, 0x30
               brne    stop\_it
               ldi     cntr\_1, 0x20
               st      x+, cntr\_1
               dec     mod10
               brne    clr\_lead
stop\_it:
               inc     mod10
               sts     LEN\_ASCII\_BUFF, mod10  // save how many bytes ASCII NUMBERS strig is here
               ret
//---
//------------------------------------------------------------
//--- divide unsigned 32bit value by 8bit unsigned divisor ---
//------------------------------------------------------------
//--- input :                                              ---
//--- var 1,2,3,4 = 32 bit unsigned value                  ---
//--- divisor: our 8bit unsigned value of divide           ---
//------------------------------------------------------------
//--- output:                                              ---
//--- mod10 is touched, rest of divide                     ---
//--- var 1,2,3,4 = RESULT                                 ---
//--- cntr\_1 is touched = 0                                ---
//------------------------------------------------------------
div32u\_by\_8u:
               clr     mod10          // initialize mod10 = 0

               ldi     cntr\_1, 32     // lc = 32 (times bits to shift)
divloop:                              // simple 32bit\_u divide by const 8bit\_u
               lsl     var1
               rol     var2
               rol     var3
               rol     var4
               rol     mod10
               cp      mod10, divisor     // divisor value
               brcs    divlp2
               inc     var1
               sub     mod10, divisor     // divisor value
divlp2:
               dec     cntr\_1
               brne    divloop
               ret
//---
//-----------------------------------------------------------------------
//--- copy from STATIC RAM our 32bit value into registers var 1,2,3,4 ---
//-----------------------------------------------------------------------
//--- input : don't care                                              ---
//--- output :                                                        ---
//--- var 1,2,3,4 = 32bit value                                       ---
//--- XL, XH, ZL, ZH, cntr\_1 is touched                               ---
//--- cntr\_1 = 0                                                      ---
//--- XL,XH  = @(var4 + 1)                                            ---
//--- ZL,ZH  = @(MY\_LONG\_INT + 4)                                     ---
//-----------------------------------------------------------------------
copy\_long\_to\_registers:
               rcall   set\_addr\_var
COPY\_1:
               ld      var4, Z+
               st      x+, var4
               dec     cntr\_1
               brne    COPY\_1
               ret
//---
set\_addr\_var:
               ldi     xl, 1
               clr     xh

set\_my\_long\_addr:

               ldi     zl, low(MY\_LONG\_INT)
               ldi     zh, high(MY\_LONG\_INT)
               ldi     cntr\_1,4
               ret
//-------------
copy\_registers\_to\_long:
               rcall   set\_addr\_var
COPY\_2:
               ld      var1, X+
               st      Z+, var1
               dec     cntr\_1
               brne    COPY\_2
               ret
//-------------
CLEAR\_MY\_LONG\_INT:
               rcall   set\_my\_long\_addr
               clr     r0
COPY\_4:
               st      Z+, r0
               dec     cntr\_1
               brne    COPY\_4
               ret
//-------------
//------------------------------------------------------------------
//--- substract CALIBRATE OFFSET and repair offset if RESULT < 0 ---
//------------------------------------------------------------------
SUBSTRACT\_MY\_LONG\_INT:

               lds     r0, offset
               rcall   set\_my\_long\_addr

               clc     // prepare carry for substract

COPY\_5:
               ld      r1, Z
               sbc     r1, r0
               st      Z+, r1
               clr     r0
               dec     cntr\_1
               brne    COPY\_5
               brcs    repair\_offset
               ret
//---
repair\_offset:
               lds     r0, offset
               lds     r1, MY\_LONG\_INT
               add     r0, r1

               lds     r2, range

               ldi     ZH, high(offs1)
               ldi     ZL, low(offs1)
               add     ZL, r2
               st      Z,      r0
               rjmp    CLEAR\_MY\_LONG\_INT
//-----------------------------------------------------------------------------:

//-------------------------------------------------------------------
//----------   operations for prepare fixed point value   -----------
//-------------------------------------------------------------------
make\_nanofarads:
               ldi     multiplier, 2   // multiple 2 times
               rjmp    make\_mult
make\_picofarads:
               ldi     multiplier, 25
make\_mult:
               rcall   copy\_long\_to\_registers
               rcall   mul32u
               rjmp    copy\_registers\_to\_long

//------------------------------------------------------------------
// ---------- like above - prepare our fixed point value  ----------
//------------------------------------------------------------------
divide\_by\_100:
               ldi     divisor, 100
make\_divide:
               rcall   copy\_long\_to\_registers
               rcall   div32u\_by\_8u
               rjmp    copy\_registers\_to\_long
//---
make\_microfarads:
               ldi     divisor, 10   //divide by 95
               rjmp    make\_divide
//-------------

//-----------------------------------------------------------------------------
// 32bit x 8bit unsigned multiply
//
// Register Variables
//  Call:  var1-4         = 32bit multiplicand
//         multiplier     = multiply 8 bit unsigned value
//         mod10          = <don't care>
//         cntr\_1         = <don't care> (high register must be allocated)
//
//  Result:var1-4         = 32bit result
//         multiplier     = <not changed>
//         cntr\_1         = 0
//
//-------------------------
mul32u:
               ldi     cntr\_1,33
               sub     mod10, mod10  // prepare carry
mul\_loop:
               brcc    mul\_lp2
               add     mod10,multiplier
mul\_lp2:
               ror     mod10
               ror     var4
               ror     var3
               ror     var2
               ror     var1
               dec     cntr\_1
               brne    mul\_loop
               ret
//-----------------------------------------
print\_hex:
               push    r24
               swap    r24
               rcall   pr\_h1
               pop     r24
pr\_h1:         andi    r24,0x0f
               cpi     r24,10
               brcs    pr\_h2
               subi    r24,-7
pr\_h2:         subi    r24,-'0'
               rjmp    \_Lch
//----------------------------------------
//-------------------------
//---  init LCD display ---
//-------------------------
//---------------------
// Lcd = PORTB.4,5,6,7
// RS=PORTB.2
// EN=PORTB.3
// LCD\_TYPE = 16 * 2
//---------------------
LcdIni:
               ldi     zl, DDRB\_START\_VALUE  // 11111100
               out     DDRB,zl
               //sbi          DDRB,2
               //sbi          DDRB,3
               cbi     PORTB,2
               cbi     PORTB,3
               ldi     zl, 0x18
               rcall   \_Wms
               ldi     r24, 0x03
               rcall   \_LOut
               ldi     zl, 0x08
               rcall   \_Wms
               rcall   \_LEN
               ldi     zl,0x0d
               rcall   \_Wus
               rcall   \_LEN
               cbi     PORTB,4
               rcall   \_LEN
               ldi     r24, 0x28
               rcall   \_LCtr
               ldi     r24, 0x06
               rcall   \_LCtr
               ldi     r24, 0x0c
               rcall   \_LCtr
\_CLS:
               ldi     r24, 0x01
               rcall   \_LCtr
               ldi     zl, 0x04
               rjmp    \_Wms
//---------------------------
//--- print char from R24 ---
//---------------------------
\_Lch:          sbi     PORTB,2
               rjmp    \_LNib
//---
\_from\_line2:
               ori     r24, 0x40
\_from\_line1:
               ori     r24, 0x80
               dec     r24
//---
\_LCtr:         cbi     PORTB,2
//---
\_LNib:         mov     r21, r24
               swap    r24
               rcall   \_LOut
               mov     r24, r21
\_LOut:         in      r23, PORTB
               andi    r23, 0x0f
               swap    r24
               andi    r24, 0xf0
               or      r24, r23
               out     PORTB, r24
//---------------------
//--- EN line setup ---
//---------------------
\_LEN:          sbi     PORTB, 3
               sbi     PORTB, 3 // 2 cycles delay !! FOR 8 MHZ !!!
               sbi     PORTB, 3 // 2 cycles delay !! FOR 8 MHZ !!!
               cbi     PORTB, 3
               push    zl
               ldi     zl, 0x04
               rcall   \_Wus
               pop     zl
\_stop:
               ret
//--------------------------------------
//--- print string from flash memory ---
//--------------------------------------
\_LSc:          lpm
               tst     r0
               breq    \_stop
               mov     r24,r0
               rcall   \_Lch
               adiw    zl,1
               rjmp    \_LSc
//------------------------------------------
\_Wsec:
               ldi     zl, 232
               rcall   \_Wms
\_Whalf:
               rcall   \_Wms
               rcall   \_Wms
//------------------------------------
//---- !!! FOR 8MHZ CLOCK  !!!  ------
//------------------------------------

//----------------------------
//--- a mili seconds delay ---
//----------------------------
\_Wms:          ldi     r20, 0x14
\_Wms1:         ldi     r21, 0x85
\_Wms2:         dec     r21
               brne    \_Wms2
               dec     r20
               brne    \_Wms1
               dec     zl
               brne    \_Wms
               ret
//-----------------------------
//--- a micro seconds delay ---
//-----------------------------
\_wus:          ldi     r22, 25
\_wus1:         dec     r22
               brne    \_wus1
               dec     zl
               brne    \_wus
               ret
//--------------------------
//-- a start info string ---
//--------------------------
welcome:
               .db "  WEGI C meter" , 0
info\_txt:
               .db "calibr. 0x" , 0

dischg\_info:
               .db "discharge " , 0
//-------------------
// todo :
// ATmega328 version for ardiuno uno R3 shield C\_meter
//-------------------

```
 


A new FW version on bitbucket.org


[https://bitbucket.org/wegi1/wegi...](https://bitbucket.org/wegi1/wegi_cruel_c_meter_attiny2313/src/0882ea0cf953b382082b95f0316d4915269ede40/C_meter.asm?at=master&fileviewer=file-view-default)


Added extra discharge function for huges cappacitors over 5000 uF


 


EOT

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
- Electricity Meters
