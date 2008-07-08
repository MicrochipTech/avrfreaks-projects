# V/F & SVP demo on ATAVRMC200

Uploaded by RogerHuang on 2008-07-08 07:01:00 (rating 0 out of 5)

## Summary

July-08, 08 updated: The EVB has UART connector which has to drive DTR & RTS MODEN control lines. I did not find any Linux utility to drive these pins, so written the small code. i.g. I used ttyS0 to connect EVB  

./setuart dtr -rts < /dev/ttyS0 


Original version is for IAR, but I just have GNU avr-gcc. So modified they for me and publish here. Note some things as follow


1. Original code is front-back ground model, and all PWM execute on back-ground, then the PWM performance is base on main loop which now is 2Khz that is ADC converter speed.


2. For planning implement PWM on PSC interrupt possible, measurement critical PWM assign parts by scope on PC7. Got 324uS for V/F, and 67uS for SVP. It's seem hard to implement implement they both on ISR

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
