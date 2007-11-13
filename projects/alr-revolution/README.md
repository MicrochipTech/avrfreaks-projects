# ALR REVOLUTION

Uploaded by jim116 on 2007-11-13 09:34:00 (rating 0 out of 5)

## Summary

The purpose of the device  

This computer modding device allows to collect several very useful devices in one. In about four times bigger then normal Midi Tower server case there are two independent computers (standard ATX motherboards, RAID array, two ATX power supplies and other peripherals), audio-video switch, radio tuner, audio signal spectrum and level analyzer, power amplifier, cooling system that consists of the six twelve centimeters fans with adjustable air velocity, measurement of motherboards main parameters, also it is possible to start up main systems at the appointed time. All data are displayed on two 128x64 Graphical LCDs. Management is conducted with the help of a 8-button keyboard. 


Key features of the device  

Volume, balance, equalizer [TDA9859].  

4 AV inputs/outputs and 3 Audio inputs commutation [TDA9859/MAX4618].  

Spectrum (mono) and level (stereo) analyzer.  

Built-in radio: 6 presets, search function [TEA5757H].  

Power independent clock and calendar. Starting of any of the two computers on schedule [DS1307].  

The analysis and display of the data from two computers.  

Data collected from LPT port (CPU load (%), memory load (%), SWAP file (%), network activity (kB/s)). And directly (not through LPT) measurement of the temperature (1 sensor is connected (up to 8 maximum) ) [DS1621] and the HDDs' activity.  

Fan's speed management (PWM).  

Management of Power On/Off and Reset of two computers.  

Password protected start-up of the main systems with floating code. When you enter an incorrect password time before the next attempt is growing exponentially. There is a possibility of changing the password.  

Touch startup and automatic move up of the LCD and Keyboard unit.  

Automatic shutdown of the main processor after ATX power off and automatic move down of the LCD and Keyboard unit.  

The structure of the electric scheme  

Electric scheme is made using three processors : ATmega32, ATmega16, ATtiny2313. 


ATmega32 (operating at a frequency of 18,432 MHz) has the main control functions: master of the ATmega16 (TWI Protocol), controls sound and volume, switching audio-video inputs (TDA9859 - TWI Protocol), a clock/calendar/alarm clock (DS1307 - TWI Protocol), checking of the temperature (DS1621 - TWI Protocol), and activity of the HDDs, the spectrum analyzer and level of the audio signal, passwords input, showing data on two GLCDs, backlighting and control keyboard and display. 


ATmega16 (operating at a 8 MHz frequency of built-in generator) functions are: management of the radio, fan rotation speed, data from the two LPT ports, management Power On/Off and Reset of the two computers, STANDBY function of the power amplifier. 


ATtiny2313 (operating at a 4 MHz frequency of built-in generator) functions are: monitoring servomotor, survey of the sensors unit indicators and keyboards, beep when you are clicking buttons (3 kHz signal generation) and operations performed by mechanics, power on/off of the main scheme ATmega32/16, communication with the ATmega32 to confirm permission to turn off the power, a volume sensor management, voltage control of the two ATX power supplies of the computers. 


This electrical scheme is optically isolated, also this device has an independent power supply. Optically isolated are: LPT ports, PWM control of the fans, management of the Power On/Off and Reset, data of the HDDs activity and +5V port at the ATX power supplies. 


Read full article on my Home page  

[http://magaavr.narod.ru/cpu/inde...](http://magaavr.narod.ru/cpu/indexe.html)

## Links

- [cpu](http://magaavr.narod.ru/cpu/indexe.html)

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
