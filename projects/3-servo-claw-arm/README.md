# 3 servo claw arm

Uploaded by clranc on 2016-01-06 16:48:00 (rating 0 out of 5)

## Summary


```

This is an AVR-C project that permits the user control of a claw arm 
operated by multiple DC servos through analog and digital input for the 
atmega328p. The servos managing left, right, up, and down motion are controlled
via an analog stick while the claw is changed from being opened and closed via
pressing a button. 

```
 


The motion managing servos are controlled on the PWM signals generated by compares use of the TCNT0 counter 


while the claw servo is managed by the PWM signal controlled by the compare for the TCNT1 counter.  The TCNT0 


increments every 1024 cycles while the TCNT1 increments every 256 cycles.  This is to prevent the counters from 


sending OVF interrupts at the same time when updating their next duty cycle values.

## Links

- [github project code](https://github.com/clranc/3_servo_arm)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
