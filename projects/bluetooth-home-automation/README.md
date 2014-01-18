# bluetooth home automation!

Uploaded by costy35 on 2014-01-18 10:00:00 (rating 0 out of 5)

## Summary

My project is composed of several parts... programs,electronics... The most important piece is bluetooth module ,this receive data from any bluetooth device(phone,pc,etc)and send to atmega8 microcontroller. Principle is simple: A bluetooth device send and character ("A" or "B" or "C" ,etc) and microcontroller read this data an action. Is very simple! The code for atmega8 first part: Bitwait Pind.0 , Reset Waitus 150 If Pind.0 = 1 Then A.0 = 1 Waitus 100 If Pind.0 = 1 Then A.1 = 1 Waitus 100 If Pind.0 = 1 Then A.2 = 1 Waitus 100 If Pind.0 = 1 Then A.3 = 1 Waitus 100 If Pind.0 = 1 Then A.4 = 1 Waitus 100 Read directly impulses from bluetooth module (tx from bluetooth connect to PD0 from atmega8)...i'm used this variant to understand how functioning serial data... For 9600 bauds (1/9600=0,00010416666666666666666666666666667 second... bytes have a space of approx. 100 microseconds. The data start with a bit of start , 8 bits of data ,parity bit and stop bit... For read first four bits of data should jump first bit (bit of start... 100us) and half byte of data (50us). In this mode you can get data in personal mode. Of course you can get data with Inkey() or Ischarwaiting() but this method makes you understand more things about serial data... Second part: If A = 1 Then Portb.0 = Not Portb.0 change state of port every time is accessed... For control varius consumers like lamps,motors,tv,dvd,etc you can use an led+photoresistor+triac. Is many types of triacs like tic206 -4ampers,tic126 -12ampers, etc For example : for tic206 you can control a consumer with 4 ampers... this means 220volt X 4 ampers = 880 Watt ... This means that you can control with this triac 8 lamps by 100watts !!! Visit [https://sites.google.com/site/at...](https://sites.google.com/site/atmega8pro/) for another details... and my ultimate video with this simple principle, no library, no additional data,non complicated ! <http://youtu.be/K6pqQxlyN34>

## Compilers

- BASCOM-AVR

## Tags

- Complete code
- BASCOM-AVR
