# SCP1000 recording barometer

Uploaded by jremington on 2007-12-03 01:52:00 (rating 0 out of 5)

## Summary

I implemented a recording barometer using the SCP1000 and the DX160 serial graphics LCD, available from Spark Fun. An ATmega8 was used to read out the SCP1000 and format the display.


The DX160 runs on 5V while the SCP1000 runs on 3.3V. To solve the interfacing problem, I decided to run the ATmega8 at 3.3 V and interface it directly to the SCP1000. It turns out that the output level of TXD (slightly less than 3.3 V) is acceptable to the DX160 and I've had no problem with reliability of the communications.


A photo of the complete project is appended below. The graph displays the last 128 samples held in a circular buffer (about two days at 20 minutes/sample) and is plotted on a scale of about 1 milliBar/pixel. This covers the 2 sigma range for typical weather conditions (+/- 20 milliBar). Pressure data are stored in the circular buffer as integers (deviation from 1 ATM = 1013.25 mB) in units of 0.1 mB. The current pressure trend is calculated by linear regression from the last 20 minutes, at 1 sample/minute, and is displayed in Pa/hr. The number at bottom right reports the number of minutes until the display and pressure trend are updated.


The sharp dip to the right of the graph occurred about 1 hour before a fairly severe storm hit the area.


The display software could use a lot of tweaking, such as axis ticks, autoscaling of the displayed values, display of 1 ATM, etc., so improvements are welcome!

## Links

- [Photo of SCP1000 recording barometer display (JPG)](http://www.uoxray.uoregon.edu/orangutan/scp1000_baro.jpg)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
