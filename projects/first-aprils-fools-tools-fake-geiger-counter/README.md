# First aprils Fools Tools: Fake Geiger counter

Uploaded by daqq on 2006-07-20 15:57:00 (rating 0 out of 5)

## Summary

WARNING: This is a potentialy panic creating device! Do not make fun with it in places it is not apropriate (like nuclear power plants, labs, where they work with radioactive stuff, when you go check out if someone hasn't got any nuclear weapons, etc.)


The Geiger counter (GC from now on), also called a Geiger-Müller counter, measures ionizing radiation. For those of you not familiar with it, go watch a movie about atomic catastrophies, it's the thing that makes the TIK-TIK-TTTTIK sound. More about real counters can be found on Wikipedia, or just do some Googling. Normally, the instrument amplifies a signal from a Geiger Muller tube and displays it to the user, either as a current measurement (needle, lamp) or an audible click, with one click for each single particle. The more clicks, the higher radiation. GC are used everywhere, where radiation measurment is neccesary (nuclear power plant, maybe X-rays, labs, etc.).  

A high radiation level is a bad thing. Most people know GCs from movies. They know, that a low amount of clicks is good. When a higher amount of clicks is detected, the main hero gets nervous and says something in the meaning of "Oh shit! This is not good.". The purpose of this Fools tool is to make a clicky sound and let the user intensify it at his command, so an unsuspecting victim will freak out.  

The function is simple: you turn the device on by S3. It beggins to click in a non-alarming tone. By holding (for about 1sec) the TURBO key, the clicking starts to intensify. You don't need to hold it longer, it will intensify automatically. The equivalent of the maximum intensity on the Fake GC would, in a real GC, mean "Were done for.". When your victims have suffered enough, press the NORMAL key, which will return the clicking to a "safe" level. The whole device consists of a Tiny2313, with an active internal RC oscillator (factory settings), two buttons, one switch, two caps, and a voltage reg. (78L05. NOTE: If you wanna go from a battery source which gives 5.5V or less (to about 3V) you can skip the regulator, and connect the battery directly to +VCC), and a piezo. It is placed in a simple box, I will leave the creativity on you.


Look it up at daqq.ic.cz

## Links

- [The complete project folder.](http://daqq.ic.cz/download/funny_gm.zip)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
