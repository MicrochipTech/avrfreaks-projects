# 3 phase SPWM

Uploaded by electronic.designer on 2013-09-19 09:22:00 (rating 0 out of 5)

## Summary

This project generates 3 SPWM outputs on OC1A,OC1B,OC1C with 120 degrees of phase difference. An analog voltage connected to ADC0 (PF0) controls the output frequency of all phases. The output frequency range is 0.5-100Hz with 0.1Hz step and 15.6KHz of carrier frequency,so there are 996 different frequencies. There is an average mechanism for ADC result calculation(64 samples). For ADC average results less than 14, the outputs are not active and forced to be in low level. PWM resolution is 8 bit and mega64 oscillator frequency is 8MHz. 


SPWM3\_1:  

V/f curve was added for AC motor control applications. Boost and Base frequencies determine curve shape.


SPWM3\_2:  

Acceleration-deceleration was added.


SPWM3\_3:  

Adjustable Acceleration-Deceleration by ADC1 and ADC2 analog inputs.  

ADC1 sets acceleration (1s-255s).  

ADC2 sets decceleration (1s-255s).  

Adjusted values are times for 100Hz change in frequency.


October 4, 2012 :  

Timer3 mode changed due to problems in frequency change.  

Modification in amplitude calculation.


------------------------------------------------------------------------  

September 18, 2013:  

------------------------------------------------------------------------


Run,Stop and Direction push buttons added.  

Run, Dir1 and Dir2 leds added.  

Fault input activated.  

V/f curve changed.  

Boost voltage can be set by ADC3 (9.8%-21.9%).


------------------------------------------------------------------------

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code
- CodeVisionAVR-Standard
