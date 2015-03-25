# ILI9341 library to drive 2.2" tft display(derived from adafruit tft library for ili9340 type controller)

Uploaded by SHARANYADAS on 2015-03-25 11:14:00 (rating 4 out of 5)

## Summary

I was designing a lab power supply.During it i thought to display "colourful" data....lolz.So i bought a 2.2" TFT display which is using ili9341 as it's heart.There is no complete library available for avr family microcontrollers.So i thought to derive a library from adafruit's oriinal library for arduino to drive TFTs.After 3 days of hard work,i was able to port the library for avr family.It has also the function to display strings.(which was a little bit difficult to write as it was not included in the library as arduino has inbuilt print function).I put enough comments by which anyone can completely understand the functios.i used atmega8@8MHz for test purpose and it is running quite well and display updating speed is not bad at all.take a look if u have any use of it.The "main" is just used here to test.Modify it accordingly to your needs.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
- ILI9340/ILI9341
- Digital Cameras
- TFT LCD
