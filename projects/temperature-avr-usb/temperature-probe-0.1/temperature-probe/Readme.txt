This is a simple USB temperature probe that uses the AVR USB library by
Objective Development (http://www.obdev.at/avrusb/).  It is in large part
built upon the EasyLogger example by Christian Starkjohann.  Whereas
EasyLogger would send the data values over a keyboard interface, this project
uses a custom device class and reads values using the ruby-usb library.  As
such, it is primarily intended to be used in Linux, although it is possible to
get it to work in XP with a bit of hassle involved (I was not able to get it
working in Vista, although it may be possible).  The included ruby script
reads the temperature and optionally logs it using RRDTool.

Instructions:

* Compile the code in the firmware directory and upload it to your AVR device:
	$ cd firmware
	$ make
	$ make fuse
	$ make flash

* Build the circuit

* Plug it in to the USB port

* Install the ruby modules from
	http://www.a-k-r.org/ruby-usb/
	http://rubyrrdtool.rubyforge.org/
  You can skip the RRDTool stuff if you remove a few lines from the
  temp_probe.rb script.

* Now you need to use the included temp_probe.rb script to set a device id.
  The device ID is just any integer, it is used to distinguish between devices
  if you choose to plug in several loggers at once.
	$ ./temp_probe.rb --set-id <device_id>

* The device must be calibrated, since the AVR's internal voltage reference
  is not very accurate.  To calibrate, just run the logger program:
	$ ./temp_probe.rb
  Make note of the numbers displayed in the "dn" column.  Use the "dn" value
  along with the real temperature to calibrate the chip:
	$ ./temp_probe.rb --calibrate <device_id> <dn> <temperature>
  The calibration will be stored on the chip's EEPROM.  A good way to find the
  real temperature is to just read the voltage coming out of the LM34 sensor
  (it is the AVR's ADC that needs to be calibrated here, the sensor itself is
  quite accurate).

* Now you can run temp_probe.rb to view temperature, or you can pass the
  name of an RRD file if you want to log.  The RRD file can be created
  using the create-rrd.sh script.
	$ ./create-rrd.sh
	$ ./temp_probe.rb --rrd temperature.rrd

* The included CGI script can be used to view the temperature logs from 
  a web page.
