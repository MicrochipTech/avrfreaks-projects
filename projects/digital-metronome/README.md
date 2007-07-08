# Digital Metronome

Uploaded by RaceDriver205 on 2007-07-08 07:46:00 (rating 0 out of 5)

## Summary

Uses ATTiny13 to implement a basic digital metronome. It allows tempos of 5 to 295 beats per minute, and time signatures of 0 to 9 beats per bar. It uses a single 7 segment LED display to indicate values. Two push buttons allow for changing of the tempo and time signature, and any piezo buzzer can be used for the sound output (even a 105dB unit). The decimal point of the 7SD also provides a tempo indication.  

Accuracy was measured to 1.001Hz at 60bpm, using the calibrated internal oscillator only. As ATTiny13 ICs do not have consistent internal oscillators, you will need to calibrate yours in the manner stated in the C file. Using a digital multimeter with frequency output, and setting beats per bar to 0 is one very efficient way.


PS. used all 1024 out of 1024 bytes program memory!

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
