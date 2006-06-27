# Two-way Infra-red Processor (TWIRP)

Uploaded by stevech on 2006-06-27 03:04:00 (rating 5 out of 5)

## Summary

I did this in 2003. I thought I'd previously uploaded this here, guess not.


An Atmel '2313 chip with an IR emitter and a generic (Radio Shack) IR receiver module ($4). It should port to a newer processor easily.


Serial port connects to a PC.


TWIRP code detects received IR data - and passes it to the PC as either mode (1) a series of pulse widths as delta-time numbers in binary - raw data essentially, so the PC can decode the data in any format, or (2) TWIRP attempts to decode the data and send a hexadecimal code to the PC that is unique to the remote and button on that remote that was pushed.


The PC can "learn" remote buttons using mode (1).


To emit IR, TWIRP takes data from the PC - essentially the same data as in mode (1) and TWIRP pulses the IR diode at 38KHz using that data stream from the PC. 


A tricky part of TWIRP was do do all of this at 19.2Kbaud on the PC serial link. Other products on the market use 115K baud. I wanted 19.2 so that the serial link can flow long distances on cable or across modems and wireless serial extenders and so on that are speed limited.


Included is a PC demo program written in Visual Basic 6 - EXE included. It does the actions I talk about above. It also displays the received data stream on the PC screen in an oscillocope-like manner.


The '2313's code was hard to cram into that chip and live with it's small RAM.


This was a lot of work back in 2003. I was going to publish this in a hobby magazine and make a whopping $300 or so. I didn't, so here it is!


Steve

## Compilers

- CodeVisionAVR-Light

## Tags

- Complete code with HW
- CodeVisionAVR-Light
