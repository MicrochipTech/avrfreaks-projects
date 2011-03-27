# Fireworks Launching System

Uploaded by admin on 2011-03-27 06:21:00 (rating 0 out of 5)

## Summary

Running around with torches is dangerous.  

Using a laptop from 100 feet away from a "box" with computer chips inside wired to 32+ spring speaker terminals with talon igniters connected to 32 legal, local, consumer fireworks placed into 32+ HDPE mortor tubes in a strong custom built carrying/launching base seems safer.  

I'll be using an Atmega 644P because that's what I have on stock.  

I grabbed a $20 board from olimex already pcb'd with a 12v rectifier so I can pull the 12v off for the ULN2003A's to have a higher voltage drop.


I grabbed some HEF4514BP decoders that are active high to match up with the active high on the ULN2003A's.  

Also to increase power transfer I'll be stacking 2 DIP 2003A's on top of each other (in parallel) quite literally to double effectiveness and speed of firing and reduce the breaking of chips since fire time will be less.


This solution allows for only firing 2 fireworks at a time. I think if I stacked 3 2003A's it would fire a lot quicker but I'd have to do some testing and it also depends on who made your igniters so...2 is all I care about right now. 


There is no wireless (yet), that might require more safety features. This is pretty no frills, no key system for physical off switch. If you want it to turn off...unplug it.


I think that about covers it, I may modify this if there are questions. Feel free to request information, I just kinda winged this.


I think that covers the hardware...if you need a schematic ask, this is pretty simple standard wiring. All the chips were pretty well documented but I'll post a schematic later if I get any requests.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
