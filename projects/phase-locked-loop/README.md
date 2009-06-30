# Phase Locked Loop

Uploaded by dcriawan on 2009-06-30 03:08:00 (rating 0 out of 5)

## Summary

This project is aimed to provide synchronization signal which follows phase angle of grid voltage. The output of PLL would be in phase with grid voltage following zero crossing of the waveform. The output of PLL could be set to 8-bit or 10-bit PWM signal (OC1A). Pulse width of PWM output varies along with network voltage phase angle. This is very useful feature e.g. when designing a current control for VSI.


How it works?  

1. Attenuated network voltage is fed into analog comparator to generate zero crossing interrupt. At the same time timer0 is counting for it "period" (period = how many time an overflow occurs in timer0 during one full crossing interrupt).  

2. This "period" is then used to determine instantaneous value of index "i" that corresponds (relatively) to actual phase angle of network voltage.  

3. Value of "i" is then used as index of 2048 points sine waveform array in pre-determined lookup table.  

4. The final step is assigning Timer1 with array\_sine[i]. As the result, pulse width of PWM signal coressponds to the phase angle of network voltage (synchronized).  

5. Extra feature, this pll is capable in scaling the modulation index. This is aimed to have "controllable" power flow in current control VSI application.


Note:  

Manual adjustment of lookup table is needed due to time delay in uC. In actual test, 2-3% delay occurs when no adjustment is made (based on 50Hz network).  

Speed is crucial in this application.  

Anyway, how can I upload file in this website? I tried to upload zip format but it failed. Just blank screen showing up.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
