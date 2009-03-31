# Gamecube Controller Interface

Uploaded by coachphil5 on 2009-03-31 04:31:00 (rating 0 out of 5)

## Summary

This is a short routine to interface a ATMega8 with a gamecube controller. It uses some ASM to achieve proper timing. I relied heavily on the work done at [http://www.int03.co.uk/crema/har...](http://www.int03.co.uk/crema/hardware/gamecube/gc-control.html). The site is well written and easy to understand. Although I don't have a good use for the software yet, the controller does do ADC conversion and debounce work for you. The only downside is that the analog joy sticks do not go all the way from 0 to 255. Usually goes from about 30..225. Any suggestions on how to improve the code is appreciated.


Update: Found wireless controller from Psyclon (sold @ Target in USA) that has full range (0..255) on ADC channels on joysticks as well as on upper left and right buttons, PLUS its WIRELESS! One other note I needed an external 10K pull-up resistor on the data line for the wireless controller to work. The new file GameCubeAsm is a complete ASM sub-routine to include in a project. It reduced the total program bytes requried to about 150 and the data bytes to 33. The ASM routine is much faster and pack the data bits into the 8 bytes.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
