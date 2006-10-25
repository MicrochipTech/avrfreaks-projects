# ALPINE M-BUS COMMUNICATION : Audio Input activation

Uploaded by exterieur on 2006-10-25 07:40:00 (rating 0 out of 5)

## Summary

- Alpine M-BUS is an 1-wire bus and half duplex transmssion. The line coding is unusual. It’s none of the textbook codes, instead some kind of pulse width modulation. A short low pulse (about 0.6 ms) is a binary zero, a longer pulse (about 1.8 ms) is a one. The distance between the pulses remains constant, about 3 ms. The resulting baud rate is about 325 baud, fairly slow.  

- Audio Input of autoradio headunit can only be activated if headunit received an acknoledge of equipment link on headunit by M-BUS.  

- It's also necessary to implement this response frame to activate audio input. Because of M-Bus caracteristics, i should use a PWM to generate frames on M-BUS.


I choose the Atmel ATTiny26 to realize this project.  

With this controller, i can devellop more functionnalities for others applications linked with auto-radio head unit (Mp3 player directly drive with headunit for exemple)


To realize my project, i've buy a low cost product made by Oregon University and their AVR-ISP (For LPT port).  

You can find this product description here :  

[http://eecs.oregonstate.edu/tekb...](http://eecs.oregonstate.edu/tekbots/tiny26.0/) for Tiny26 basic circuit.  

and  

[http://eecs.oregonstate.edu/tekb...](http://eecs.oregonstate.edu/tekbots/avr-isp/) for AVR-ISP programmer circuit.


I've able to program my Tiny26 the first time I try.  

This little circuit is handy for develloppment phase of a projet because µC part is already operational...


For programming my Tiny26 I use AnyEdit open source editor (A french have made a special package for AVR on AnyEdit -> <http://anyedit.free.fr/> ... but it's in french langage... I'm french...good for me ;-) )


25th October 2006  

The new source code is fully operational on my head unit, the older source code doesn't implement fully M-BUS protocol and Audio Input was not activated. But now, it's ok with this new source code.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
