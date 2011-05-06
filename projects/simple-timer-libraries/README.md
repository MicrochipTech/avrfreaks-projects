# Simple Timer Libraries

Uploaded by tigrezno on 2011-05-05 18:56:00 (rating 0 out of 5)

## Summary

Timer0.h, Timer1.h and Timer2.h are three small libraries to allow system timers to run a specific function at your desired period.


All them work equal:


void timerX(prescaler, top, loops, function)


You set the prescaler, the top value (OCRXA), the number of loops that top value will be reached, and the function to execute.


An example on a cpu running at 16Mhz (arduino for example):


timer0(TIMER0\_PRESCALER\_64, 250, 10, myfunction);


"myfunction" will be called every 10 milliseconds, according to prescaler, top and loops values.


timer freq = 16000000/64 = 250000  

period = 1/250000 = 4 us  

setting top to 250 we get: 250 x 4us = 1000us or 1 millisecond  

setting loops to 10 we get 10 milliseconds


"myfunction" must be defined void with empty arguments:


void function() {  

....  

}


The web is bugged and I can't upload project files in firefox...

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
