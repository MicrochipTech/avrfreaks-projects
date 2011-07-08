# PS/2 mouse led with tiny13

Uploaded by vulicevic on 2011-07-07 22:32:00 (rating 0 out of 5)

## Summary

Turn on/off led inside PS/2 mouse on command from host. Communication is only from host to avr using valid mouse commands, so I recommend to repeat command multiple times.  

Basically, you have to set mouse sample rate to 10 or 20 samples/sec to shift in 0 or 1, respectively. When MSB becomes 1 command is completely shifted in and switch() decides what to do. Obviously, you can extend this scheme to 128 commands, which is more than enough.  

Currently, I am using this as new mail notification. shell script checks mail and sends command to mouse when needed by echo-ing '10' or '20' to /sys/bus/serio/devices/serio1/rate. If you know how to do this in systems other than linux, please give me some hint.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
