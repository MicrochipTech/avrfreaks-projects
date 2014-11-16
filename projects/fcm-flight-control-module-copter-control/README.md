# FCM - Flight control Module - Copter control

Uploaded by fabianhu on 2014-11-15 21:10:00 (rating 5 out of 5)

## Summary

[FCM 2.1 - Flight control Module](https://www.huslik-elektronik.de/cms/embedded-hardware/7-fcm-flight-control-module)


![FCM](https://www.huslik-elektronik.de/cms/images/hardware/fcm_s.jpg)


 


![](https://www.huslik-elektronik.de/cms/images/hardware/fcm2.1s2.png)


**Features:** 


 


* Size & weight:
	+ 45 x 27 x 8 mm
	+ 10 g
* CPU
	+ AVR32 UC3B0256
	+ 32 bit
	+ 48MHz
* Sensors:
	+ Gyroscope (3 axis)
	+ Accelometer (3 axis)
	+ Magnetometer (3 axis)
	+ Barometer
* Interfaces:
	+ 4x PPM input or
	+ Receiver serial input for SPEKTRUM and HoTT or
	+ Receiver PPM sum signal input
	+ 2x extra TTL USART (for Bluetooth, GPS, Telemetry etc.)
	+ 6x PPM output for speed controllers (SimonK firmware recommended)
	+ optocoupler for 5 PPM outputs (placement variant)
	+ Pulse input for ultrasonic distance measurement
	+ USB for configuration and updates
* Software features:
	+ FabOS32 RTOS
	+ Quaternion based attitude control
	+ Altitude hold
	+ Any copter configuration via mixing matrix
	+ RC compatibility
		- HoTT SUMD receiver signal
		- SPEKTRUM sattelite receiver signal
		- 4-channel "any RX" classic mode
		- PPM sum signal
	+ Telemetry
		- HoTT telemetry with settings menue
		- Bluetooth telemetry with settings menue, Attitude and graph display
		- [MAVlink](http://qgroundcontrol.org/mavlink/start) prepared
	+ PID setting via extra RC analog channels
	+ Software is "closed source" = all own code with minor extensions with LGPL
	+ Software is open-sourced to hardware owners. (SVN access)
	+ USB Bootloader
* Being tested:
	+ GPS integration with waypoint processing and return-to-home
	+ Throw to start

The FCM is not yet a finished product and it is not for sale as a readily produced module.  

If you are interested in development or production to make this available to a bigger audience, please feel free ...

## Links

- [github project page](https://github.com/fabianhu/FCM)
- [Project homepage](https://www.huslik-elektronik.de/cms/embedded-hardware/7-fcm-flight-control-module)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
- UC3B0256
