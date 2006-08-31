# Project Dexter Part 1: SerialSherry

Uploaded by cyclidox on 2006-08-30 20:28:00 (rating 0 out of 5)

## Summary

Newbie(becoming ISH) discovery project: This project has much broader scope than the description that follows.


Alas! I have little time to indulge and there is no Radioshack here. I originally wanted to use ATRF211's (I have not soldered anything yet - ever so those 0603's are how can I say scary) but I am busy looking at Bluechip RFB433 blocks atm as they seem simpler. To follow a ordered path, I have set the following at my Hello project. To determine the relative position of a transmitter w.r.t the receiver (distance and direction). I am not sure if I will need triangulation to sort this out but thats ok too. It is probably going to come down to signal strength for distance and whatever for position. Looking at 300m radius for now (but would have liked 5km more). I can go on forever. Lemme dump into a file rather...


In any case, (did I mention I used an ATMega 8?) I have included the the Serial port C# source code that I adapted (detailed below) along with the AVRStudio project.


The original C# (.Net 2) code was written by Noah Coad (<http://coad.net/noah>). I found the Flying Electron USART Library ([www.flyingelectron.com](http://www.flyingelectron.com)) on the web and used it (as-is if I can recall) for the USART part.


I run the serial port at 115200 and I have found that if I transmit relatively large text files, the echoed data loses some characters. So I implemented a buffer to send the data in pieces rather than using the .Net 2.0 oneliner. The original serial port code was integrated with the windows form so I took it all out and created a SerialSherry class on which the port code runs on a seperate thread.


On the SerialSherry class there are two constants that can be set, the one is the buffer size and the other is a sleep time in milliseconds. For my implementation (via a STK 500 so - as far as I could establish - high clock speeds could not be set up. I have 10.245Mhz crystals which I set up on the STK 500 but this did not seem to really help (even after playing with the dippies and fuses in AVRStudio). In any case, I found that bytes start disappearing at sending around 64 bytes at a time. I set it at 16 to make relatively sure (caveat: at my newbie thumb-suck standards!) that all bytes are accounted for.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
