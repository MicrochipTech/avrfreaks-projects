# BatteryMeter

Uploaded by kd0rc on 2012-06-10 15:23:00 (rating 0 out of 5)

## Summary

I use large trolling motor deep cycle batteries for ham radio, astronomy, and (strangely enough) trolling for fish. I am always concerned about how much capacity I have left on a battery, and thought the ATTiny 861A looked like a good basis for a charge status meter.


The meter is powered from the same battery that is under test, so no separate batteries or power supply are required. 


The meter shows voltage to within .05 volts or so as measured by an EXTECH model 22-816 purchased from Radio Shack. It also shows percent capacity remaining in 10% increments based on a table of voltages that I got from this web site: [http://www.solarnavigator.net/ba...](http://www.solarnavigator.net/battery_charging.htm) 


An audio alarm is provided that sounds when the battery reaches 30%. An alarm reset button silences the alarm until it reaches each next lower 10% until it reaches 0%. At 0%, the alarm sound changes, and cannot be reset by the button. 


The meter draws about 12 mA (a bit higher when the alarm is sounding) when running normally, and around 5 mA when in sleep mode.


A six pin header is provided to allow the unit to be re-programmed from the STK 500 or any other Atmel compatible programmer.


The code is written in mikroBasic PRO and takes up just under 4K. I made no attempt at optimizing the code, and I threw in frivolous things like sending my ham radio call sign in morse at start-up. The code could easily be cut to around 2K by getting rid of the sound routines and coding the sound directly, getting rid of the morse code, and a little general optimization.

## Compilers

- mikroBasic for AVR

## Tags

- Complete code with HW
- mikroBasic for AVR
