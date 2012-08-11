# Door Controller

Uploaded by dhahaj on 2012-08-10 19:00:00 (rating 0 out of 5)

## Summary

This software is designed to control an electronic door latch and door operator for both single and dual door setups. It can be configured to operate in dependent/independent mode, active high/low outputs, and output toggle. Delay times are set via two multiplexed digital switches. The basic operation is as follows: A signal input triggers a latch retract output signal to the door, where a small delay allows enough time for the motor to finish retracting before sending another signal to the door operator to begin opening the door. This can be performed either dependently where both doors would activate concurrently, or independently where the two inputs activate each door individually. 


This is essentially my first embedded design so any tips or improvements are welcome!

## Compilers

- AVR Studio 5

## Tags

- Complete code
- AVR Studio 5
