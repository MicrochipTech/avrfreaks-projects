#!/usr/bin/env ruby

require 'usb'
require 'RRD'

class HidData
	USBRQ_HID_GET_REPORT = 1
	USBRQ_HID_SET_REPORT = 9
	USB_HID_REPORT_TYPE_FEATURE = 3

	def initialize(dev_handle)
		@dev_handle = dev_handle
		@timeout = 5000
	end

	def readBlock(len, report_number=0)
		buffer = "\0" * len

		ret = @dev_handle.usb_control_msg(
			USB::USB_TYPE_CLASS | USB::USB_RECIP_DEVICE | USB::USB_ENDPOINT_IN,
			USBRQ_HID_GET_REPORT, 
			USB_HID_REPORT_TYPE_FEATURE << 8 | report_number,
			0, buffer, @timeout)

		if ret != len
			raise "read wrong number of bytes (#{ret} vs. #{len})"
		end

		return buffer
	end

	def writeBlock(buffer, report_number=0)
		len = buffer.length

		ret = @dev_handle.usb_control_msg(
			USB::USB_TYPE_CLASS | USB::USB_RECIP_DEVICE | USB::USB_ENDPOINT_OUT,
			USBRQ_HID_SET_REPORT, 
			USB_HID_REPORT_TYPE_FEATURE << 8 | report_number,
			0, buffer, @timeout)

		if ret != len
			raise "wrote wrong number of bytes (#{ret} vs. #{len})"
		end
	end
end

class TempProbe < HidData
	VENDOR_ID = 0x16c0
	PRODUCT_ID = 0x05dc
	PRODUCT_STRING = 'TemperatureProbe'
	MANUFACTURER_STRING = 'www.stahlke.org'
	STATE_LEN = 8
	CONFIG_LEN = 128

	def initialize(dev_handle)
		super(dev_handle)

		#puts "admux=%02x" % readBlock(1, 1).unpack("C")[0].to_i
		setMux(0x83)
		_readEeprom
	end

	def self.findAll
		usb_devs = USB.devices.select { |d|
			d.idVendor == VENDOR_ID &&
			d.idProduct == PRODUCT_ID &&
			d.product == PRODUCT_STRING &&
			d.manufacturer == MANUFACTURER_STRING
		}
		return usb_devs.collect { |dev|
			h = dev.open
			TempProbe.new(h)
		}
	end

	def _readState
		buffer = readBlock(STATE_LEN, 0)
		#puts "state = " + (buffer.unpack("C*").collect{|v| "%02x" % v}.join(" "))
		(accum, samps) = buffer.unpack('NN')
		@dn = accum.to_f / samps.to_f
		@temperature = @dn * @scale
	end

	def _readEeprom
		buffer = readBlock(CONFIG_LEN, 7)
		#puts "eeprom = " + buffer.unpack("C*").collect{|v| "%02x" % v}.join(" ")
		(@scale, @dev_id) = buffer.unpack('gN')
	end

	def _writeEeprom
		buffer = [@scale, @dev_id].pack("gN")
		buffer += "\0" * (CONFIG_LEN - buffer.length)
		writeBlock(buffer, 7)
	end

	def setMux(mux)
		buffer = [mux].pack('C')
		writeBlock(buffer, 1)
	end

	def calibration=(s)
		@scale = s
		_writeEeprom
	end

	def deviceID
		return @dev_id
	end

	def deviceID=(id)
		@dev_id = id
		_writeEeprom
	end

	def temperature(verbose=false)
		_readState
		if verbose
			puts "id=#{@dev_id} dn=#{"%.4f" % @dn} T=#{"%.4f" % @temperature}"
		end
		return @temperature
	end
end

#####################################

rrd = nil

while !ARGV.empty?
	cmd = ARGV.shift
	if cmd == '--calibrate'
		raise if ARGV.length != 3
		id = ARGV.shift.to_i
		dn = ARGV.shift.to_f
		t = ARGV.shift.to_f

		devs = TempProbe.findAll
		devs = devs.select { |d| d.deviceID == id }
		if devs.length != 1
			raise "found #{devs.length} matching devices - need exactly one"
		end

		devs[0].calibration = (t.to_f / dn.to_f)
		puts "Calibration set."
		exit(0)
	elsif cmd == '--set-id'
		raise if ARGV.length != 1
		id = ARGV.shift.to_i

		devs = TempProbe.findAll
		if devs.length != 1
			raise "found #{devs.length} devices - need exactly one"
		end

		devs[0].deviceID = id
		puts "Identifier set."
		exit(0)
	elsif cmd == '--rrd'
		raise if ARGV.length < 1
		rrd = ARGV.shift
		if !File.writable?(rrd)
			raise "cannot write to file #{rrd}"
		end
		puts "Logging to #{rrd}"
	else
		puts "Usage:"
		puts "  #{$0} --calibrate <device_id> <dn> <temperature>"
		puts "  #{$0} --set-id <device_id>"
		puts "  #{$0} --rrd <temperature.rrd>"
		exit(0)
	end
end

prev_indices = []

while true
	begin
		devs = TempProbe.findAll
	rescue
		puts "Error finding devices: #{$!}"
		sleep(1.0)
		retry
	end

	indices = devs.collect { |d| d.deviceID }
	(prev_indices - indices).each { |s|
		puts "Device id=#{s} unplugged."
	}
	(indices - prev_indices).each { |s|
		puts "Device id=#{s} plugged in."
	}
	prev_indices = indices

	if devs.empty?
		puts 'Found no devices'
		# FIXME - the USB driver doesn't seem to rescan for devices
		# so we have to just quit and make the user run the script again
		exit
	end

	devs.sort_by { |d| d.deviceID }.each { |d|
		begin
			val = d.temperature(true)
			# FIXME - log to proper column of RRD file
			if rrd != nil
				RRD.update('temperature.rrd', ['N', val].join(':'))
			end
		rescue
			puts "Read error for #{d.deviceID}: #{$!}"
		end
	}

	sleep(1.0)
end
