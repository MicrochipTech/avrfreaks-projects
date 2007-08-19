# Butterfly freq-duty on LCD

Uploaded by sveinutne on 2007-08-19 07:58:00 (rating 0 out of 5)

## Summary

The frequency will go up or down with the joystick moving up or down. The duty cycle will change by moving the joystick to the left or right. If you push in the joystick it will start over with 50% duty cycle. The output will go to PB5, that is also the Pizo element, so you can hear the signal too.  

The LCD will give some info and show the number used for period or pulswith.  

Later I might change the display to % of duty cycle and frequency in Hz.  

PB0 can be used as a trigger interrupt, and will reset the counter till 0. This can be used as a signal from a hallprobe, to start the counting, and give power to a coil after the duty cycle count max value and stop the power at TOP.  

Ideas for improvments are welcome.  

Some bug fix on August 19. 2007

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
