# Rowgoski Coil Integrator

Uploaded by ksvitale on 2012-08-26 20:38:00 (rating 0 out of 5)

## Summary

This project is an off-shoot of the "AVR Data Recorder" project that is intended to be used with it. 


There is no AVR processor involved, thus no actual code. Only schematics and construction instructions are provided.


A Rowgowski Coil is used to measure current flow through wires without actually breaking the connection and inserting a shunt to measure across. They are used in the industry to measure current flows exceeding kilo-amps in magnitude.


This project, in its current incarnation, can measure A.C. current flow down to about an amp.


The Rogowski Coil itself is hand-made. The instructions are attached below.


The Rogowski Coil is fed to a differential amplifier with a 100x gain, which is then fed to the integrator stage where it is integrated and further amplified. What results is a square'ish wave at the frequency of the current being measured. 


In the final design, I added an LED to indicate "power-on". This increased the over-all current consumption from 200ua to 1.4ma. 


I have used this project to measure cycle times of water heater, freezer, refrigerator, etc. Any device that you wish to track power consumption over a long duration of time.


This project could easily become an AVR project; a simple envelop detector could be added after the amplifier/integrator stage and then fed to the ADC of an AVR such as a TINY13. The AVR could then use the PWM as a DAC to drive the analog input of the AVR data recorder (or other device, such as a multimeter). The AVR could employ a calibration constant to scale the measured input current and to compensate for the forward voltage drop of the envelope detector diode. If I ever get around to doing this, I'll replace the contents of this project with that design.


Have fun!


Peace,  

Scott

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
