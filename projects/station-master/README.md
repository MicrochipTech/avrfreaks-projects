# Station Master

Uploaded by NickLott on 2007-07-21 20:29:00 (rating 0 out of 5)

## Summary

The Station Master is a small key ring sized alarm that uses an accelerometer to track your movement. The device will gently vibrate and let you know when you've reached your stop and it’s time to disembark from the train. Using a simple one button interface the user can set an alarm for the right number of stops and then be reminded when its time to get off. 


The ATtiny13 was selected for this design due to its low cost and suitable array of features. An MMA7260 3-Axis Accelerometer from Freescale is used to monitor the tilt and movement of the device. A simple cell phone’s alarm motor is used to provide a discrete silent alarm. These motors can be found very cheap at electronic surplus stores. 


Due to the high computational needs of integrating acceleration data and calculating actual speed and distance in real-time this initial approach was quickly discarded in favour of a simpler design. After analysing data from many train journeys (using a data-logger and accelerometer) I was able to develop a simple algorithm to establish whether the train was moving or stationary. It was important that this algorithm could work with only 64 Bytes of RAM available on the ATtiny13; the final implementation used only 6 Bytes of RAM. By programming in assembler I was able to pack plenty of code into the flash and still have room for a 256-byte sine wave table. The sine wave is used to give the alarm a really natural look and feel when using either an LED or motor for output. 


Although this design worked for my own daily commute its main filter would likely need to be re-tuned for other train systems. The best application for this device would be to incorporate it into a cell phone or portable music device (especially if it already uses an accelerometer for HDD protection) that way existing system resources could be used to implement a learning function, or more accurate stop detection as well as reducing the number of gadgets you need to carry. Imagine an MP3 player that fades out the music and announces your stop at the appropriate time! 


[http://www.brokentoaster.com/sta...](http://www.brokentoaster.com/stationmaster/)

## Links

- [Project homepage](http://brokentoaster.com/stationmaster/)

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
