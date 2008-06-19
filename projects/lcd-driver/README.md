# LCD-Driver

Uploaded by artasquer on 2008-06-18 18:34:00 (rating 0 out of 5)

## Summary

You can add a lcd to your project without worrying about how could impact it in your code... doesn't add any active waiting (delays)


Features  

- Do not use delay functions  

- Minimal impact on your firmware (use only 3 inst on one timer ISR)  

- Double buffer  

- Refresh time configurable


Use  

- One ISR timer clocked to 500usec(could be changed to more or less)  

- A LCD init function (at startup, with the ISR disabled)  

- A screen refresh function on the main loop.


Version 0.6 (6-5-2008)  

- First public release


Version 0.7 (15-5-2008)  

- Added more functions (flash strings, better putnumber)  

- Added soft blinking


more info : <http://www.tasquer.com/arte/>


Support PIC/ATMEL MCU (avr-gcc, hitech picc and microchip c18)

## Links

- [lcd-driver-0.7](http://www.tasquer.com/arte/upload/lcd-driver-0.7.rar)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Part-specific Lib.functions
- WinAVR (GNU GCC)
