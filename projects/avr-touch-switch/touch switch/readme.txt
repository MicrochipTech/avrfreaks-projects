This is a project I did last year. Just added the schematic & changed license today (& wrote up this overly worded readme ;)) to make it an easier project to follow.

It's really simple, just add a single 1M resistor to any AVR part & you have a touch switch! It's a bit power hungry as is, the touch sense needs the CPU running near 10mhz and, apart from the sleep between checks & dynamic clocking, I haven't really looked into seeing if that can be improved -- lost the interest to do so when I saw atmel bought quantum, figured I'd see touch on the AVRs pretty soon.

For the touch pad, I connected a wire from the touch pad to a small piece of foil stuck on the back of a sheet of plastic (stick it there with glue, tape, or whatever). No critical materials here (wire length, resistor value, voltage, touch panel thickness) so tolerances can be wide. Also you can use whatever material you like for the touch panel; part of a CD case, a marble block, wood...of course the sensitivity (stability?) is different with each.

If you want to look into the code, the part for the touch switch is in the main for(;;) loop of the main() section, and the timer0 overflow ISR. The detection is actually done in the for loop. The ISR is just used to time the press for tap, double tap & hold detection. Sensitivity is adjusted with the four constants at the top, TS_BACKLOG, MAF_LOOPS & THRESH; I tweaked these for my particular application, but I can't remember right now their particular effects; anyway you can play with them & see.

The rest of the code is related to other stuff; this particular implementation was a remote control for a dimmable lamp, so there's IR transmit stuff, as well as a serial port thing (just to debug). I left it all in there to give a hint as to how I used it, so you can just change the commands called in the switch in TIM0_OVF_vect to make it do your bidding :)

Some TODOs that I never got to doing:
- further shrink/speed up the code (haven't really looked into it but I think some of the stuff is redundant)
- make 2 touch sensors with the single resistor (using both PB3 & PB4 since the resistor is wired between them)
- turn this into a function, so it can be added easily in an include & called from the user app every once in a while
- reduce CPU/power usage (might require different HW, maybe a future AVR that doesn't mind less power on the inputs?)

Feel free to drop me a line if you make any improvements, find it useful, etc.
Muhammad Jamil Galadima
mjag17@gmail.com
