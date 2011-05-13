# Thermostat with ATtiny13

Uploaded by falat on 2011-05-13 10:37:00 (rating 0 out of 5)

## Summary

The current temperature is displayed on 2x7-segment display. There are two temperature values which are possible to change: Ton - the temperature at which the output is switched on (off) and Toff - the temperature at which the output is switched off (on). The appropriate values are set via two buttons. Button 1 sets the mode of programming in sequence(current temperature -> Ton mode set(indicated on display as "on") -> Ton value -> Toff mode set (indicated on display as "oF") -> Toff value -> current temperature. Button 2 works only if the Ton or Toff value is displayed. The value is increased when the button is pressed. In case the value reaches 99 it starts from 0 again. After switching to the next mode the last displayed value is saved into internal EEPROM.  

In case you have any question please mail to [admin@falcom.sk](mailto:admin@falcom.sk)  

<http://www.falcom.sk>

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
