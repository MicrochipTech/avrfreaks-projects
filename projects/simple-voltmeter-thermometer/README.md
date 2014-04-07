# SIMPLE VOLTMETER +THERMOMETER

Uploaded by SHARANYADAS on 2014-04-06 18:38:00 (rating 0 out of 5)

## Summary

This is my first post here.I used to work with 8051 in assembly before and made various projects during last several years like GSM based load control,Code lock with eeprom,motion detector,remote control etc etc.I am an telecom engineer by profession and microcontroller is purely my hobby.I am also very much new with avr family and c programming.So please rectify any errors in coding.Also shortcut is appreciated. This circuits is tested and working 100% in real time. This circuit shows voltage from 0-15v dc range and also shows room temprature on an lcd.Two channels of inbuilt adc of mega16 are used to measure voltage and temprature.A simple 10k pot is used as a resistor divider network and LM35 is used as temparature sensor.16x2 lcd is used here.ADC is in single conversion mode.Software filter is used to filter ADC output.Here AREF is connected with AVCC.But in realtime i didn't connected AREF to AVCC.Simply left AREF open.If the 5v power supply is noisy,then simply connect two 100nf/104pf decoupling caps to AREF and AVCC to gnd as referred to ATMEL datasheet just without the inductor/with inductor . proteus screenshot is shown here as the circuit diagram.Please connect vcc and gnd pins of micro to 5v supply(which is not shown here)before real time implementation.The probe+ and probe- will be the testing probes.After powering up for the first time,a calibration is needed.Connect the probes to 5v power supply of the circuit and tune it until 5v is shown on LCD as voltage output.No need to calibrate for temparature.It will appear autometically on LCD. AGAIN I'M SAYING..IF THE POWER SUPPLY IS NOISY,PLEASE CONNECT DECOUPLING CAPS TO AREF AND AVCC TO GND.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
