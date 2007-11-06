# RC5 Decoder

Uploaded by kenjewell on 2007-11-05 18:59:00 (rating 0 out of 5)

## Summary

This project was designed to test Phillips RC5 remote controls. The result is displayed shown on a 2x16 character LCD display. 


The Atmel ATMega16L was used for this project although it can easily fit in a smaller micro.


There is not much to the hardware design, heres a list of ports used.


PORT A - PA0 thru PA7 – LCD module DB0 – DB7 respective  

PORT B – PB0 – LCD module RS pin.  

PORT B – PB1 – LCD module R/W pin.  

PORT B – PB2 – LCD module E pin.  

PORT D – PD2 – Demodulated IR Data Input (INT0).  

PORT D – PD6 – LCD module backlight via NPN transistor (optional) the backlight will flash as the RC5 code is being recieved.


IR receiver part – HIM702VM (Farnell 414-2846)


Crystal Frequency: 8MHz 


Supply Voltage: 5V 


Although data is displayed on a LCD module, the code can easily be adapted to drive other outputs such as relays, LEDs or drive a Triac for lamp dimming control etc...

## Tags

- Complete code
