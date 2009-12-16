# 8 outputs pwm + servo

Uploaded by ldrolez on 2009-12-16 07:52:00 (rating 0 out of 5)

## Summary

Features:  

- RS232 Interface (from 9600 to 57600 bauds)  

- 8 PWM or Servo motor outputs, 255 steps, 20 us resolution,  

- 5.1 ms period in PWM mode, 20.4 ms period in Servo mode  

- 5 General purpose Input / Outputs (20mA sink)  

- User definable board ID number (up to 224 devices). '0' by default  

- Power supply: 4.0V to 6.0V 


Ports Used:  

PORTB 0-7 8 PWM / Servo outputs  

PORTD 0 Uart receive  

PORTD 1 Uart send  

PORTD 2-6 5 GPIO


Serial Commands: (x = device ID, hh = 8 bits hexadecimal value)  

xPn hh : Set PWM #n value to 'hh' (0=always 0, 1=20us pulse... 255=always 1)  

xCP hh : Set PWM/Servo mode bits (0=Servo, 1=PWM) default=FF  

xCD hh : Set PORTD direction bits (0=input, 1=ouput) default=00  

xI : Get PORTD bits (returns "hh\n")  

xO hh : Set PORTD bits to 'hh'


**More info and GCC source code:** (adding urls below does not work)  

<http://www.drolez.com/hardware/>

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
