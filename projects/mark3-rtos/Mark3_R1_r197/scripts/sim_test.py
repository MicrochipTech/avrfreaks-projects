### Quick script to automate the execution of all unit tests
### By running the tests on the flavr simulator
import pexpect

# Set the environment variables for the test platform
platform  = "avr"
cpu       = "atmega328p"
toolchain = "gcc"
stage	= "./stage"
# List of unit tests to run
test_list = ["ut_logic", "ut_thread", "ut_semaphore", "ut_mutex", "ut_eventflag", "ut_heap", "ut_message", "ut_timers"]

# Run each test in succession
for test in test_list:
	# Build the commandline used to run the tests
	test_cmd = "flavr --exitreset --silent --elffile %s/app/%s/%s/%s/%s.elf" % (stage, platform, cpu, toolchain, test)
	child = pexpect.spawn( test_cmd )
	print "--[Running Test: %s]--" % test 
	index = child.expect (["--DONE--","(FAIL)", pexpect.EOF, pexpect.TIMEOUT], timeout=240)
	if index == 0:
		print "		(PASS)"
	elif index == 1:
		print "		(FAIL)"
	elif index == 2:
		print "		(EOF!)"
		print child.before
	elif index == 3:
		print "		(FAIL -- TIMEOUT)"
		break

