### Quick script to automate the execution of all unit tests
### using FunkProgMono to load the test code, and FunkTermMono
### to read the test results - all on real hardware connected
### to the device.

### Note - this test assumes we're talking to an arduino UNO
### running on linux with mono installed

import pexpect

# Set the environment variables for the test platform
platform  = "avr"
cpu       = "atmega328p"
toolchain = "gcc"
stage	= "./stage"

# Programming/serial interface used to communicate with 
device	= "/dev/ttyACM0"
baud	= "57600"

# List of unit tests to run
test_list = ["ut_logic", "ut_thread", "ut_semaphore", "ut_mutex", "ut_eventflag", "ut_heap", "ut_message", "ut_timers"]

# Run each test in succession
for test in test_list:
	load_cmd = "mono ./util/FunkProgMono.exe %s/app/%s/%s/%s/%s.hex %s %s" % (stage, platform, cpu, toolchain, test, device, baud)
	print "--[Loading Binary for %s]--" % test
	child = pexpect.spawn( load_cmd )
	child.expect( pexpect.EOF, timeout=240 )

	# Build the commandline used to run the tests
	test_cmd = "mono ./util/FunkTermMono.exe %s %s ASCII" % (device, baud)
	print "--[Running Test: %s]--" % test 
	child = pexpect.spawn( test_cmd )
	index = child.expect (["--DONE--","(FAIL)", pexpect.EOF, pexpect.TIMEOUT], timeout=240)
	if index == 0:
		print "		(PASS)"
	elif index == 1:
		print "		(FAIL)"
	elif index == 2:
		print "		(Premature EOF - did test crash?!)"
		break
	elif index == 3:
		print "		(Test timed out!)"
		break
	child.terminate()


