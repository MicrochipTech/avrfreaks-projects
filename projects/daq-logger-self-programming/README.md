# DAQ Logger with self-programming

Uploaded by visakhanc on 2012-03-06 11:04:00 (rating 5 out of 5)

## Summary

 Data aquisition and logging of a number of analog sources can be done using this project. Here two quantities are logged: Light intensity and temperature. LDR is used for light measurement, LM35 centigrade temperature sensor is used for temperature measurement.


 Logging is done at interval of 1 minute (can be changed). ADC of the AVR device take samples at 5 second intervals and average of the 12 samples is stored to a temporary buffer(=Page size of AVR) in SRAM. When the buffer is full, it is written to a flash page using self programming function.


 Time keeping is done by asynchronous RTC timer of AVR with a 32768Hz watch crystal. This helps to reduce power consumption since the AVR can be set to sleep mode when there is no CPU activity. AVR is woken up at every second (Timer interrupt) and performs necessary operations (time keeping, ADC, or page write).

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
