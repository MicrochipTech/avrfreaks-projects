HighResTimer
------------
is a timer component for high resolution time measurement and wait routines.

(c) 2002 by Roman Lauer

The component uses a Win32 multimedia timer and a high resolution counter 
for timing. Using the high resolution counter only for a wait routine adds a 
lot of CPU load. Therefor this component uses a multimedia timer for long time 
timing. As soon the remaining time to wait is smaller than the accuracy of the 
multimedia timer, the component switches to the high resolution counter. This 
reduces the CPU load to a minimum still achieving high accuracy. 
The high resolution timer can be selected between the time stamp counter 
introduced with the Pentium processor or the high performance counter of 
Windwos. The availability of the time stamp counter is tested during the 
creation of the component.

The resolution and accuracy is determined by the OS (see limitations) and the 
CPU load. Therefor setting the timers thread priority and the applications 
thread priority to appropriate levels can help to get higher accuracy.

File list:
 - HighResTimer.pas		Component Source code
 - HighResTimer.dcr		Component symbol for IDE installation
 - Readme.txt			This file
 - HRTimerTest.dpr		Project file for example test program
 - HRTimerTest.res		Resource file for example test program
 - uHRTimerTest.dfm		Form file for example test program
 - uHRTimerTest.pas		Source file fro example test program

Usage:
  Just drop the component on the form. On every create of the component an 
  auto calibration is done. This calibration is needed to determine the count
  speed. For higher accuracy of the count speed the calibration can be repeated 
  any time. As longer the calibration time, as higher the accuracy of the count 
  speed (applies only for time stamp counter). 
Properties:
  CountSpeed: read only, frequency for counter in MHz.
  Enabled: Set to True to activate the timer.
  MinAccuracy: read only, minimum possible accuracy of the used multimedia 
               timer in the used system.
  MinResolution: read only, minimum resolution of the used multimedia timer.
  Accuracy: Accuracy in ms for the multimedia timer to use. As higher the 
            value as lower is the CPU load added by the multimedia timer. A 
            value of 0 sets highest possible accuracy.
  Resolution: Resolution of the multimedia timer.
  Thread Class, ThreadPriority: Priority levels for the timing thread of the 
            component. 
  TSC: read only, Set to true if time stamp counter is available,
  UseTSC: Used to manualy switch between time stamp counter and high 
            performance counter;
Methods:
  Caibrate(calibration time in milliseconds):
            Calculates the counter speed. Can be repeated at any time.
  GetCounterValue:
            Returns the Value of the used counter.
  GetCounterTicks(time in micro seconds)
            Calculates the number of ticks needed for the given time.
  Wait(time in micro seconds)
            Suspends the calling thread until the given time interval is 
            expired. Like the Sleep() function but higher resolution and 
            accuracy possible.
  StartTimeMeasure:
            Starts an internal time measurement.
  StopTimeMeasure:
            Stops an internal time measurement.
  GetTimeDifference:
            Calculates the time in micro seconds that elapsed between a 
            StartTimeMeasurement and a StopTimeMeasurement call. 

Limitations:
- The minimum resolutions and accuracy are determined by the used operating 
  system. According to MSDN Win9x can achieve an resolution of 1 ms but the
  minimum accuracy is only 5ms. On NT systems the minimum accuracy is 1ms.

Comments:
  Component is tested from the author only with D5 on an AMD K6 450Mhz with 
  Win98.
