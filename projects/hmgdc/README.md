# HMGDC

Uploaded by SQ9MDD on 2011-09-25 19:04:00 (rating 0 out of 5)

## Summary

DTMF Controler


The controller is designed to control the "on-off" using the built-in  

relays, configured in the "NC" or "NO" with jumpers. It consists of a primary DC1 driver plate (3 lines) and the extension plate DCE1 (another 3 lines).


Commands are issued using DTMF tones. Control functions are secured by individual pin code of the device, which can be changed at any time of use.  

The format control code: *- pin-code-line number command, where the pin code is four digits (default  

2222). Line number is a number from 1 to 6 Control command is "1" or "0", which corresponds to the "turn" or "off".


Examples:  

* 222211 - turn the machine on line 1  

* 222210 - disable the device on line 1  

* 222212 - reset the device on the line 1 (30 sec.)


This special command line 9, or all lines simultaneously:  

* 222291 - Turn the device on all lines  

* 222290 - disable the device on all lines  

* 222292 - reset devices on all lines (30 sec.)


Device PIN code can be changed by the command: #-old-new pin code pin code example:  

# 22221111, change the default PIN code of 1111


All project files(source):  

[http://home.safeland.org/~rysiek...](http://home.safeland.org/~rysiek/dcdce/)


YouTube presentation:  

[http://www.youtube.com/watch?v=\_...](http://www.youtube.com/watch?v=_bMdbXwAIQc)

## Compilers

- BASCOM-AVR

## Tags

- Complete code with HW
- BASCOM-AVR
