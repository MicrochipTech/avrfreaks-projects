; Open GPS Tracker firmware
; Version 0.17A 2008/05/09
; Mike Ingle (email: mike at opengpstracker dot org)
; http://www.opengpstracker.org
; This is open source software.
;
; If you are upgrading from build 0.14, you must change the phone configuration:
; Message/Options/Memory Meter/Select SMS Memory/Phone First
;
; For ATTINY84 AVR, Tyco A1035 GPS, and Motorola C168i
; Set MCU to run at 1 MHz (CKDIV8 fuse)

; Commands:
;
; SETSPEED
;   speed limit or 0 for none - units
;   minimum speed to accept as moving - tenths of a unit
;   minimum speed to accept as moving on first try - tenths of a unit

; SETTRACK
;   stopped fix interval - seconds
;   stopped notify delay - intervals
;   blocked fix interval - seconds
;   blocked notify delay - intervals
;   moving fix interval - seconds
;   moving notify frequency - intervals
;   minimum displacement for MOVED alert

; SETPOWER
;   powersave phone off interval - seconds
;   powersave phone on interval - seconds
;   four-sat fix wait time (before accepting three-sat fix) - seconds
;   fix wait time (before giving up and reporting no fix) - seconds
;   blocked fix wait time (shorter fix wait when no signal available) - seconds

; SETNAME tracker-name
; SETPASSWORD newpassword newpassword
; SETADDRESS phone-number optional-email-address
; SALOCATE phone-number optional-email-address
; LOCATE
; STATUS
; TRACKON
; TRACKOFF
; POWERSAVE
; POWERON
; REBOOT
; REINIT
;
; Blink codes:
; 11 = phone polled
; 12 = send message failed
; 13 = phone poll failed
; 14 = no phone number defined
; 21 = invalid password
; 31 = power on or reset
; 32 = watchdog reset
; 33 = EEPROM initialized from defaults
; 34 = remove jumper to reinitialize EEPROM

 .include "tn84def.inc"

; Must define ONE and ONLY ONE of SPEED_IN_KPH, SPEED_IN_MPH, or SPEED_IN_KNOTS
;#define SPEED_IN_KPH = 1
#define SPEED_IN_MPH = 1
;#define SPEED_IN_KNOTS = 1

; Must define ONE and ONLY ONE of ALT_IN_FEET or ALT_IN_METERS
#define ALT_IN_FEET = 1
;#define ALT_IN_METERS = 1

; Serial debug will copy phone interaction to TX_OUT_TO_DEBUG
;#define SERIAL_DEBUG 1

; Debug message count will keep track of messages sent and output in status page
;#define DEBUG_MESSAGE_COUNT_ENABLE 1

; Keeps invalid incoming messages in the phone. This disables reading of REC READ
; folder. It is only for debugging. I put it here to track down a message bounce problem.
;#define DEBUG_KEEP_INVALID_MESSAGES 1

; Add error codes to WDR_COUNT for send message failures.
;#define DEBUG_SEND_MESSAGES 1

; Reset phone after every PHONE_RESET_INTERVAL interactions and for power on
; Required for C168i and GoPhone service
#define RESET_PHONE_PERIODICALLY 1

.equ	BAUD_RATE_COUNTER = 208 ; 4800 baud
.equ	INIT_RECEIVE_TIMEOUT = 8 ; short timeout for ATE0 (2 seconds)
.equ	DEFAULT_RECEIVE_TIMEOUT = 20 ; timeout for most commands (5 seconds)
.equ	SMS_SEND_RECEIVE_TIMEOUT = 120 ; timeout for SMS SEND (24 seconds)
.equ	DEFAULT_PHONE_POLL_INTERVAL = 900 ; default phone poll interval (not tracking) in seconds (15 minutes)
.equ	DEFAULT_FOUR_SAT_WAIT_TIME = 18 ; 72 seconds (x4)
.equ	DEFAULT_GPS_FIX_WAIT_TIME = 34 ; 136 seconds (x4)
.equ	DEFAULT_BLOCKED_GPS_FIX_WAIT_TIME = 24 ; 96 seconds
.equ	DEFAULT_SPEED_LIMIT = 0 ; disabled
.equ	DEFAULT_TRACK_STOPPED_FIX_INTERVAL = 120 ; two minutes
.equ	DEFAULT_TRACK_STOPPED_NOTIFY_DELAY = 2 ; 2 poll intervals
.equ	DEFAULT_TRACK_BLOCKED_FIX_INTERVAL = 600 ; ten minutes
.equ	DEFAULT_TRACK_BLOCKED_NOTIFY_DELAY = 2 ; 2 poll intervals
.equ	DEFAULT_TRACK_MOVING_FIX_INTERVAL = 120 ; two minutes
.equ	DEFAULT_TRACK_MOVING_NOTIFY_FREQ = 5 ; 5 poll intervals
.equ	DEFAULT_POWERSAVE_PHONE_OFF_INTERVAL = 3600 ; one hour
.equ	DEFAULT_POWERSAVE_PHONE_ON_INTERVAL = 600 ; 10 minutes
.equ	DEFAULT_TRACK_MIN_DISPLACEMENT = 200 ; 0.2 minute of lat/lon
.equ	DEFAULT_GPS_MOVING_THRESHOLD = 20 ; 2 or more units reported = moving
.equ	DEFAULT_GPS_MOVING_RETRY_THRESHOLD = 100 ; if under 10 units, double check speed
.equ	GPS_DATASTREAM_WAIT_TIME = 60 ; 15 seconds
.equ	GPS_LINES_AFTER_FIX = 10 ; how many more lines to wait for after fix valid
.equ	PHONE_RESET_INTERVAL = 100 ; reset phone every N polls, after message received
.equ	PHONE_RESET_WAKEUP_DELAY = 45 ; time in seconds to wait for the phone to power on
.equ	PHONE_MAX_CHECKSUM_LENGTH = 160 ; largest message phone will store

; Units conversion factors (from HP50G calculator)
.equ	CF_KNOT_MPH = 37709 ; KNOT * 2 before conversion
.equ	CF_KNOT_KPH = 60686 ; KNOT * 2 before conversion
.equ	CF_M_FT = 53753 ; M * 4 before conversion

.equ	TX_OUT_TO_PHONE	= PORTA1
.equ	RX_IN_FROM_PHONE = PORTA0
.equ	RX_IN_FROM_GPS = PORTA2
.equ	TX_OUT_TO_DEBUG = PORTA7
.equ	LED_OUT_RED = PORTB0
.equ	LED_OUT_GREEN = PORTB1
.equ	GPS_POWER_OUT = PORTB2
.equ	REINIT_CHECK_IN = PORTA4
.equ	REINIT_CHECK_OUT = PORTA6
.equ	MAX_PHONE_ERRORS = 5 ; AT command retries

.def	FLAG_REGISTER = r16
.equ	FLAG_SERIAL_SEND = 0 ; set during serial send
.equ	FLAG_SERIAL_RECEIVE = 1 ; set during serial receive; clear to detect unsolicited message
.equ	FLAG_PARSE_NMEA = 2 ; set to put PARSE_LINE in NMEA mode
.equ	FLAG_SKIP_REPEATED_DELIMITER = 3 ; flag used in PARSE_LINE
.equ	FLAG_GOT_RECEIVED_MESSAGE_NUMBER = 4 ; indicates +CMGL captured
.equ	FLAG_NEED_TO_POLL_MESSAGES = 5 ; got CMTI, interrupt, etc.
.equ	FLAG_PHONE_FULL = 6 ; need to clear space in phone
.equ	FLAG_MESSAGE_SENT = 7 ; message sent

.def	REG_ZERO = r0 ; register always zero; saves a lot of clr instructions
.def	SERIAL_PORT = r1 ; Set bit corresponding to I/O line you want to use.
; Note: if more than one bit is set, in output mode, you can send to multiple output
; lines at once. This is used for the debug output.
.def	SERIAL_DATA = r2 ; Shift register for data byte.
.def	SERIAL_COUNT = r17 ; Counts down bits sent/received.
.def	SERIAL_POINTER = r3 ; Pointer into 256-byte serial receive buffer.
.def	SERIAL_SCRATCH = r4 ; Scratch register used by serial I/O
.def	SERIAL_INTERRUPT_SREG = r5 ; Storage for Status flags during serial interrupt
.def	TIMER_INTERRUPT_SREG = r6 ; Storage for Status flags during timer interrupt
.def	DELAY = r23 ; this register is counted down by an interrupt 4x/sec, stopping at zero
.def	RETRY_COUNT = r9 ; error count for serial operations
; Parser register assignments
; Y has pointer into buffer
; T flag is error 
.def	CHAR = r18 ; current character
.def	PAT = r7 ; current pattern character
.def	BASE = r8 ; base of string being matched
.def	CHECKSUM = r10
.def	DELIM = r11

; 26-27, 28-29, 30-31 are pointers

.def	SCRATCH0 = r19
.def	SCRATCH1 = r20
.def	SCRATCH2 = r21
.def	SCRATCH3 = r22
.def	READ_FIELD = r19
.def	PARSE_FIELD = r20
.def	FIELD_LENGTH = r21
.def	VERIFY_CHECKSUM = r12
#ifdef	SERIAL_DEBUG
.def	DEBUG_POINTER = r13
#endif

; Status codes returned by GPS_LOCATE
.equ	GPS_STATUS_DATASTREAM_TIMEOUT = 0
.equ	GPS_STATUS_FIX_TIMEOUT = 1
.equ	GPS_STATUS_FIX_STOPPED = 2
.equ	GPS_STATUS_FIX_MOVING = 3

; Tracker state values
.equ	TRACK_STATE_OFF = 0
.equ	TRACK_STATE_BLOCKED = 1
.equ	TRACK_STATE_STOPPED = 2
.equ	TRACK_STATE_MOVING = 3
.equ	TRACK_STATE_INITIAL = 4
.equ	TRACK_STATE_POWERSAVE = 5 ; lowest powersave value
.equ	TRACK_STATE_POWERSAVE_PHONE_OFF = 5
.equ	TRACK_STATE_POWERSAVE_PHONE_ON = 6

; Decode modes
.equ	DECODE_SETTING_TWO_BYTE = 7
.equ	DECODE_SETTING_ONE_BYTE = 6
.equ	DECODE_SETTING_ONE_BYTE_X4 = 5



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; EEPROM addresses
; All the short one and two byte settings are first, so we can use single-byte addressing
; and save space in the program. Assumptions all through the program that settings
; are in the first 256 bytes of EEPROM. Long fields at the end may extend into higher
; pages of EEPROM. EEPROM is never written except for first-time initialization and
; in response to user commands.
.equ	EEADR_GPS_FOUR_SAT_WAIT_TIME = 0
.equ	EELEN_GPS_FOUR_SAT_WAIT_TIME = 1 ; four seconds per count
.equ	EEADR_GPS_FIX_WAIT_TIME = EEADR_GPS_FOUR_SAT_WAIT_TIME + EELEN_GPS_FOUR_SAT_WAIT_TIME
.equ	EELEN_GPS_FIX_WAIT_TIME = 1 ; four seconds per count
.equ	EEADR_BLOCKED_GPS_FIX_WAIT_TIME = EEADR_GPS_FIX_WAIT_TIME + EELEN_GPS_FIX_WAIT_TIME
.equ	EELEN_BLOCKED_GPS_FIX_WAIT_TIME = 1 ; four seconds per count
.equ	EEADR_SPEED_LIMIT = EEADR_BLOCKED_GPS_FIX_WAIT_TIME + \
                            EELEN_BLOCKED_GPS_FIX_WAIT_TIME
.equ	EELEN_SPEED_LIMIT = 1 ; in selected units
.equ	EEADR_GPS_MOVING_THRESHOLD = EEADR_SPEED_LIMIT + \
                                     EELEN_SPEED_LIMIT
.equ	EELEN_GPS_MOVING_THRESHOLD = 1 ; in selected units * 10
.equ	EEADR_GPS_MOVING_RETRY_THRESHOLD = EEADR_GPS_MOVING_THRESHOLD + \
                                           EELEN_GPS_MOVING_THRESHOLD
.equ	EELEN_GPS_MOVING_RETRY_THRESHOLD = 1 ; in selected units * 10
.equ	EEADR_TRACK_STOPPED_FIX_INTERVAL = EEADR_GPS_MOVING_RETRY_THRESHOLD + \
                                           EELEN_GPS_MOVING_RETRY_THRESHOLD
.equ	EELEN_TRACK_STOPPED_FIX_INTERVAL = 2 ; how often to get GPS fix when stopped
.equ	EEADR_TRACK_STOPPED_NOTIFY_DELAY = EEADR_TRACK_STOPPED_FIX_INTERVAL + \
                                           EELEN_TRACK_STOPPED_FIX_INTERVAL
.equ	EELEN_TRACK_STOPPED_NOTIFY_DELAY = 1 ; how many intervals after stopped to notify and use
                                             ; stopped-mode fix interval
.equ	EEADR_TRACK_BLOCKED_FIX_INTERVAL = EEADR_TRACK_STOPPED_NOTIFY_DELAY + \
                                           EELEN_TRACK_STOPPED_NOTIFY_DELAY
.equ	EELEN_TRACK_BLOCKED_FIX_INTERVAL = 2 ; how often to attempt GPS fix when blocked
.equ	EEADR_TRACK_BLOCKED_NOTIFY_DELAY = EEADR_TRACK_BLOCKED_FIX_INTERVAL + \
                                           EELEN_TRACK_BLOCKED_FIX_INTERVAL
.equ	EELEN_TRACK_BLOCKED_NOTIFY_DELAY = 1 ; how many intervals after blocked to notify and use 
                                             ; blocked-mode fix interval
.equ	EEADR_TRACK_MOVING_FIX_INTERVAL = EEADR_TRACK_BLOCKED_NOTIFY_DELAY + \
                                          EELEN_TRACK_BLOCKED_NOTIFY_DELAY
.equ	EELEN_TRACK_MOVING_FIX_INTERVAL = 2    ; how often to get GPS fix when moving
.equ	EEADR_TRACK_MOVING_NOTIFY_FREQ  = EEADR_TRACK_MOVING_FIX_INTERVAL + \
                                          EELEN_TRACK_MOVING_FIX_INTERVAL
.equ	EELEN_TRACK_MOVING_NOTIFY_FREQ = 1 ; notify every N intervals while moving
.equ	EEADR_POWERSAVE_PHONE_OFF_INTERVAL = EEADR_TRACK_MOVING_NOTIFY_FREQ + \
                                             EELEN_TRACK_MOVING_NOTIFY_FREQ
.equ	EELEN_POWERSAVE_PHONE_OFF_INTERVAL = 2 ; time in seconds to leave phone off in powerdown
.equ	EEADR_POWERSAVE_PHONE_ON_INTERVAL = EEADR_POWERSAVE_PHONE_OFF_INTERVAL + \
                                            EELEN_POWERSAVE_PHONE_OFF_INTERVAL
.equ	EELEN_POWERSAVE_PHONE_ON_INTERVAL = 2 ; time in seconds to leave phone on in powerdown

.equ	EEADR_TRACK_MIN_DISPLACEMENT = EEADR_POWERSAVE_PHONE_ON_INTERVAL + \
                            	 EELEN_POWERSAVE_PHONE_ON_INTERVAL
.equ	EELEN_TRACK_MIN_DISPLACEMENT = 2 ; displacement to consider MOVED, in minutes*100 of lat/lon
.equ	EEADR_PHONE_NUMBER = EEADR_TRACK_MIN_DISPLACEMENT + \
                             EELEN_TRACK_MIN_DISPLACEMENT
.equ	EELEN_PHONE_NUMBER = 32
.equ	EEADR_EMAIL_ADDR = EEADR_PHONE_NUMBER + EELEN_PHONE_NUMBER
.equ	EELEN_EMAIL_ADDR = 64
.equ	EEADR_PASSWORD = EEADR_EMAIL_ADDR + EELEN_EMAIL_ADDR
.equ	EELEN_PASSWORD = 16
.equ	EEADR_DEVICENAME = EEADR_PASSWORD + EELEN_PASSWORD
.equ	EELEN_DEVICENAME = 16
.equ	EEADR_CHECKCODE = 510
.equ	EELEN_CHECKCODE = 2


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; SRAM addresses
.dseg
CMGL_SAFETY_COUNT: .byte 1 ; at beginning of buffer so wild pointer does not step on it
WDR_COUNT: .byte 1 ; count of watchdog resets
SEND_CHECKSUM: .byte 1 ; checksum value for sending
SEND_CHECKSUM_COUNTER: .byte 1 ; count of characters checksummed
SEND_CHECKSUM_BYTES: .byte 1 ; captured count at end of line
READ_CHECKSUM_BYTES: .byte 1 ; received message length for verification
#ifdef DEBUG_MESSAGE_COUNT_ENABLE
DEBUG_MESSAGE_COUNT: .byte 2 ; count of messages sent
#endif
#ifdef RESET_PHONE_PERIODICALLY
PHONE_RESET_COUNT: .byte 1
#endif
#ifdef DEBUG_KEEP_INVALID_MESSAGES
DEBUG_INVALID_MESSAGE_FLAG: .byte 1
#endif
SERIAL_SEND_DELAY: .byte 1 ; number of bit times per byte for serial, min is 11
RECEIVE_TIMEOUT: .byte 1 ; delay for receiving data
PHONE_STRING: .byte 2
SEND_STRING_OFFSET_0: .byte 1
SEND_STRING_OFFSET_1: .byte 1
CURRENT_SMS: .byte 4
COMMAND_PENDING: .byte 2
GPS_TIMER: .byte 1 ; counts up if reset to zero, one count per 4 seconds, stops at 0xff
GPS_STATUS: .byte 1
SUBSECONDS_COUNT: .byte 1
BLINK_CODES: .byte 8
BLINK_CODES_INDEX: .byte 1 ; High-order halfbyte is INSERT pointer.  Low-order is SHOW pointer.
TRACK_STATE: .byte 1
TRACK_COUNT: .byte 1 ; counts up when GPS_STATUS != TRACK_STATE until state change
CHECKSUM_RETRY_COUNT: .byte 1
MOVING_LOCATE_COUNT: .byte 1 ; counts up to notify while moving
POLL_INTERVAL: .byte 2 ; delay between phone/GPS polls in seconds
FASTEST_SPEED: .byte 2 ; fastest speed above speed limit during this tracking session
.equ	LAST_GOOD_FIX_LEN = 27 ; FLDLEN_GPRMC_TIME + FLDLEN_GPRMC_LATITUDE + \
;                            FLDLEN_GPRMC_LATDIR + FLDLEN_GPRMC_LONGITUDE + \
;                            FLDLEN_GPRMC_LONGDIR 
LAST_GOOD_FIX: .byte LAST_GOOD_FIX_LEN 

; Parse fields are used for parsing strings, and are reused for commands, 
; settings, and NMEA strings
.equ	PARSE_FIELDS_LENGTH = 64
PARSE_FIELDS: .byte PARSE_FIELDS_LENGTH
;DEBUG_LAST_BYTE: .byte 1

; Receive buffer is 256 bytes at 0x100
; Receive pointer is one byte and is allowed to roll over to create circular buffer
.org 0x0100
RECEIVE_BUFFER: .byte	256

; Offsets into LAST_GOOD_FIX buffer
.equ	LAST_GOOD_FIX_TIME = LAST_GOOD_FIX
.equ	LAST_GOOD_FIX_LATITUDE = LAST_GOOD_FIX + 6
.equ	LAST_GOOD_FIX_LATDIR = LAST_GOOD_FIX_LATITUDE + 9
.equ	LAST_GOOD_FIX_LONGITUDE = LAST_GOOD_FIX_LATDIR + 1
.equ	LAST_GOOD_FIX_LONGDIR = LAST_GOOD_FIX_LONGITUDE + 10

; Field for parsing DEVICENAME
.equ	FLDADR_DEVICENAME = PARSE_FIELDS + 0
.equ	FLDLEN_DEVICENAME = 16

; Field for parsing CMGL
.equ	FLDADR_CMGL_UNSENT = PARSE_FIELDS + 0
.equ	FLDADR_CMGL_SENT = PARSE_FIELDS + 32
.equ	FLDADR_CMGL_STOP = PARSE_FIELDS + 56
.equ	FLDLEN_CMGL_MAXMSGS = 8 ; actually limits it to 7 of each

; Fields for parsing SETPASSWORD
.equ	FLDADR_PASSWORD1 = PARSE_FIELDS + 0
.equ	FLDLEN_PASSWORD1 = 16
.equ	FLDADR_PASSWORD2 = PARSE_FIELDS + 16
.equ	FLDLEN_PASSWORD2 = 16
.equ	MIN_PASSWORD_LENGTH = 3

; Fields for parsing GPS output
.equ	FLDADR_GPRMC_FIXVALID = PARSE_FIELDS + 0
.equ	FLDLEN_GPRMC_FIXVALID = 1
.equ	FLDADR_GPRMC_TIME = FLDADR_GPRMC_FIXVALID + FLDLEN_GPRMC_FIXVALID
.equ	FLDLEN_GPRMC_TIME = 6
.equ	FLDADR_GPRMC_LATITUDE = FLDADR_GPRMC_TIME + FLDLEN_GPRMC_TIME
.equ	FLDLEN_GPRMC_LATITUDE = 9
.equ	FLDADR_GPRMC_LATDIR = FLDADR_GPRMC_LATITUDE + FLDLEN_GPRMC_LATITUDE
.equ	FLDLEN_GPRMC_LATDIR = 1
.equ	FLDADR_GPRMC_LONGITUDE = FLDADR_GPRMC_LATDIR + FLDLEN_GPRMC_LATDIR
.equ	FLDLEN_GPRMC_LONGITUDE = 10
.equ	FLDADR_GPRMC_LONGDIR = FLDADR_GPRMC_LONGITUDE + FLDLEN_GPRMC_LONGITUDE
.equ	FLDLEN_GPRMC_LONGDIR = 1
.equ	FLDADR_GPRMC_SPEED = FLDADR_GPRMC_LONGDIR + FLDLEN_GPRMC_LONGDIR
.equ	FLDLEN_GPRMC_SPEED = 6
.equ	FLDADR_GPRMC_COURSE = FLDADR_GPRMC_SPEED + FLDLEN_GPRMC_SPEED
.equ	FLDLEN_GPRMC_COURSE = 6
.equ	FLDADR_GPRMC_DATE = FLDADR_GPRMC_COURSE + FLDLEN_GPRMC_COURSE
.equ	FLDLEN_GPRMC_DATE = 6
.equ	FLDADR_GPGGA_FIXVALID = FLDADR_GPRMC_DATE + FLDLEN_GPRMC_DATE
.equ	FLDLEN_GPGGA_FIXVALID = 1
.equ	FLDADR_GPGGA_SATS = FLDADR_GPGGA_FIXVALID + FLDLEN_GPGGA_FIXVALID
.equ	FLDLEN_GPGGA_SATS = 2
.equ	FLDADR_GPGGA_ALT = FLDADR_GPGGA_SATS + FLDLEN_GPGGA_SATS
.equ	FLDLEN_GPGGA_ALT = 5 ; Too long?
.equ	FLDADR_GPRMC_SPEED_NUMERIC = FLDADR_GPGGA_ALT + FLDLEN_GPGGA_ALT
.equ	FLDLEN_GPRMC_SPEED_NUMERIC = 2
.equ	FLDADR_GPGGA_ALT_NUMERIC = FLDADR_GPRMC_SPEED_NUMERIC + FLDLEN_GPRMC_SPEED_NUMERIC
.equ	FLDLEN_GPGGA_ALT_NUMERIC = 2
.equ	FLDADR_GPS_LINE_COUNT_FLAGS = FLDADR_GPGGA_ALT_NUMERIC + \
                                      FLDLEN_GPGGA_ALT_NUMERIC 
.equ	FLDLEN_GPS_LINE_COUNT_FLAGS = 1
.equ	GPS_FLAG_LOW_SPEED_RETRY = 7
.equ	GPS_FLAG_DISPLACED = 6

; fields for status message
.equ	STATUSMSG_SPEED_LIMIT = PARSE_FIELDS + 0
.equ	STATUSMSG_GPS_MOVING_THRESHOLD = PARSE_FIELDS + 2
.equ	STATUSMSG_GPS_MOVING_RETRY_THRESHOLD = PARSE_FIELDS + 4
.equ	STATUSMSG_TRACK_STOPPED_FIX_INTERVAL = PARSE_FIELDS + 6
.equ	STATUSMSG_TRACK_STOPPED_NOTIFY_DELAY = PARSE_FIELDS + 8
.equ	STATUSMSG_TRACK_BLOCKED_FIX_INTERVAL = PARSE_FIELDS + 10
.equ	STATUSMSG_TRACK_BLOCKED_NOTIFY_DELAY = PARSE_FIELDS + 12
.equ	STATUSMSG_TRACK_MOVING_FIX_INTERVAL = PARSE_FIELDS + 14
.equ	STATUSMSG_TRACK_MOVING_NOTIFY_FREQ = PARSE_FIELDS + 16
.equ	STATUSMSG_MIN_DISPLACEMENT = PARSE_FIELDS + 18
.equ	STATUSMSG_POWERSAVE_PHONE_OFF_INTERVAL = PARSE_FIELDS + 20
.equ	STATUSMSG_POWERSAVE_PHONE_ON_INTERVAL = PARSE_FIELDS + 22
.equ	STATUSMSG_GPS_FOUR_SAT_WAIT_TIME = PARSE_FIELDS + 24
.equ	STATUSMSG_GPS_FIX_WAIT_TIME = PARSE_FIELDS + 26
.equ	STATUSMSG_BLOCKED_GPS_FIX_WAIT_TIME = PARSE_FIELDS + 28
.equ	STATUSMSG_WDR_COUNT = PARSE_FIELDS + 30
.equ	STATUSMSG_BATTERY_CHARGE = PARSE_FIELDS + 32
.equ	STATUSMSG_SIGNAL_STRENGTH = PARSE_FIELDS + 34

; Fields for CPMS
.equ	FLDADR_CPMS_USED = PARSE_FIELDS + 8
.equ	FLDADR_CPMS_TOTAL = PARSE_FIELDS + 16
.equ	FLDADR_CPMS_USED_NUMERIC = PARSE_FIELDS + 24
.equ	FLDADR_CPMS_TOTAL_NUMERIC = PARSE_FIELDS + 32


.cseg
; Interrupt vectors
	rjmp	V_RESET
	rjmp	V_INT0
	rjmp	V_PCINT0
	rjmp	V_PCINT1
	rjmp	V_WDT
	rjmp	V_TIM1_CAPT
	rjmp	V_TIM1_COMPA
	rjmp	V_TIM1_COMPB
	rjmp	V_TIM1_OVF
	rjmp	V_TIM0_COMPA
	rjmp	V_TIM0_COMPB
	rjmp	V_TIM0_OVF
; Watchdog reset on stray interrupts
V_INT0:
V_PCINT1:
V_TIM1_CAPT:
V_TIM1_COMPB:
V_TIM1_OVF:
V_TIM0_COMPA:
V_TIM0_COMPB:
	cli
	rjmp	V_INT0
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; Main program entry point
V_RESET:
; Stack pointer
	ldi	SCRATCH0,high(RAMEND)
	ldi	SCRATCH1,low(RAMEND)
	out	SPH,SCRATCH0
	out	SPL,SCRATCH1
; Init registers
	clr	REG_ZERO ; will stay zero
	clr	FLAG_REGISTER
	out	EECR,REG_ZERO ; EEPM1 and EEPM0 should be clear for atomic write
	sts	TRACK_STATE,REG_ZERO
	sts	COMMAND_PENDING,REG_ZERO 
	sts	COMMAND_PENDING+1,REG_ZERO
	sts	BLINK_CODES_INDEX,REG_ZERO
; Setup for serial I/O
	ldi	SCRATCH0,0xff-((1<<RX_IN_FROM_PHONE)+(1<<RX_IN_FROM_GPS)) ; serial out idles high, and turn on pull-ups on unused lines to reduce power
	out	PORTA,SCRATCH0
#ifdef	SERIAL_DEBUG
	ldi	SCRATCH0,(1<<TX_OUT_TO_PHONE)|(1<<REINIT_CHECK_OUT)|(1<<TX_OUT_TO_DEBUG)
#else
	ldi	SCRATCH0,(1<<TX_OUT_TO_PHONE)|(1<<REINIT_CHECK_OUT)
#endif
	out	DDRA,SCRATCH0 ; set to output
;
	ldi	SCRATCH0,0xff-((1<<LED_OUT_RED)+(1<<LED_OUT_GREEN)) ; turn on pull-ups to reduce power in standby
	out	PORTB,SCRATCH0 ; make sure GPS power is off
	ldi	SCRATCH0,(1<<LED_OUT_RED)|(1<<LED_OUT_GREEN)|(1<<GPS_POWER_OUT)
	out	DDRB,SCRATCH0 ; then enable outputs
;
	sbi	TIMSK1,OCIE1A ; enable interrupt A as the serial timer
	in	SCRATCH0,GIMSK
	ori	SCRATCH0,1<<PCIE0 ; enable Pin Change Interrupt globally
	out	GIMSK,SCRATCH0
; Enable watchdog timer with 8-second timeout (assuming Safety Level 1)
	ldi	SCRATCH0,(1<<WDP3)+(1<<WDP0)+(1<<WDE)
	wdr
	out	WDTCSR,SCRATCH0
; Activate Timer0 for clock interrupt
	ldi	SCRATCH0,(1<<CS02)+(1<<CS00) ; prescaler / 1024
	out	TCCR0B,SCRATCH0 ; 4X per second for 1MHz
	ldi	SCRATCH0,1<<TOIE0 ; overflow interrupt
	out	TIMSK0,SCRATCH0 ; enabled
; Set baud rate
	ldi	SCRATCH0,high(BAUD_RATE_COUNTER)
	ldi	SCRATCH1,low(BAUD_RATE_COUNTER)
	out	OCR1AH,SCRATCH0
	out	OCR1AL,SCRATCH1
; Set the serial timing
	ldi	SCRATCH0,12
	sts	SERIAL_SEND_DELAY,SCRATCH0
; Low power settings
	sbi	ACSR,ACD ; analog comparator off
; Check the EEPROM data and, if missing, load from the defaults file
	ldi	XH,high(EEADR_CHECKCODE)
	ldi	XL,low(EEADR_CHECKCODE)
	rcall	EEPROM_READWORD_X_TO_SCRATCH10
	cpi	SCRATCH1,'M'
	brne	INIT_EEPROM_BAD ; not already configured
	cpi	SCRATCH0,'I'
	breq	INIT_EEPROM_DONE ; already configured
INIT_EEPROM_BAD:
	ldi	SCRATCH0,0x33 ; BLINK CODE initialized EEPROM
	rcall	SEND_BLINK_CODE
	ldi	ZH,high(EEPROM_DEFAULTS<<1)
	ldi	ZL,low(EEPROM_DEFAULTS<<1)
INIT_EEPROM_LOOP1:
	lpm	XH,Z+ ; Address high + length ( << 2 )
	tst	XH ; Zero means done
	breq	INIT_EEPROM_DONE
	lpm	XL,Z+ ; Address low
	mov	SCRATCH0,XH
	andi	XH,3 ; two low bits
	lsr	SCRATCH0
	lsr	SCRATCH0 ; length
INIT_EEPROM_LOOP2:
	lpm	CHAR,Z+
	rcall	EEPROM_WRITEBYTE
	dec	SCRATCH0
	brne	INIT_EEPROM_LOOP2
	rjmp	INIT_EEPROM_LOOP1
INIT_EEPROM_DONE:
	rcall	CMD_TRACKOFF_ATSTART ; set default poll interval
;
	sei ; !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! INTERRUPTS ENABLED
;
	ldi	DELAY,8 ; 2 seconds
	rcall	SAFE_DELAY
;
; Check for reinit jumper - pull-up should be set on REINIT_CHECK_IN to prevent
; capacitive coupling from triggering reinit, and DDRA output should be enabled
	clr	SCRATCH0
	clr	SCRATCH1
MAIN_REINIT_LOOP:
	sbi	PORTA,REINIT_CHECK_OUT ; set reinit check high
	nop
	sbis	PINA,REINIT_CHECK_IN ; see if input line follows
	rjmp	MAIN_REINIT_JUMPER_NOT_PRESENT
	cbi	PORTA,REINIT_CHECK_OUT ; set reinit check low
	nop
	sbic	PINA,REINIT_CHECK_IN ; see if input line follows
	rjmp	MAIN_REINIT_JUMPER_NOT_PRESENT
	dec	SCRATCH0
	brne	MAIN_REINIT_LOOP
; If we got here, jumper is in place and we should reinit
	wdr
	tst	SCRATCH1 ; first pass?
	brne	MAIN_REINIT_LOOP ; no, wait until jumper removed
; first pass
	inc	SCRATCH1
	ldi	SCRATCH0,0x34 ; BLINK CODE remove jumper to reinit
	rcall	SEND_BLINK_CODE
	rjmp	MAIN_REINIT_LOOP
MAIN_REINIT_JUMPER_NOT_PRESENT:
	tst	SCRATCH1
	breq	MAIN_NOT_REINIT
	rcall	CLEAR_EEPROM_FLAG
	rjmp	CLEAR_RAM_AND_REBOOT
MAIN_NOT_REINIT:
	cbi	DDRA,REINIT_CHECK_OUT ; turn off port output
	sbi	PORTA,REINIT_CHECK_OUT ; enable pull-up
;
; Check for watchdog reset
	in	SCRATCH1,MCUSR
	sbrc	SCRATCH1,WDRF
	rjmp	MAIN_WATCHDOG_RESET
MAIN_NOT_WATCHDOG:
	sts	CMGL_SAFETY_COUNT,REG_ZERO ; Do not clear on watchdog reset!
#ifdef	RESET_PHONE_PERIODICALLY
	sts	PHONE_RESET_COUNT,REG_ZERO
#endif
#ifdef DEBUG_MESSAGE_COUNT_ENABLE
	sts	DEBUG_MESSAGE_COUNT,REG_ZERO 
	sts	DEBUG_MESSAGE_COUNT+1,REG_ZERO
#endif
	sts	WDR_COUNT,REG_ZERO ; clear watchdog reset count
; clear LAST GOOD FIX to dashes
	ldi	SCRATCH0,'-'
	ldi	XH,high(LAST_GOOD_FIX)
	ldi	XL,low(LAST_GOOD_FIX)
	ldi	SCRATCH1,LAST_GOOD_FIX_LEN
MNW_CLGF_LOOP:
	st	X+,SCRATCH0
	dec	SCRATCH1
	brne	MNW_CLGF_LOOP
;
	ldi	SCRATCH0,0x31 ; BLINK CODE startup
	rjmp	MAIN_AFTER_WATCHDOG_CHECK
MAIN_WATCHDOG_RESET:
	cbr	SCRATCH1,1<<WDRF
	out	MCUSR,SCRATCH1 ; clear the flag
	lds	SCRATCH0,WDR_COUNT
	inc	SCRATCH0
	brne	MAIN_WDR_COUNTNOTOVF
	dec	SCRATCH0 ; do not wrap to zero
MAIN_WDR_COUNTNOTOVF:
	sts	WDR_COUNT,SCRATCH0
	ldi	SCRATCH0,0x32 ; BLINK CODE watchdog reset
MAIN_AFTER_WATCHDOG_CHECK:
	rcall	SEND_BLINK_CODE ; startup

; Send startup message to debug port
#ifdef SERIAL_DEBUG
	ldi	ZH,high(DEBUG_STARTUP_MESSAGE<<1)
	ldi	ZL,low(DEBUG_STARTUP_MESSAGE<<1)
	rcall	SERIAL_SEND_STRING_TO_DEBUG
#endif
;
;	rcall	GPS_LOCATE ; DEBUG 
;	rcall	CMD_STATUS ; DEBUG
;
; Turn the phone on if it is turned off
#ifdef	RESET_PHONE_PERIODICALLY
	rcall	PHONE_RESET_IF_OFF
#else
	ldi	ZH,high(PHONE_POWERON_STRING<<1)
	ldi	ZL,low(PHONE_POWERON_STRING<<1)
	rcall	DO_PHONE_OPERATION ; power the phone on
	ldi	DELAY,(PHONE_RESET_WAKEUP_DELAY * 4)
	rcall	SAFE_DELAY
#endif

;; NOTIFY IF WDR - enable if you are chasing down a watchdog reset
;	lds	SCRATCH0,WDR_COUNT
;	cpi	SCRATCH0,1
;	brne	MAIN_DEBUG_NOT_WDR
;	ldi	SCRATCH0,PHONE_STRING_WATCHDOG_RESET - PHONE_STRING_BASE
;	rcall	MULTI_SEND_DO
;MAIN_DEBUG_NOT_WDR:
;

; Main loop: check tracking and send if required, check for message,
; retrieve it, process it, delete it, run callback/send, sleep.
; CMGL_SAFETY_COUNT: incremented when +CMGL detected
;  Cleared after a successful message delete.
;  If a bad message causes a watchdog reset or other crash, the count will not be cleared.
;  If this happens three times, stop trying to read the message and just delete it.
;  This prevents a bad message from causing an infinite loop.
MAIN:
; If in tracking mode, get a GPS fix and possibly transmit it
	lds	SCRATCH0,TRACK_STATE
	cpi	SCRATCH0,TRACK_STATE_POWERSAVE
	brsh	MAIN_TRK_SKIP ; no tracking in powersave mode
	cpi	SCRATCH0,TRACK_STATE_OFF
	brne	MAIN_TRK_LOCATE
MAIN_TRK_SKIP:
	rjmp	MAIN_TRK_END
MAIN_TRK_LOCATE:
	rcall	GPS_LOCATE ; result code returned in SCRATCH0 and in GPS_STATUS
	cpi	SCRATCH0,GPS_STATUS_FIX_STOPPED
	lds	SCRATCH0,TRACK_STATE ; does not affect flags
	breq	MAIN_TRK_GPS_STATUS_NOT_MOVING
	brsh	MAIN_TRK_GPS_STATUS_MOVING
	rjmp	MAIN_TRK_GPS_STATUS_BLOCKED
;
MAIN_TRK_GPS_STATUS_NOT_MOVING:
	cpi	SCRATCH0,TRACK_STATE_STOPPED
	breq	MAIN_TRK_CHECK_DISPLACED ; GPS=stopped, TRACK_STATE=stopped
	brsh	MAIN_TRK_TO_STATE_STOPPED ; GPS=stopped, TRACK_STATE=moving
	rjmp	MAIN_TRK_CHECK_DISPLACED ; GPS=stopped, TRACK_STATE=blocked
;
MAIN_TRK_GPS_STATUS_MOVING:
	cpi	SCRATCH0,TRACK_STATE_STOPPED
	breq	MAIN_TRK_SEND_STARTED ; GPS=moving, TRACK_STATE=stopped
	brsh	MAIN_TRK_CONTINUE_MOVING ; GPS=moving, TRACK_STATE=moving
	rjmp	MAIN_TRK_SEND_STARTED ; GPS=moving, TRACK_STATE=blocked
;
MAIN_TRK_GPS_STATUS_BLOCKED:
	cpi	SCRATCH0,TRACK_STATE_STOPPED
	breq	MAIN_TRK_TO_STATE_BLOCKED ; GPS=blocked, TRACK_STATE=stopped
	brsh	MAIN_TRK_TO_STATE_BLOCKED ; GPS=blocked, TRACK_STATE=moving
	rjmp	MAIN_TRK_END ; GPS=blocked, TRACK_STATE=blocked
;
; Notify if displaced flag is set, otherwise do nothing
MAIN_TRK_CHECK_DISPLACED:
	lds	SCRATCH0,FLDADR_GPS_LINE_COUNT_FLAGS
	sbrs	SCRATCH0,GPS_FLAG_DISPLACED
	rjmp	MAIN_TRK_END
	ldi	SCRATCH2,FIX_TYPE_MOVED - FIX_TYPE_BASE
	rjmp	MAIN_TRK_SEND

MAIN_TRK_SEND_STARTED:
	ldi	SCRATCH2,FIX_TYPE_STARTED - FIX_TYPE_BASE
	rcall	MAIN_TRK_CHECK_SPEEDING
MAIN_TRK_SEND_MOVING:
	ldi	SCRATCH3,TRACK_STATE_MOVING
	ldi	XH,high(EEADR_TRACK_MOVING_FIX_INTERVAL)
	ldi	XL,low(EEADR_TRACK_MOVING_FIX_INTERVAL)
	rjmp	MAIN_TRK_SAVE_AND_SEND

MAIN_TRK_TO_STATE_STOPPED:
	lds	SCRATCH1,TRACK_COUNT
	inc	SCRATCH1
	sts	TRACK_COUNT,SCRATCH1
;
	ldi	XH,high(EEADR_TRACK_STOPPED_NOTIFY_DELAY)
	ldi	XL,low(EEADR_TRACK_STOPPED_NOTIFY_DELAY)
	rcall	EEPROM_READBYTE_X_TO_SCRATCH0
;
	cp	SCRATCH1,SCRATCH0
	brsh	MAIN_TTSS_SEND
	rjmp	MAIN_TRK_END
MAIN_TTSS_SEND:
	ldi	XH,high(EEADR_TRACK_STOPPED_FIX_INTERVAL)
	ldi	XL,low(EEADR_TRACK_STOPPED_FIX_INTERVAL)
	ldi	SCRATCH2,FIX_TYPE_STOPPED - FIX_TYPE_BASE
	ldi	SCRATCH3,TRACK_STATE_STOPPED
	rjmp	MAIN_TRK_SAVE_AND_SEND

MAIN_TRK_TO_STATE_BLOCKED:
	lds	SCRATCH1,TRACK_COUNT
	inc	SCRATCH1
	sts	TRACK_COUNT,SCRATCH1
;
	ldi	XH,high(EEADR_TRACK_BLOCKED_NOTIFY_DELAY)
	ldi	XL,low(EEADR_TRACK_BLOCKED_NOTIFY_DELAY)
	rcall	EEPROM_READBYTE_X_TO_SCRATCH0
;
	cp	SCRATCH1,SCRATCH0
	brsh	MAIN_TTSB_SEND
	rjmp	MAIN_TRK_END
MAIN_TTSB_SEND:
	ldi	XH,high(EEADR_TRACK_BLOCKED_FIX_INTERVAL)
	ldi	XL,low(EEADR_TRACK_BLOCKED_FIX_INTERVAL)
	ldi	SCRATCH2,FIX_TYPE_STOPPED - FIX_TYPE_BASE
	ldi	SCRATCH3,TRACK_STATE_BLOCKED
	rjmp	MAIN_TRK_SAVE_AND_SEND

MAIN_TRK_CONTINUE_MOVING:
	lds	SCRATCH1,MOVING_LOCATE_COUNT
	inc	SCRATCH1
	sts	MOVING_LOCATE_COUNT,SCRATCH1
	sts	TRACK_COUNT,REG_ZERO
;
	ldi	SCRATCH2,FIX_TYPE_MOVING - FIX_TYPE_BASE
	rcall	MAIN_TRK_CHECK_SPEEDING ; carry returns set if fastest so far
	brcs	MAIN_TRK_SEND_MOVING ; always if fastest so far
;
	ldi	XH,high(EEADR_TRACK_MOVING_NOTIFY_FREQ)
	ldi	XL,low(EEADR_TRACK_MOVING_NOTIFY_FREQ)
	rcall	EEPROM_READBYTE_X_TO_SCRATCH0
;
	lds	SCRATCH1,MOVING_LOCATE_COUNT
	cp	SCRATCH1,SCRATCH0
	brsh	MAIN_TRK_SEND_MOVING
	rjmp	MAIN_TRK_END
	
; Sets FIX_TYPE_SPEEDING if above limit
; Returns carry clear if not fastest seen so far
; Returns carry set if fastest seen so far
MAIN_TRK_CHECK_SPEEDING:
	mov	CHAR,SCRATCH2
;
	ldi	XH,high(EEADR_SPEED_LIMIT)
	ldi	XL,low(EEADR_SPEED_LIMIT)
	rcall	EEPROM_READBYTE_X_TO_SCRATCH0
	tst	SCRATCH0
	breq	MAIN_TRK_SPEEDING_OUT ; if zero, speed limit disabled
;	
	clr	SCRATCH1
	rcall	MULX10_SCRATCH10
	lds	SCRATCH3,FLDADR_GPRMC_SPEED_NUMERIC
	lds	SCRATCH2,FLDADR_GPRMC_SPEED_NUMERIC+1
;
	cp	SCRATCH0,SCRATCH2 ; higher than saved speed limit?
	cpc	SCRATCH1,SCRATCH3
	brsh	MAIN_TRK_SPEEDING_OUT ; not speeding
;
	ldi	CHAR,FIX_TYPE_SPEEDING - FIX_TYPE_BASE
;
	lds	SCRATCH0,FASTEST_SPEED ; seen so far
	lds	SCRATCH1,FASTEST_SPEED+1
	cp	SCRATCH0,SCRATCH2 ; going faster?
	cpc	SCRATCH1,SCRATCH3
	brsh	MAIN_TRK_SPEEDING_OUT
;
	sts	FASTEST_SPEED,SCRATCH2
	sts	FASTEST_SPEED+1,SCRATCH3
	rjmp	MAIN_TRK_SPEEDING_FASTEST ; carry already set
MAIN_TRK_SPEEDING_OUT:
	clc
MAIN_TRK_SPEEDING_FASTEST:
	mov	SCRATCH2,CHAR
	ret

MAIN_TRK_SAVE_AND_SEND:
	sts	TRACK_COUNT,REG_ZERO
	sts	MOVING_LOCATE_COUNT,REG_ZERO
	sts	TRACK_STATE,SCRATCH3
	rcall	EEPROM_READWORD_X_TO_SCRATCH10
	sts	POLL_INTERVAL,SCRATCH1
	sts	POLL_INTERVAL+1,SCRATCH0
MAIN_TRK_SEND:
	sts	SEND_STRING_OFFSET_1,SCRATCH2 ; save fix type
	rcall	GPS_SEND_LOCATION
MAIN_TRK_END:

MAIN_POLL_PHONE:
; Poll the phone
	ldi	SCRATCH0,0x11 ; BLINK CODE polling
	rcall	SEND_BLINK_CODE
	cbr	FLAG_REGISTER,(1<<FLAG_GOT_RECEIVED_MESSAGE_NUMBER)|(1<<FLAG_NEED_TO_POLL_MESSAGES)|(1<<FLAG_MESSAGE_SENT)|(1<<FLAG_PHONE_FULL)
	ldi	ZH,high(PHONE_INIT_POLL_STRING<<1)
	ldi	ZL,low(PHONE_INIT_POLL_STRING<<1)
	rcall	DO_PHONE_OPERATION
	sbrc	FLAG_REGISTER,FLAG_GOT_RECEIVED_MESSAGE_NUMBER
	sbr	FLAG_REGISTER,1<<FLAG_NEED_TO_POLL_MESSAGES ; re-poll if we got a message
;	
	brtc	MAIN_AFTER_CHECK_FAILED
	ldi	SCRATCH0,0x13 ; BLINK CODE poll failed
	rcall	SEND_BLINK_CODE
MAIN_AFTER_CHECK_FAILED:
;
#ifdef	SERIAL_DEBUG
	ldi	ZH,high(SPACE_USED_MESSAGE<<1)
	ldi	ZL,low(SPACE_USED_MESSAGE<<1)
	rcall	SERIAL_SEND_STRING_TO_DEBUG
#endif
;
#ifdef	RESET_PHONE_PERIODICALLY
	lds	SCRATCH0,PHONE_RESET_COUNT ; only increment phone_reset_count on poll,
	tst	SCRATCH0                   ; if a message has been received
	breq	MAIN_SKIP_INC_PHONE_RESET  ; if no message received since last reset,
	inc	SCRATCH0                   ; do not increment
	sts	PHONE_RESET_COUNT,SCRATCH0
MAIN_SKIP_INC_PHONE_RESET:
#endif
;
	sbrs	FLAG_REGISTER,FLAG_GOT_RECEIVED_MESSAGE_NUMBER
	rjmp	MAIN_NO_MESSAGE
;
	lds	SCRATCH0,CMGL_SAFETY_COUNT
	cpi	SCRATCH0,4
	brsh	MAIN_FAULT_SKIP_READ
;	
#ifdef	DEBUG_KEEP_INVALID_MESSAGES
	sts	DEBUG_INVALID_MESSAGE_FLAG,REG_ZERO
#endif
	ldi	ZH,high(PHONE_READ_MESSAGE_STRING<<1)
	ldi	ZL,low(PHONE_READ_MESSAGE_STRING<<1)
	rcall	DO_PHONE_OPERATION
;
MAIN_FAULT_SKIP_READ:
	ldi	ZH,high(PHONE_DELETE_MESSAGE_STRING<<1)
	ldi	ZL,low(PHONE_DELETE_MESSAGE_STRING<<1)
#ifdef	DEBUG_KEEP_INVALID_MESSAGES
	lds	SCRATCH0,DEBUG_INVALID_MESSAGE_FLAG
	tst	SCRATCH0
	brne	MAIN_NO_DELETE
#endif
	rcall	DO_PHONE_OPERATION
MAIN_NO_DELETE:
	cbr	FLAG_REGISTER,1<<FLAG_GOT_RECEIVED_MESSAGE_NUMBER ; or send will fail
	brts	MAIN_FAULT_NO_CLEAR ; don't clear if delete failed
	sts	CMGL_SAFETY_COUNT,REG_ZERO
MAIN_FAULT_NO_CLEAR:
;
#ifdef	RESET_PHONE_PERIODICALLY
	lds	SCRATCH0,PHONE_RESET_COUNT
	inc	SCRATCH0 ; inc phone reset count after receiving a message
	sts	PHONE_RESET_COUNT,SCRATCH0
#endif
;
;
MAIN_NO_MESSAGE:
;
; If phone full flag is set, delete first message in STO UNSENT
	sbrc	FLAG_REGISTER,FLAG_PHONE_FULL
	rcall	MSD_CLEAR_SENT_AND_SEND_PENDING ; clears FLAG_PHONE_FULL
;
; Do command pending		
	lds	ZH,COMMAND_PENDING
	lds	ZL,COMMAND_PENDING+1
	tst	ZH
	brne	MAIN_HAVE_COMMAND_PENDING
	tst	ZL
	breq	MAIN_AFTER_COMMAND_PENDING
MAIN_HAVE_COMMAND_PENDING:
	sts	COMMAND_PENDING,REG_ZERO
	sts	COMMAND_PENDING+1,REG_ZERO
	icall ; Z
MAIN_AFTER_COMMAND_PENDING:
;
#ifdef	RESET_PHONE_PERIODICALLY
	lds	SCRATCH0,PHONE_RESET_COUNT
	cpi	SCRATCH0,PHONE_RESET_INTERVAL ; number of polls/messages before reset
	brlo	MAIN_NO_PHONE_RESET
	rcall	PHONE_RESET
	rjmp	MAIN_POLL_PHONE ; poll again to configure phone
MAIN_NO_PHONE_RESET:
#endif
;
; Any remaining characters coming from the phone can cause the device to
; poll continuously. Waiting too long could miss a message.
	ldi	SCRATCH1,BAUD_RATE_COUNTER/9  ; wait here to prevent retriggering
MAIN_DONE_PAUSE1:
	clr	SCRATCH0
MAIN_DONE_PAUSE2:
	dec	SCRATCH0
	brne	MAIN_DONE_PAUSE2 ; wait one character time (1MHz)
	dec	SCRATCH1
	brne	MAIN_DONE_PAUSE1
	rcall	SERIAL_RECEIVE_DISABLE
;
MAIN_FINISH_BLINKING_WDR:
	wdr ; if interrupts are not responding, wdr will not be repeatedly called
	mov	SCRATCH2,SCRATCH0
MAIN_FINISH_BLINKING:
	lds	SCRATCH0,BLINK_CODES_INDEX
	mov	SCRATCH1,SCRATCH0
	swap	SCRATCH1
	cp	SCRATCH0,SCRATCH1
	breq	MAIN_FINISHED_BLINKING ; don't sleep until all blink codes are sent
	cp	SCRATCH0,SCRATCH2
	brne	MAIN_FINISH_BLINKING_WDR
	rjmp	MAIN_FINISH_BLINKING
MAIN_FINISHED_BLINKING:
;
	sbrc	FLAG_REGISTER,FLAG_NEED_TO_POLL_MESSAGES
	rjmp	MAIN_POLL_PHONE ; check again for messages
;
; Here we go into MCU power down mode
; Watchdog timer wakes CPU every 8 seconds, or serial I/O wakes us up
;
MAIN_SLEEP:
#ifdef SERIAL_DEBUG
	ldi	SCRATCH0,(1<<TX_OUT_TO_PHONE)|(1<<TX_OUT_TO_DEBUG)
	out	PORTA,SCRATCH0 ; serial idle high
	out	DDRA,SCRATCH0 ; assert serial output
	ldi	ZH,high(POLL_INTERVAL_MESSAGE<<1)
	ldi	ZL,low(POLL_INTERVAL_MESSAGE<<1)
	rcall	SERIAL_SEND_STRING_TO_DEBUG
	rcall	SERIAL_RECEIVE_DISABLE
#endif
	lds	SCRATCH2,POLL_INTERVAL
	lds	SCRATCH1,POLL_INTERVAL+1 ; is in seconds
	wdr	; start timer at top
;
MAIN_BACK_TO_SLEEP:
	in	SCRATCH0,WDTCSR
	sbr	SCRATCH0,1<<WDIE ; enable watchdog interrupt, this gets cleared when it happens
	out	WDTCSR,SCRATCH0
;
	ldi	SCRATCH0,(1<<SE)+(1<<SM1)
	out	MCUCR,SCRATCH0 ; enable sleep mode
	sleep
; here woken by a serial interrupt or watchdog interrupt
	sbrc	FLAG_REGISTER,FLAG_NEED_TO_POLL_MESSAGES
	rjmp	MAIN_SLEEP_OUT ; wakeup was caused by a pin change interrupt, which set the flag
; Here we have watchdog wakeup
	subi	SCRATCH1,9 ; sleep period is 8 seconds, but timer runs a bit slow so make it 9
	sbci	SCRATCH2,0
	brcc	MAIN_BACK_TO_SLEEP ; poll interval expired?
;
; Check for wake up phone
	lds	SCRATCH0,TRACK_STATE
	cpi	SCRATCH0,TRACK_STATE_POWERSAVE_PHONE_OFF
	brne	MAIN_PHONE_WAS_NOT_OFF
; Wake up the phone, which was turned off
	ldi	SCRATCH0,0xff-((1<<RX_IN_FROM_PHONE)+(1<<RX_IN_FROM_GPS)) ; serial out idles high, and turn on pull-ups on unused lines to reduce power
	out	PORTA,SCRATCH0
#ifdef	SERIAL_DEBUG
	ldi	SCRATCH0,(1<<TX_OUT_TO_PHONE)|(1<<TX_OUT_TO_DEBUG)
#else
	ldi	SCRATCH0,1<<TX_OUT_TO_PHONE
#endif
	out	DDRA,SCRATCH0 ; assert serial output
#ifdef	RESET_PHONE_PERIODICALLY
	rcall	PHONE_RESET
#else
	ldi	ZH,high(PHONE_POWERON_STRING<<1)
	ldi	ZL,low(PHONE_POWERON_STRING<<1)
	rcall	DO_PHONE_OPERATION ; power the phone on
	ldi	DELAY,(PHONE_RESET_WAKEUP_DELAY * 4)
	rcall	SAFE_DELAY
#endif
	ldi	SCRATCH0,TRACK_STATE_POWERSAVE_PHONE_ON
	sts	TRACK_STATE,SCRATCH0
	ldi	XH,high(EEADR_POWERSAVE_PHONE_ON_INTERVAL)
	ldi	XL,low(EEADR_POWERSAVE_PHONE_ON_INTERVAL)
	rcall	EEPROM_READWORD_X_TO_SCRATCH10
	sts	POLL_INTERVAL,SCRATCH1
	sts	POLL_INTERVAL+1,SCRATCH0 ; set poll interval
	rjmp	MAIN_POLL_PHONE
MAIN_PHONE_WAS_NOT_OFF:
; Here check to see if we should enter powersave phone off mode
	lds	SCRATCH0,TRACK_STATE
	cpi	SCRATCH0,TRACK_STATE_POWERSAVE_PHONE_ON
	brne	MAIN_NOT_PHONEOFF
; Enter powersave phone off mode
	ldi	ZH,high(PHONE_POWEROFF_STRING<<1)
	ldi	ZL,low(PHONE_POWEROFF_STRING<<1)
	rcall	DO_PHONE_OPERATION
	rcall	SERIAL_RECEIVE_DISABLE 
	out	PCMSK0,REG_ZERO ; deactivate Pin Change Interrupt
	out	DDRA,REG_ZERO ; deassert serial output
	ldi	SCRATCH0,0xff-((1<<RX_IN_FROM_PHONE)+(1<<RX_IN_FROM_GPS)+(1<<TX_OUT_TO_PHONE)) ; set pull-ups on unconnected lines to save power
	out	PORTA,REG_ZERO ; deassert serial output (pull-up)
	ldi	SCRATCH0,TRACK_STATE_POWERSAVE_PHONE_OFF
	sts	TRACK_STATE,SCRATCH0
;
	ldi	XH,high(EEADR_POWERSAVE_PHONE_OFF_INTERVAL)
	ldi	XL,low(EEADR_POWERSAVE_PHONE_OFF_INTERVAL)
	rcall	EEPROM_READWORD_X_TO_SCRATCH10
	sts	POLL_INTERVAL,SCRATCH1
	sts	POLL_INTERVAL+1,SCRATCH0 ; set sleep interval
	rjmp	MAIN_SLEEP
;
MAIN_NOT_PHONEOFF:
MAIN_SLEEP_OUT:
	rjmp	MAIN

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#ifdef	RESET_PHONE_PERIODICALLY
; Send reset command to the phone and wait for restart
PHONE_RESET_IF_OFF:
	ldi	SCRATCH0,3 ; retry count
	rjmp	PRS_E1
PHONE_RESET:
	ldi	SCRATCH0,3 ; retry count
	push	SCRATCH0
	rjmp	PRS_NOW
;
PRS_LOOP:
	pop	SCRATCH0 ; retry count
	tst	SCRATCH0
	brne	PRS_CONT
	ret ; retry count expired
PRS_CONT:
	dec	SCRATCH0
PRS_E1:
	push	SCRATCH0
;
	ldi	ZH,high(PHONE_CHECK_STATUS_STRING<<1)
	ldi	ZL,low(PHONE_CHECK_STATUS_STRING<<1)
	sts	SEND_STRING_OFFSET_1,REG_ZERO ; will be set if phone +CFUN: 0
	rcall	DO_PHONE_OPERATION
	lds	SCRATCH1,SEND_STRING_OFFSET_1
	sbrs	SCRATCH1,0 ; will be 0xff if phone is off
	pop	SCRATCH0
	sbrs	SCRATCH1,0
	ret 
;
PRS_NOW:
	ldi	ZH,high(PHONE_RESET_STRING<<1)
	ldi	ZL,low(PHONE_RESET_STRING<<1)
	rcall	DO_PHONE_OPERATION ; leaving Z set correctly
	adiw	ZH:ZL,1
	rcall	SERIAL_SEND_STRING_TO_PHONE ; send without listening for reply
	sts	PHONE_RESET_COUNT,REG_ZERO ; clear count
	ldi	DELAY,(PHONE_RESET_WAKEUP_DELAY * 4)
	rcall	SAFE_DELAY
	rjmp	PRS_LOOP ; make sure it came on
#endif
		
; Function to interact with the phone
; Set ZH:ZL to address of string to send
; A 0xf0 and following timeout byte sets new receive timeout
; A 0xf1 clears the parse fields
DO_PHONE_OPERATION:
	ldi	SCRATCH0,MAX_PHONE_ERRORS
	mov	RETRY_COUNT,SCRATCH0
	sts	PHONE_STRING,ZH
	sts	PHONE_STRING+1,ZL
DPO_RETRY_INIT_STRING:
	lds	ZH,PHONE_STRING
	lds	ZL,PHONE_STRING+1
DPO_NEXT_COMMAND: ; sending
	wdr
	lpm	SCRATCH0,Z
	cpi	SCRATCH0,0xf0 ; change timeout?
	brne	DPO_NOT_CHANGE_TIMEOUT
	lpm	SCRATCH0,Z+ ; dispose 0xf0
	lpm	SCRATCH0,Z+ ; load new timeout
	sts	RECEIVE_TIMEOUT,SCRATCH0
	rjmp	DPO_NEXT_COMMAND
DPO_NOT_CHANGE_TIMEOUT:
	cpi	SCRATCH0,0xf1 ; clear parse fields?
	brne	DPO_NOT_CLEAR_PARSE_FIELDS
	lpm	SCRATCH0,Z+ ; dispose 0xf1
	rcall	CLEAR_PARSE_FIELDS
	rjmp	DPO_NEXT_COMMAND
DPO_NOT_CLEAR_PARSE_FIELDS:
	tst	SCRATCH0
	breq	DPO_RET ; done with last command
	rcall	SERIAL_SEND_STRING_TO_PHONE ; send next AT command
	push	ZH
	push	ZL ; preserve location in init string
; receiving
	ldi	YH,high(RECEIVE_BUFFER) ; 1
	mov	YL,SERIAL_POINTER ; dump all characters in buffer
	clt	; remember to clear the error flag 
#ifdef	SERIAL_DEBUG
	mov	DEBUG_POINTER,YL ; save start of buffer for debug output
#endif
	rcall	SERIAL_RECEIVE_ENABLE_FROM_PHONE
; processing response
DPO_SNL_PARSE_INPUT:
	rcall	SKIP_WHITESPACE
DPO_PARSE_INPUT:
	clt
	ldi	ZH,high(PHONE_MATCH_PATTERN<<1)
	ldi	ZL,low(PHONE_MATCH_PATTERN<<1)
	rjmp	BRANCH_ON_STRING

; branch targets based on string parse
PHONE_INIT_ERROR:
#ifdef	SERIAL_DEBUG
	rcall	DEBUG_DUMP_BUFFER
	ldi	ZH,high(FAILED_MESSAGE<<1)
	ldi	ZL,low(FAILED_MESSAGE<<1)
	brtc	PIE_NOT_TIMEOUT
	ldi	ZH,high(TIMEOUT_MESSAGE<<1)
	ldi	ZL,low(TIMEOUT_MESSAGE<<1)
PIE_NOT_TIMEOUT:
	rcall	SERIAL_SEND_STRING_TO_DEBUG
#endif
	pop	ZL
	pop	ZH ; discard location in init string
	dec	RETRY_COUNT
	brne	DPO_RETRY_INIT_STRING ; try again?
	set
DPO_RET:
	ret	; give up
	
PHONE_SEND_PROMPT:
PHONE_INIT_OK:
#ifdef	SERIAL_DEBUG
	rcall	DEBUG_DUMP_BUFFER
	ldi	ZH,high(SUCCEEDED_MESSAGE<<1)
	ldi	ZL,low(SUCCEEDED_MESSAGE<<1)
	rcall	SERIAL_SEND_STRING_TO_DEBUG
#endif
	pop	ZL
	pop	ZH ; get address in command list
	lpm	SCRATCH0,Z
	tst	SCRATCH0 ; end of list?
	brne	DPO_NEXT_COMMAND
	ret	

PHONE_MSG_SENT: ; CMSS
	sbr	FLAG_REGISTER,1<<FLAG_MESSAGE_SENT
	rjmp	PHONE_INIT_IGNORE
PHONE_INIT_ISOFF: ; CFUN: 0
	ldi	SCRATCH0,0xff
	sts	SEND_STRING_OFFSET_1,SCRATCH0
PHONE_INIT_IGNORE: ; got ignorable responses
	rcall	SKIP_TO_NEXT_LINE
	rjmp	DPO_PARSE_INPUT

PHONE_READ_CMGR:
	rcall	SKIP_TO_NEXT_LINE ; remainder of CMGR line
	mov	BASE,YL ; remember where we left off
	mov	CHECKSUM,VERIFY_CHECKSUM ; checksum from CMGL or CMGW
	rcall	CHECKSUM_AND_SKIP_TO_NEXT_LINE ; checksum and pull whole command into memory
	brts	PHONE_INIT_ERROR
;
	sbrs	FLAG_REGISTER,FLAG_GOT_RECEIVED_MESSAGE_NUMBER ; is this received message?
	rjmp	DPO_SNL_PARSE_INPUT ; read-back of sent message, so don't process it
;
	cp	CHECKSUM,VERIFY_CHECKSUM
	brne	PHONE_INIT_ERROR ; checksum mismatch
	mov	YL,BASE ; put pointer back to beginning of line
	rcall	CHECK_PASSWORD
	brts	PRC_PASSWORD_NO_GOOD
PRC_PASSWORD_GOOD: ; got valid password
	ldi	ZH,high(COMMAND_MATCH_PATTERN<<1)
	ldi	ZL,low(COMMAND_MATCH_PATTERN<<1)
	rcall	BRANCH_ON_STRING ; parse and execute the command
PRC_PASSWORD_NO_GOOD:
	clt
	rjmp	DPO_SNL_PARSE_INPUT


; A CMGL request lists all messages in a particular folder in the phone. The C168i has an
; anomaly where messages with status STO SENT remain in the STO UNSENT folder. Power cycling
; the phone causes them to move over to STO SENT folder. It appears that a user interface
; task in the phone is responsible for moving the messages, and when using the AT interface,
; they do not get moved. This means we have to CMGL both folders (STO UNSENT and STO SENT)
; and process the messages according to their individual status, which appears after the
; message number.
PHONE_INIT_CMGL: ; got the +CMGL response
	sbrs	FLAG_REGISTER,FLAG_GOT_RECEIVED_MESSAGE_NUMBER ; already have a message number
	rjmp	PICMGL_PARSE
; here, discarding message
	rcall	SKIP_TO_NEXT_LINE ; Skip CMGL line
	rcall	SKIP_TO_NEXT_LINE ; Skip message text
	rjmp	DPO_PARSE_INPUT ; Waiting for next message or OK
PICMGL_PARSE:	; Get message number and checksum the message
;
; This builds a list in the parse fields of STO SENT and STO UNSENT messages
; as well as filling out CURRENT_SMS. It will only accept one RECeived message
; and will skip all following messages. Will accept multiple STO messages.
; Get number, then look for "STO SENT" or "STO UNSENT" or "REC "
	rcall	PHONE_COPY_CURRENT_SMS
PPMN_CSMS_MATCH:
	ldi	ZH,high(CMGL_MATCH_PATTERN<<1)
	ldi	ZL,low(CMGL_MATCH_PATTERN<<1)
	rjmp	BRANCH_ON_STRING
;
PPMN_CSMS_SENT:
	ldi	ZH,high(FLDADR_CMGL_SENT)
	ldi	ZL,low(FLDADR_CMGL_SENT)
	rjmp	PPMN_CSMS
PPMN_CSMS_UNSENT:
	ldi	ZH,high(FLDADR_CMGL_UNSENT)
	ldi	ZL,low(FLDADR_CMGL_UNSENT)
PPMN_CSMS:
	ldi	SCRATCH0,FLDLEN_CMGL_MAXMSGS
PPMN_CSMS_FINDFREE: ; search for unused field
	ld	SCRATCH1,Z
	tst	SCRATCH1 ; field used?
	breq	PPMN_CSMS_COPY
	adiw	ZH:ZL,4
	dec	SCRATCH0
	breq	PPMN_OUT
	rjmp	PPMN_CSMS_FINDFREE	
;
PPMN_CSMS_COPY:
	ldi	XH,high(CURRENT_SMS)
	ldi	XL,low(CURRENT_SMS)
	ldi	SCRATCH0,4
PPMN_CSMS_COPY_LOOP:
	ld	SCRATCH1,X+
	st	Z+,SCRATCH1
	dec	SCRATCH0
	brne	PPMN_CSMS_COPY_LOOP
	rjmp	PPMN_OUT
;
PPMN_CSMS_RECEIVED:
	sbr	FLAG_REGISTER,1<<FLAG_GOT_RECEIVED_MESSAGE_NUMBER
	lds	SCRATCH0,CMGL_SAFETY_COUNT
	inc	SCRATCH0
	sts	CMGL_SAFETY_COUNT,SCRATCH0
PPMN_OUT:
	rcall	SKIP_TO_NEXT_LINE ; at end of CMGL LINE
	rcall	CHECKSUM_AND_SKIP_TO_NEXT_LINE ; Checksum the message into VERIFY_CHECKSUM
	rjmp	DPO_PARSE_INPUT

PHONE_SEND_CMGW: ; get the message number after a CMGW
	rcall	PHONE_COPY_CURRENT_SMS
	rjmp	DPO_SNL_PARSE_INPUT

; Copy numeric value from incoming to CURRENT_SMS
PHONE_COPY_CURRENT_SMS:
	rcall	SKIP_WHITESPACE
	ldi	ZH,high(CURRENT_SMS)
	ldi	ZL,low(CURRENT_SMS)
	ldi	SCRATCH0,4 ; field length
PCCS_LOOP: ; copy message number to CURRENT_SMS
	st	Z,REG_ZERO ; zero field shorter than 4 characters
	rcall	GET_NEXT_CHAR
	brts	PCCS_LOOP_OUT
	cpi	CHAR,'0'
	brlo	PCCS_LOOP_OUT
	cpi	CHAR,'9'+1
	brsh	PCCS_LOOP_OUT
	st	Z+,CHAR
	dec	SCRATCH0
	brne	PCCS_LOOP
PCCS_LOOP_OUT:
	ret


; Incoming message notification
PHONE_GOT_CMTI:
	sbr	FLAG_REGISTER,1<<FLAG_NEED_TO_POLL_MESSAGES
	rcall	SKIP_TO_NEXT_LINE
	rjmp	DPO_PARSE_INPUT
	
; Got count of messages in storage
PHONE_INIT_CPMS:
; parse fields cleared by control string
	rcall	SKIP_WHITESPACE
	ldi	ZH,high(SETTINGS_PARSE_STRING<<1)
	ldi	ZL,low(SETTINGS_PARSE_STRING<<1) ; might as well reuse this
	cbr	FLAG_REGISTER,1<<FLAG_PARSE_NMEA
	ldi	CHAR,','
	mov	DELIM,CHAR
	rcall	PARSE_NMEA_OR_COMMAND_LINE
	rcall	SKIP_TO_NEXT_LINE
;
	ldi	XH,high(FLDADR_CPMS_TOTAL)
	ldi	XL,low(FLDADR_CPMS_TOTAL)
	ldi	SCRATCH0,8 ; field length
	mov	PAT,SCRATCH0
	clr	DELIM ; ignore decimal point (should not be there)
	rcall	PARSE_DECIMAL ; SCRATCH1:SCRATCH0 now has numeric value
	push	SCRATCH1
	push	SCRATCH0
;
	ldi	XH,high(FLDADR_CPMS_USED)
	ldi	XL,low(FLDADR_CPMS_USED)
	ldi	SCRATCH0,8 ; field length
	mov	PAT,SCRATCH0
	clr	DELIM ; ignore decimal point (should not be there)
	rcall	PARSE_DECIMAL ; SCRATCH1:SCRATCH0 now has numeric value
	pop	SCRATCH2
	pop	SCRATCH3
;
#ifdef	SERIAL_DEBUG
	sts	FLDADR_CPMS_USED_NUMERIC,SCRATCH1
	sts	FLDADR_CPMS_USED_NUMERIC+1,SCRATCH0
	sts	FLDADR_CPMS_TOTAL_NUMERIC,SCRATCH3
	sts	FLDADR_CPMS_TOTAL_NUMERIC+1,SCRATCH2
#endif
	ldi	CHAR,4 ; Keep 4 free slots for incoming messages
	add	SCRATCH0,CHAR
	adc	SCRATCH1,REG_ZERO
	cp	SCRATCH2,SCRATCH0
	cpc	SCRATCH3,SCRATCH1
	brsh	PICPMS_NOT_FULL
	sbr	FLAG_REGISTER,1<<FLAG_PHONE_FULL
PICPMS_NOT_FULL:
	rjmp	DPO_PARSE_INPUT


; Battery charge / signal strength response
PHONE_GOT_BATTERY_CHARGE:
	ldi	ZL,low(STATUSMSG_BATTERY_CHARGE)
	rjmp	PGSSBC
PHONE_GOT_SIGNAL_STRENGTH:
	ldi	ZL,low(STATUSMSG_SIGNAL_STRENGTH)
PGSSBC:
	ldi	ZH,high(STATUSMSG_BATTERY_CHARGE) ; assumption on same page
	mov	XH,YH
	mov	XL,YL
	rcall	SKIP_TO_NEXT_LINE ; bring the whole line into buffer
	ldi	SCRATCH0,3
	mov	PAT,SCRATCH0
	ldi	SCRATCH0,0xff
	mov	DELIM,SCRATCH0 ; do not parse .
	rcall	PARSE_DECIMAL ; parse the number
	st	Z+,SCRATCH1
	st	Z+,SCRATCH0 ; save the number
	rjmp	DPO_PARSE_INPUT ; resume

; Waits on DELAY countdown, while doing watchdog reset only when number counts down
; If interrupt goes away, system will reset. 
SAFE_DELAY:
	cp	SCRATCH0,DELAY
	breq	SDY_NOTICK
	wdr
	mov	SCRATCH0,DELAY
SDY_NOTICK:
	tst	DELAY
	brne	SAFE_DELAY
	ret


; Compare the buffer against the password in EEPROM
; Expects password followed by a space
; If password matches, returns T flag clear and NEXTCHAR is the one after the space
; If password does not match, returns T flag set and NEXTCHAR is the newline
CHECK_PASSWORD:
	clt
	ldi	XH,high(EEADR_PASSWORD)
	ldi	XL,low(EEADR_PASSWORD)
	ldi	SCRATCH1,EELEN_PASSWORD+1
CPW_LOOP:
	rcall	EEPROM_READBYTE_X_TO_SCRATCH0 ; increments X
	dec	SCRATCH1
;
	rcall	GET_NEXT_CHAR
	brts	CPW_FAIL
	cpi	CHAR,' '
	breq	CPW_SPACE ; space
	cpi	CHAR,13
	breq	CPW_FAIL ; newline
;
	cp	CHAR,SCRATCH0
	brne	CHECK_PASSWORD ; start over
	rjmp	CPW_LOOP ; keep comparing
;
CPW_SPACE:
	tst	SCRATCH0 ; at end of password?
	breq	CPW_PASS ; yes, so return success
	tst	SCRATCH1 ; at end of password field?
	breq	CPW_PASS ; yes, so return success
	rjmp	CHECK_PASSWORD ; otherwise start over
;
CPW_FAIL:
	ldi	SCRATCH0,0x21 ; BLINK CODE invalid password
	rcall	SEND_BLINK_CODE
#ifdef	DEBUG_KEEP_INVALID_MESSAGES
	ldi	SCRATCH0,0xff
	sts	DEBUG_INVALID_MESSAGE_FLAG,SCRATCH0
#endif
	set	
CPW_PASS:
	ret

#ifdef	SERIAL_DEBUG
; Print the contents of the buffer to the debug port
DEBUG_DUMP_BUFFER:
	ldi	ZH,high(CRLF<<1)
	ldi	ZL,low(CRLF<<1)
	rcall	SERIAL_SEND_STRING_TO_DEBUG
	ldi	ZH,high(RECEIVE_BUFFER)
	mov	ZL,DEBUG_POINTER ; address before read
DDB_PRINT_LOOP:
	cp	ZL,SERIAL_POINTER
	breq	DDB_PRINT_DONE
	ld	SERIAL_DATA,Z+
	ldi	ZH,high(RECEIVE_BUFFER) ; make sure it does not roll over
	rcall	SERIAL_SEND_BYTE
	rjmp	DDB_PRINT_LOOP
DDB_PRINT_DONE:
	ldi	ZH,high(CRLF<<1)
	ldi	ZL,low(CRLF<<1)
	rcall	SERIAL_SEND_STRING_TO_DEBUG
	ret
#endif

; Skip over remainder of line, CRLF and any whitespace
; Leaves pointer at first non-whitespace character, or at end of buffer
; if timeout occurs. This version updates VERIFY_CHECKSUM
CHECKSUM_AND_SKIP_TO_NEXT_LINE:
	clr	SCRATCH0
	clr	VERIFY_CHECKSUM
CSNL_LOOP:
	rcall	GET_NEXT_CHAR
	brts	SW_OUT
	cpi	CHAR,13 ; first newline
	breq	SKIP_WHITESPACE
;
	cpi	SCRATCH0,PHONE_MAX_CHECKSUM_LENGTH
	brsh	SKIP_TO_NEXT_LINE
	inc	SCRATCH0
	sts	READ_CHECKSUM_BYTES,SCRATCH0 ; store length
	eor	VERIFY_CHECKSUM,CHAR
	rjmp	CSNL_LOOP

; Skip over remainder of line, CRLF and any whitespace
; Leaves pointer at first non-whitespace character, or at end of buffer
; if timeout occurs. 
SKIP_TO_NEXT_LINE:
	rcall	GET_NEXT_CHAR
	brts	SW_OUT
STNL_CPI:
	cpi	CHAR,13 ; first newline
	brne	SKIP_TO_NEXT_LINE
SNL_OUT:
;	rjmp	SKIP_WHITESPACE ; Fall through, don't put anything in between!

; Skip over whitespace in the buffer
; Leaves pointer at first non-whitespace character, or at end of buffer
; if timeout occurs
SKIP_WHITESPACE:
	rcall	GET_NEXT_CHAR
	brts	SW_OUT
	cpi	CHAR,33 ; first non-whitespace character
	brlo	SKIP_WHITESPACE
	dec	YL ; back up to first non-whitespace character
SW_OUT:
	ret

; Branch based on string
; Call with Y set to the buffer, and Z set to the pattern
; Pattern is like
;	.db	low(POE_ACCEPT),high(POE_ACCEPT),"OK",13,10,0, \
;	   	low(POE_REJECT),high(POE_REJECT),"ERROR",13,10,0, \
;		low(POE_GOT_ERROR),high(POE_GOT_ERROR),0
; The function will check the string in Y against each of the strings in
; the pattern, and will branch (JMP) to the label corresponding to the
; matched string. Your pattern must end with a default (zero length string)
; which will be taken if nothing matches. If get_next_char returns a timeout
; (T flag) the function skips to the default.
; This is a case insensitive search against the buffer. Patterns should be uppercase.
BRANCH_ON_STRING:
	mov	BASE,YL ; store beginning of string
BOS_LOOP:
	lpm	PAT,Z+
	push	PAT
	lpm	PAT,Z+
	push	PAT ; branch address now on stack
BOS_MATCHING:
	lpm	PAT,Z+
	tst	PAT
	breq	BOS_MATCHED ; zero in pattern indicates we found a match
	brts	BOS_NOGETNEXTCHAR ; once it times out, skip rest of patterns
	rcall	GET_NEXT_CHAR
	cpi	CHAR,'a'
	brlo	BOS_NOTLC
	cpi	CHAR,'z'+1
	brsh	BOS_NOTLC
	subi	CHAR,'a'-'A' ; change lowercase to uppercase
BOS_NOTLC:
BOS_NOGETNEXTCHAR:	
	brts	BOS_MISMATCH ; if get_next_char failed
	cp	CHAR,PAT
	breq	BOS_MATCHING ; matching so far
BOS_MISMATCH:
	pop	CHAR
	pop	CHAR ; discard branch address from stack
BOS_SKIP: 
	lpm	PAT,Z+ ; advance pattern to next zero
	tst	PAT
	brne	BOS_SKIP
	mov	YL,BASE ; reset string pointer
	rjmp	BOS_LOOP ; try the next one
BOS_MATCHED:
	ret	; Jumping to the selected routine, NOT returning to caller!


; Clear the parse memory
CLEAR_PARSE_FIELDS:
	ldi	XH,high(PARSE_FIELDS)
	ldi	XL,low(PARSE_FIELDS)
	ldi	SCRATCH0,PARSE_FIELDS_LENGTH
CPF_LOOP:
	st	X+,REG_ZERO
	dec	SCRATCH0
	brne	CPF_LOOP
	ret


; Parse a NMEA line or command line into fields and store them in SRAM
; Compute the NMEA checksum and decode the hex checksum to verify data
; Call with Y set to the buffer, and Z set to the pattern
; Set DELIM to the desired delimiter, normally space or comma
; Pattern is three-byte sequences:
; <field number><max length and high address bits><low address byte>
; Field number starts with 0 meaning first field. Put FF after last field.
; NMEA Checksum goes from right after $ to before *
; Non-NMEA checksum is whole line excluding newline
; NMEA delimiter is a comma and end of line is * then two checksum digits
; Command line delimiter is normally a space and end of line is a CR
; Set FLAG_PARSE_NMEA for NMEA mode, clear it for command line mode
; Timeout sets T flag and aborts.
PARSE_NMEA_OR_COMMAND_LINE:
	clr	READ_FIELD ; current field
	clr	PAT ; use as a zero for writing into memory
	clr	CHECKSUM ; calculated in both modes, VERIFY_CHECKSUM only in NMEA mode
	cbr	FLAG_REGISTER,1<<FLAG_SKIP_REPEATED_DELIMITER ; clear by default
;	
PNL_NEXT_FIELD:
	lpm	PARSE_FIELD,Z+ ; is now field number to parse
	lpm	XH,Z+
	lpm	XL,Z+
	mov	FIELD_LENGTH,XH
	andi	XH,3 ; X is now valid address of buffer
	lsr	FIELD_LENGTH
	lsr	FIELD_LENGTH ; is now max length of field
;
PNL_NEXTCHAR:
	rcall	GET_NEXT_CHAR
	brts	PNL_FAILED
	cp	CHAR,DELIM
	sbrc	FLAG_REGISTER,FLAG_SKIP_REPEATED_DELIMITER
	breq	PNL_NEXTCHAR ; Skip whitespace
	cbr	FLAG_REGISTER,1<<FLAG_SKIP_REPEATED_DELIMITER
;
	cpi	CHAR,13 ; end marker in non-NMEA mode
	sbrc	FLAG_REGISTER,FLAG_PARSE_NMEA ; only in NMEA mode
	cpi	CHAR,'*' ; end marker in NMEA mode
	breq	PNL_GET_NMEA_CHECKSUM_OR_FINISH
	eor	CHECKSUM,CHAR ; update checksum
;
	cpi	CHAR,'$'
	brne	PNL_NOTDOLLAR
	sbrc	FLAG_REGISTER,FLAG_PARSE_NMEA ; only in NMEA mode
	clr	CHECKSUM ; in NMEA mode, $ is start of checksummed area
PNL_NOTDOLLAR:
;
	cp	CHAR,DELIM
	brne	PNL_NOTDELIM
	sbrs	FLAG_REGISTER,FLAG_PARSE_NMEA ;               ; except in NMEA mode...
	sbr	FLAG_REGISTER,1<<FLAG_SKIP_REPEATED_DELIMITER ; skip multiple delimiters
	cp	READ_FIELD,PARSE_FIELD ; was previous field the current parsed field?
	brne	PNL_NOT_CURRENT_FIELD ; delimiter in current field
	tst	FIELD_LENGTH ; field full?
	breq	PNL_NO_NULL_TERMINATE
	st	X,PAT ; null terminate field
PNL_NO_NULL_TERMINATE:
	inc	READ_FIELD
	rjmp	PNL_NEXT_FIELD ; got delimiter and we were parsing current field
PNL_NOT_CURRENT_FIELD:
	inc	READ_FIELD ; current field
	rjmp	PNL_NEXTCHAR
PNL_NOTDELIM:
;
	cp	READ_FIELD,PARSE_FIELD ; is this the field to copy?
	brne	PNL_NOSAVECHAR
	tst	FIELD_LENGTH ; field full?
	breq	PNL_NOSAVECHAR
	st	X+,CHAR	; store the character
	dec	FIELD_LENGTH ; and reduce length remaining
PNL_NOSAVECHAR:
	rjmp	PNL_NEXTCHAR
;
PNL_GET_NMEA_CHECKSUM_OR_FINISH:
	cp	READ_FIELD,PARSE_FIELD ; was previous field the current field?
	brne	PNL_LAST_NO_NULL_TERMINATE 
	tst	FIELD_LENGTH ; field full?
	breq	PNL_LAST_NO_NULL_TERMINATE
	st	X,PAT ; null terminate field
PNL_LAST_NO_NULL_TERMINATE:
	sbrs	FLAG_REGISTER,FLAG_PARSE_NMEA ; not in NMEA mode
	ret	
;
	rcall	GET_NEXT_CHAR
	brts	PNL_FAILED
	subi	CHAR,'0'
	cpi	CHAR,10 ; is A-F?
	brlo	PLGNC1_NOT_AF
	subi	CHAR,7 ; correct for A-F
PLGNC1_NOT_AF:
	swap	CHAR
	mov	VERIFY_CHECKSUM,CHAR ; high half-byte
	rcall	GET_NEXT_CHAR
	brts	PNL_FAILED
	subi	CHAR,'0'
	cpi	CHAR,10 ; is A-F?
	brlo	PLGNC2_NOT_AF
	subi	CHAR,7 ; correct for A-F
PLGNC2_NOT_AF:
	add	VERIFY_CHECKSUM,CHAR ; got whole checksum	
PNL_FAILED:
	ret	


; Reads into CHAR the next character from the serial buffer (pointer is Y)
; Spins if no character available
; Returns T flag set if RECEIVE_TIMEOUT exceeded while spinning
; Watchdog reset while waiting, only when DELAY counts down
; If interrupts fail, system will reset.
GET_NEXT_CHAR:	
	brts	GNC_TIMEDOUT ; so we don't have to check before every call
	lds	DELAY,RECEIVE_TIMEOUT
GNC_WAITCHAR:
	cp	YL,SERIAL_POINTER ; character pending?
	brne	GNC_HAVECHAR ; yes
	cp	CHAR,DELAY ; has it counted down?
	breq	GNC_NOTICK
	wdr
	mov	CHAR,DELAY
GNC_NOTICK:
	tst	DELAY
	brne	GNC_WAITCHAR ; time up?
GNC_TIMEDOUT:
	set	; Report error
	ret
GNC_HAVECHAR:
	ld	CHAR,Y+
	ldi	YH,1 ; wrap around
SERIAL_SEND_DONE:
	wdr
	ret
	

; Send a string from program memory, pointer in Z register, 0 terminated
; Special functions: 
; 0xf7 copies SEND_CHECKSUM to VERIFY_CHECKSUM and clears SEND_CHECKSUM
; 0xf8 print a 16-bit number (decimal) as an integer, next two are high, low of address
; 0xf9 print a 16-bit number as xxx.x, next two are high, low of address
; 0xfa-0xfc indicates substring call, next two bytes are high, low of substring
; 0xfa = add SEND_STRING_OFFSET_0 to select string to call (offset X2 for ROM)
; 0xfb = add SEND_STRING_OFFSET_1 to select string to call (offset X2 for ROM)
; 0xfc = no offset, call directly
; 0xfd in data indicates a SRAM string field
; 0xfe in data indicates a EEPROM field
; 0xff in data indicates a EEPROM field with a trailing space only if field not empty
; Next byte: two low bits are address high bits, remainder of bits are field length 1-64
; Next byte: address low byte
; Ex: 0xfd, high(ram_field) + ( 32<<2 ), low(ram_field) 32-byte string in RAM
; Uses X pointer and scratch0,1,2
#ifdef	SERIAL_DEBUG
SERIAL_SEND_STRING_TO_DEBUG:
	ldi	SCRATCH0,1<<TX_OUT_TO_DEBUG
	mov	SERIAL_PORT,SCRATCH0
	rjmp	SERIAL_SEND_STRING
#endif
SERIAL_SEND_STRING_TO_PHONE:
#ifdef	SERIAL_DEBUG
	ldi	SCRATCH0,(1<<TX_OUT_TO_PHONE)|(1<<TX_OUT_TO_DEBUG)
#else
	ldi	SCRATCH0,1<<TX_OUT_TO_PHONE
#endif
	mov	SERIAL_PORT,SCRATCH0
SERIAL_SEND_STRING:
	lpm	SCRATCH0,Z+ ; get one byte and increment pointer
	cpi	SCRATCH0,0xf7 ; special function?
	brsh	SSS_SPECIAL ; got a special function
	tst	SCRATCH0 ; end of string?
	breq	SERIAL_SEND_DONE ; ret
	mov	SERIAL_DATA,SCRATCH0
	rcall	SERIAL_SEND_BYTE	
	rjmp	SERIAL_SEND_STRING
SSS_SPECIAL:
	cpi	SCRATCH0,0xfd
	brsh	SSS_FIELD ; SRAM or EEPROM field
	cpi	SCRATCH0,0xfa
	brsh	SSS_SUBSTRING
	cpi	SCRATCH0,0xf8
	brsh	SSS_DECIMAL
; checksum capture
	lds	SCRATCH0,SEND_CHECKSUM_COUNTER
	sts	SEND_CHECKSUM_BYTES,SCRATCH0 ; store number of bytes checksummed
	sts	SEND_CHECKSUM_COUNTER,REG_ZERO
	lds	VERIFY_CHECKSUM,SEND_CHECKSUM
	sts	SEND_CHECKSUM,REG_ZERO
	rjmp	SERIAL_SEND_STRING
SSS_DECIMAL:
	lpm	XH,Z+ ; address high
	lpm	XL,Z+ ; address low
	ld	SCRATCH1,X+ ; high byte
	ld	XL,X ; low byte
	mov	XH,SCRATCH1
	andi	SCRATCH0,1 ; isolate integer / xxx.x flag
	rcall	PRINT_DECIMAL
	rjmp	SERIAL_SEND_STRING ; out on end of field
SSS_SUBSTRING:
; this is substring call 0xfa-0xfc
	lpm	SCRATCH2,Z+
	lpm	SCRATCH1,Z+ ; got base address of substring
	push	ZH
	push	ZL ; store old address past substring call
	lds	ZL,SEND_STRING_OFFSET_0
	sbrc	SCRATCH0,0 ; 0xfa = 11111010, 0xfb = 11111011
	lds	ZL,SEND_STRING_OFFSET_1
	sbrc	SCRATCH0,2 ; 0xfc = 11111100
	clr	ZL
	clr	ZH
; add offset to base address and run substring
	lsl	ZL ; since string is in ROM it must be word-aligned
	rol	ZH ; doubling offset gives 512-byte range
	add	ZL,SCRATCH1
	adc	ZH,SCRATCH2
	rcall	SERIAL_SEND_STRING ; send the substring
	pop	ZL
	pop	ZH ; get old address back
	rjmp	SERIAL_SEND_STRING ; resume this string
SSS_FIELD:
	clr	SCRATCH2 ; used to indicate whether we need a trailing space
	lpm	XH,Z+
	lpm	XL,Z+
	mov	SCRATCH1,XH ; field length
	andi	XH,3 ; address high bits
	andi	SCRATCH1,0xfc ; remove two low bits
	mov	SCRATCH3,SCRATCH0 ; used for EEPROM below since EEPROM read overwrites SCRATCH0
	cpi	SCRATCH0,0xfe ; eeprom field?
	brsh	SSS_FIELD_EEPROM_LOOP
SSS_FIELD_SRAM_LOOP:
	ld	SERIAL_DATA,X+
	tst	SERIAL_DATA
	breq	SSS_RJMP_SERIAL_SEND_STRING ; out on null termination
	rcall	SERIAL_SEND_BYTE
	subi	SCRATCH1,4 ; field length is *4 due to its bit position
	brne	SSS_FIELD_SRAM_LOOP
SSS_BACKTOTOP:
	rjmp	SERIAL_SEND_STRING ; out on end of field
SSS_FIELD_EEPROM_LOOP:
	rcall	EEPROM_READBYTE_X_TO_SCRATCH0 ; increments X
	mov	SERIAL_DATA,SCRATCH0
	tst	SERIAL_DATA
	breq	SSS_FIELD_EEPROM_DONE ; out on null termination		
	rcall	SERIAL_SEND_BYTE
	sbr	SCRATCH2,1 ; trailing space required
	subi	SCRATCH1,4 ; field length is *4 due to its bit position
	brne	SSS_FIELD_EEPROM_LOOP
SSS_FIELD_EEPROM_DONE:
	add	SCRATCH2,SCRATCH3  ; if type is 0xff and one or more bytes sent, 
	brne	SSS_BACKTOTOP ; zero flag is set and trailing space should be sent
	ldi	SCRATCH2,' '
	mov	SERIAL_DATA,SCRATCH2
	rcall	SERIAL_SEND_BYTE
SSS_RJMP_SERIAL_SEND_STRING:
	rjmp	SERIAL_SEND_STRING ; out on end of field
		

; Send a byte in SERIAL_DATA
; Set SERIAL_SEND_DELAY to at least 11. Higher number gives pre-data deadtime.
SERIAL_SEND_BYTE_FROM_CHAR:
	mov	SERIAL_DATA,CHAR
SERIAL_SEND_BYTE:
	push	SCRATCH0
	lds	SCRATCH0,SEND_CHECKSUM_COUNTER
	cpi	SCRATCH0,PHONE_MAX_CHECKSUM_LENGTH
	brsh	SERIAL_SEND_BYTE_NO_CHECKSUM
	inc	SCRATCH0
	sts	SEND_CHECKSUM_COUNTER,SCRATCH0
	lds	SCRATCH0,SEND_CHECKSUM
	eor	SCRATCH0,SERIAL_DATA
	sts	SEND_CHECKSUM,SCRATCH0
SERIAL_SEND_BYTE_NO_CHECKSUM:
	lds	SERIAL_COUNT,SERIAL_SEND_DELAY
	sbr	FLAG_REGISTER,1<<FLAG_SERIAL_SEND
	cbr	FLAG_REGISTER,1<<FLAG_SERIAL_RECEIVE
; is there a race condition here?
	out	PCMSK0,REG_ZERO ; shut down Pin Change Interrupt while sending
	ldi	SCRATCH0,(1<<WGM12)|(1<<CS10) ; Run Timer1 from 1MHz clock, CTC mode
	out	TCCR1B,SCRATCH0 ; start the serial clock
SERIAL_SEND_BYTE_WAIT:
	tst	SERIAL_COUNT
	brne	SERIAL_SEND_BYTE_WAIT
	pop	SCRATCH0
	ret
	
SERIAL_RECEIVE_ENABLE_FROM_GPS:
	ldi	SCRATCH0,1<<RX_IN_FROM_GPS
	mov	SERIAL_PORT,SCRATCH0
	rjmp	SERIAL_RECEIVE_ENABLE
SERIAL_RECEIVE_ENABLE_FROM_PHONE:
	ldi	SCRATCH0,1<<RX_IN_FROM_PHONE
	mov	SERIAL_PORT,SCRATCH0
SERIAL_RECEIVE_ENABLE:
	cbr	FLAG_REGISTER,1<<FLAG_SERIAL_SEND ; select receive mode
	sbr	FLAG_REGISTER,1<<FLAG_SERIAL_RECEIVE ; select receive mode
	out	PCMSK0,SERIAL_PORT ; activate Pin Change Interrupt for selected input pin
	ret
; Disable serial receive, and enable setting of unsolicited message flag
SERIAL_RECEIVE_DISABLE:
	cli	; avoid race condition with receive
	ldi	SCRATCH0,1<<RX_IN_FROM_PHONE
	mov	SERIAL_PORT,SCRATCH0
	cbr	FLAG_REGISTER,(1<<FLAG_SERIAL_RECEIVE)|(1<<FLAG_SERIAL_SEND)
	out	PCMSK0,SERIAL_PORT ; activate Pin Change Interrupt for selected input pin
	out	TCCR1B,SERIAL_SCRATCH ; stop the serial clock
; Is there any condition with an interrupt flag already set where interrupt could fire
; and Pin Change Interrupt get reenabled? Need to make sure there is not.
V_WDT: ; WDT interrupt does nothing except wakes up the CPU
	reti	; not really an interrupt, but we need to turn interrupts back on


; This is the serial timer interrupt
V_TIM1_COMPA:
	in	SERIAL_INTERRUPT_SREG,SREG
	sbrs	FLAG_REGISTER,FLAG_SERIAL_SEND
	rjmp	SERIAL_RECEIVE_INTERRUPT
;
SERIAL_SEND_INTERRUPT:
; Note: stop bit is just handled as one extra count.
; Start bit is logic LOW. Idle or stop bit is logic HIGH.
; Data sent true (not inverted)
; Comparisons below: if count=10, send start bit. Carry flag = 0, sending a LOW.
; If count < 10, carry flag = 1 and will be rotated into the data register.
; After sending all eight bits, stop bit is a 1 (logic high) coming out of
; the data register which is now all 1s
	cpi	SERIAL_COUNT,10 ; >10, do nothing. =10, send start bit. <10, send data.
	breq	SERIAL_SEND_START_BIT
	brcc	SERIAL_DEC_AND_RET
SERIAL_SEND_DATA_BIT:
	ror	SERIAL_DATA ; drop low bit into carry flag, 1 from carry into high bit
SERIAL_SEND_START_BIT:
	in	SERIAL_SCRATCH,PORTA ; IN and OR do not change carry flag
	or	SERIAL_SCRATCH,SERIAL_PORT ; sets to 1 the bit for the selected port
	brcs	SERIAL_SEND_DATA_ONE
	sub	SERIAL_SCRATCH,SERIAL_PORT ; If it's a 0, set bit to zero in output
SERIAL_SEND_DATA_ONE:
	out	PORTA,SERIAL_SCRATCH
SERIAL_DEC_AND_RET:
	dec	SERIAL_COUNT
	brne	SERIAL_SEND_NOT_DONE
; serial send done
	out	TCCR1B,REG_ZERO ; stop the serial clock
SERIAL_SEND_NOT_DONE:		
	out	SREG,SERIAL_INTERRUPT_SREG
	reti
;	
; Interrupt for the first bit (long delay after start bit)
SERIAL_RECEIVE_INTERRUPT:
	in	SERIAL_SCRATCH,PINA ; get the register
	and	SERIAL_SCRATCH,SERIAL_PORT ; isolate active input pin
	neg	SERIAL_SCRATCH ; this sets carry flag unless register is zero
	ror	SERIAL_DATA ; roll the bit from carry flag into shift register
	dec	SERIAL_COUNT
	brne	SERIAL_RECEIVE_RETI ; last bit?	
; last bit
	out	TCCR1B,REG_ZERO ; stop the serial clock
	push	ZH ; borrow Z pointer
	push	ZL
	ldi	ZH,1 ; buffer is 0x100-0x1ff
	mov	ZL,SERIAL_POINTER
	st	Z,SERIAL_DATA
	inc	SERIAL_POINTER ; bump pointer after saving; pointer indicates next byte
	pop	ZL
	pop	ZH
	out	PCMSK0,SERIAL_PORT ; enable Pin Change Interrupt for next character
SERIAL_RECEIVE_RETI:
	out	SREG,SERIAL_INTERRUPT_SREG
	reti
	
; Pin Change Interrupt 0 used to detect start bit on receive
V_PCINT0:
	in	SERIAL_INTERRUPT_SREG,SREG
	in	SERIAL_SCRATCH,PINA ; get the register
	and	SERIAL_SCRATCH,SERIAL_PORT ; isolate active input pin
	brne	PCINT0_IGNORE ; bit should be zero if falling edge, otherwise ignore
	sbrs	FLAG_REGISTER,FLAG_SERIAL_RECEIVE ; receive mode enabled?
	rjmp	PCINT0_UNSOLICITED_MESSAGE ; no

; When we receive the pin change interrupt, we are at the falling edge of the start bit
; We want to sample data bits in the middle of the bit period. Therefore we need to
; set the clock for the first cycle to 1.5 * the bit period, which is the remainder
; of the start bit plus half the data bit period. This code presets the timer to
; a high value equal to 20 - ( 1/2 the baud rate count )
; The 20 is there to compensate for the interval between receiving the pin change
; interrupt and starting the serial clock. The 16-bit subtract will underflow to a
; high value. When we start the counter, it will roll over through zero to its normal
; value, producing a longer delay.
; 20 - ( 208 / 2 ) = 65452 in 16-bit arithmetic
; Pin change interrupt is in the middle of this, only because I had a zero available.
	in	SERIAL_DATA,OCR1AH ; high byte of baud rate
	in	SERIAL_SCRATCH,OCR1AL ; low byte of baud rate
	lsr	SERIAL_DATA ; divide high byte by two
	ror	SERIAL_SCRATCH ; complete division by two
	ldi	SERIAL_COUNT,20 ; number of cycles to compensate
	sub	SERIAL_COUNT,SERIAL_SCRATCH ; compute num cycles - ( baud / 2 ) (low byte)
	clr	SERIAL_SCRATCH ; does not touch carry flag
	out	PCMSK0,SERIAL_SCRATCH ; shut down Pin Change Interrupt while receiving
	sbc	SERIAL_SCRATCH,SERIAL_DATA ; complete subtraction for high byte
	out	TCNT1H,SERIAL_SCRATCH ; high byte first
	out	TCNT1L,SERIAL_COUNT ; now the low byte
	
	ldi	SERIAL_COUNT,(1<<WGM12)|(1<<CS10) ; Start the serial clock
	out	TCCR1B,SERIAL_COUNT ; using Serial Count because we can LDI it
	ldi	SERIAL_COUNT,8 ; get ready to receive the byte
PCINT0_IGNORE:
	out	SREG,SERIAL_INTERRUPT_SREG
	reti
PCINT0_UNSOLICITED_MESSAGE:
	sbr	FLAG_REGISTER,1<<FLAG_NEED_TO_POLL_MESSAGES
	rjmp	PCINT0_IGNORE
	

; Send the blink code in SCRATCH0
; Small risk of a race condition here that would repeat a code, if the timer interrupt
; goes off and finishes a code during this routine. If it bothers you, suspend the
; timer interrupt during this routine.
SEND_BLINK_CODE:
	lds	XL,BLINK_CODES_INDEX
	push	XL
	swap	XL
	andi	XL,7 ; high halfbyte moved to low and isolated first three bits
	subi	XL,0-low(BLINK_CODES) ; offset into buffer
	ldi	XH,high(BLINK_CODES) ; assumes BLINK_CODES not on 256-byte boundary
	st	X,SCRATCH0 
	pop	XL
	subi	XL,-16 ; increment INSERT pointer
	andi	XL,0x77 ; mask high bits
	sts	BLINK_CODES_INDEX,XL
	ret
	

; Timer0 interrupt happens 4X per second at 1MHz
; 256 counts * 1024 prescaler
; This is interruptable by the serial I/O
; Interrupt handler increments the count and sends blink codes
V_TIM0_OVF:
	sei	; enable interrupts for serial I/O
	in	TIMER_INTERRUPT_SREG,SREG
	push	ZH
	push	ZL
	push	SCRATCH0
;
	tst	DELAY
	breq	CLK_DELAY_AT_ZERO
	dec	DELAY
CLK_DELAY_AT_ZERO:
;
	lds	SCRATCH0,SUBSECONDS_COUNT
	inc	SCRATCH0
	sts	SUBSECONDS_COUNT,SCRATCH0
	mov	ZL,SCRATCH0 ; for blink codes below
	andi	SCRATCH0,15 ; for 1MHz
	brne	CLK_NOT_FOUR_SECONDS
; increment every four seconds stopping at 0xff
	lds	SCRATCH0,GPS_TIMER
	inc	SCRATCH0
	brne	CLK_NOT_ROLLOVER
	dec	SCRATCH0
CLK_NOT_ROLLOVER:
	sts	GPS_TIMER,SCRATCH0
CLK_NOT_FOUR_SECONDS:

;
; generate blink codes (green, then red)
	sbrc	ZL,0 ; skip alternate clock cycles (1MHz)
	rjmp	CLK_BLINK_OUT
	bst	ZL,1 ; T flag now indicates On/Off phase of blink cycle	
; High-order halfbyte is INSERT pointer.  Low-order is SHOW pointer.
; Load the current blink code.
	lds	ZL,BLINK_CODES_INDEX
	mov	ZH,ZL
	swap	ZL
	andi	ZH,7
	andi	ZL,7
	cp	ZH,ZL
	breq	CLK_BLINK_OUT ; if INSERT and SHOW pointers are the same, no codes
	ldi	ZL,low(BLINK_CODES) ; low order address of codes array
	add	ZL,ZH ; add pointer offset
	ldi	ZH,high(BLINK_CODES) ; assumption that BLINK_CODES not on 256-byte boundary
	ld	SCRATCH0,Z ; get the blink code
	brtc	CLK_BLINK_OFF
;
CLK_BLINK_ON:
	cpi	SCRATCH0,16 ; any green (first digit) remaining?
	brlo	CLK_BLINK_ON_SECOND
	sbi	PORTB,LED_OUT_GREEN ; light on green
	subi	SCRATCH0,16 ; decrement the first digit count
	st	Z,SCRATCH0 
	rjmp	CLK_BLINK_OUT
;
CLK_BLINK_ON_SECOND:
	tst	SCRATCH0 ; any red (second digit) remaining?
	breq	CLK_BLINK_OUT ; no
	sbi	PORTB,LED_OUT_RED ; light on red
	subi	SCRATCH0,1 ; decrement the second digit count
	st	Z,SCRATCH0
	rjmp	CLK_BLINK_OUT
;
CLK_BLINK_OFF:
	cbi	PORTB,LED_OUT_GREEN
	cbi	PORTB,LED_OUT_RED ; light off
	tst	SCRATCH0 ; code finished?
	brne	CLK_BLINK_OUT ; no
	lds	SCRATCH0,BLINK_CODES_INDEX ; increment SHOW pointer
	inc	SCRATCH0
	andi	SCRATCH0,0x77 ; mask high bits of both halfbytes
	sts	BLINK_CODES_INDEX,SCRATCH0 ; save SHOW pointer
CLK_BLINK_OUT:
;
	pop	SCRATCH0
	pop	ZL
	pop	ZH
	out	SREG,TIMER_INTERRUPT_SREG
	reti

; prints a 16-bit number in decimal
; input in xh:xl
; load SCRATCH0 with 1 for xxx.x format (one after decimal)
; load SCRATCH0 with zero for straight integer
; SCRATCH0 must be 0 or 1
PRINT_DECIMAL:
	push	ZH
	push	ZL
;
	ldi	ZH,high(PD_TABLE<<1)
	ldi	ZL,low(PD_TABLE<<1)
PD_DIGITLOOP:
	ldi	CHAR,'0'
	lpm	SCRATCH1,Z+ ; low
	cpi	SCRATCH1,0xfe
	breq	PD_OUT
	brcc	PD_LASTDIGIT
	lpm	SCRATCH2,Z+ ; high
PD_COUNTLOOP:
	sub	XL,SCRATCH1
	sbc	XH,SCRATCH2
	brcs	PD_UNDERFLOW
	inc	CHAR
	sbr	SCRATCH0,1<<1 ; set leading zero suppresion flag
	rjmp	PD_COUNTLOOP
PD_UNDERFLOW:
	add	XL,SCRATCH1
	adc	XH,SCRATCH2
PD_SEND:
	sbrc	SCRATCH0,1 ; suppress leading zeroes
	rcall	SERIAL_SEND_BYTE_FROM_CHAR
	rjmp	PD_DIGITLOOP
PD_LASTDIGIT:
	cpi	SCRATCH0,1 ; xxx.x format and zero so far
	brne	PD_NO_EXTRA_ZERO
	ldi	CHAR,'0'
	rcall	SERIAL_SEND_BYTE_FROM_CHAR
PD_NO_EXTRA_ZERO:
	sbr	SCRATCH0,1<<1 ; enable printing of a single zero if value is 0
	ldi	CHAR,'.'
	sbrc	SCRATCH0,0
	rcall	SERIAL_SEND_BYTE_FROM_CHAR ; print decimal point in xxx.x mode
	rjmp	PD_DIGITLOOP
PD_OUT:
	pop	ZL
	pop	ZH
	ret
; constants 10000,1000,100,10,1 little endian, 0xff before last digit, 0xfe marks end
PD_TABLE:
	.db	0x10,0x27,0xe8,0x03,0x64,0x00,0x0a,0x00,0xff,0x01,0x00,0xfe


; Parse decimal string in XH:XL into 16-bit number in SCRATCH1:SCRATCH0 (SCRATCH0 low)
; PAT gives max length of string
; DELIM should be '.' if you want to parse decimal point,
; or 0 if you want to skip decimal point and treat decimal string as integer (45.123 -> 45123)
; or 0xff if you want to terminate on decimal point
; If decimal . found and DELIM set to '.' parse one fractional digit, so "45.67" becomes 456
; SCRATCH0123 and CHAR used
PARSE_DECIMAL:
	clt	; T flag marks last digit after '.'
	clr	SCRATCH0
	clr	SCRATCH1
PSD_LOOP:
; Get digit
	ld	CHAR,X+
	tst	DELIM
	brne	PSD_NOT_SKIP_DECIMAL
	cpi	CHAR,'.' ; skip decimal mode
	breq	PSD_NEXTCHAR
PSD_NOT_SKIP_DECIMAL:
	cp	CHAR,DELIM
	brne	PSD_NOT_DECIMAL
	set
	rjmp	PSD_NEXTCHAR
PSD_NOT_DECIMAL:
	subi	CHAR,'0'
	brlo	PSD_DONE
	cpi	CHAR,10
	brsh	PSD_DONE
; multiply by 10
	rcall	MULX10_SCRATCH10
; add digit
	clr	SCRATCH2
	add	SCRATCH0,CHAR
	adc	SCRATCH1,SCRATCH2
;
	brts	PSD_DONE ; out if decimal and one more digit
PSD_NEXTCHAR:
	dec	PAT
	brne	PSD_LOOP
PSD_DONE:
	ret

; multiply by 10 ( X*10 = X*2 + X*8 )
MULX10_SCRATCH10:
	lsl	SCRATCH0
	rol	SCRATCH1
	mov	SCRATCH3,SCRATCH0
	mov	SCRATCH2,SCRATCH1
	lsl	SCRATCH3
	rol	SCRATCH2
	lsl	SCRATCH3
	rol	SCRATCH2
	add	SCRATCH0,SCRATCH3
	adc	SCRATCH1,SCRATCH2
	ret

; Called to parse and execute SETNAME command
; Expected: one parameter
; Saves device name to EEPROM
; Called while serial comm is running
CMD_SETNAME:
	rcall	CLEAR_PARSE_FIELDS
	ldi	ZH,high(SETNAME_PARSE_STRING<<1)
	ldi	ZL,low(SETNAME_PARSE_STRING<<1)
	cbr	FLAG_REGISTER,1<<FLAG_PARSE_NMEA
	ldi	CHAR,' '
	mov	DELIM,CHAR
	rcall	PARSE_NMEA_OR_COMMAND_LINE 
;
	ldi	ZH,high(FLDADR_DEVICENAME)
	ldi	ZL,low(FLDADR_DEVICENAME)
	ldi	XH,high(EEADR_DEVICENAME)
	ldi	XL,low(EEADR_DEVICENAME)
	ldi	SCRATCH0,FLDLEN_DEVICENAME
CSN_WRITE_LOOP:
	ld	CHAR,Z+
	rcall	EEPROM_WRITEBYTE
	dec	SCRATCH0
	brne	CSN_WRITE_LOOP ; field was already null terminated or correct length
;
	rjmp	CMD_SAVE_COMMAND_EXECUTED_AND_RET ; reply with COMMAND EXECUTED message
	

; Called to parse and execute SETADDRESS command
; Expected: one or two parameters, phone number and optionally email address
; Saves phone number and email address to EEPROM
; Called while serial comm is running
; SALOCATE variant does a LOCATE after completion
CMD_SALOCATE:
	rcall	CMD_LOCATE
	rjmp	CSA_COMMON
CMD_SETADDRESS:
	rcall	CMD_SAVE_COMMAND_EXECUTED_AND_RET
CSA_COMMON:
	rcall	SKIP_WHITESPACE ; skip the space after the command
	ldi	XH,high(EEADR_PHONE_NUMBER)
	ldi	XL,low(EEADR_PHONE_NUMBER)
	ldi	SCRATCH0,EELEN_PHONE_NUMBER
	rcall	EEPROM_WRITE_FIELD ; write phone number
	ldi	XH,high(EEADR_EMAIL_ADDR)
	ldi	XL,low(EEADR_EMAIL_ADDR)
; Is there another field for the email address?
CSA_SKIP:
	rcall	GET_NEXT_CHAR
	brts	CSA_DONE
	cpi	CHAR,32 ; space?
	brlo	CSA_NO_EMAIL ; lower than space must be CR, so no email addr
	breq	CSA_SAVE_EMAIL ; space means email address follows
	rjmp	CSA_SKIP ; above space - keep skipping characters
CSA_SAVE_EMAIL:
	ldi	SCRATCH0,EELEN_EMAIL_ADDR
	rcall	EEPROM_WRITE_FIELD ; write email address
	rjmp	CSA_DONE
CSA_NO_EMAIL:
	clr	CHAR ; zero out email address
	rcall	EEPROM_WRITEBYTE
CSA_DONE:
	ret

; Set up GPS locate to happen after the message processing
CMD_LOCATE:
	ldi	SCRATCH1,high(GPS_LOCATE_AND_SEND)
	ldi	SCRATCH0,low(GPS_LOCATE_AND_SEND)
	rjmp	CMD_SAVE_PENDING_AND_RET

; Called to parse and execute SETPASSWORD command
; Expected: two parameters, which are two copies of the password
; Called while serial I/O is running
CMD_SETPASSWORD:
	rcall	CLEAR_PARSE_FIELDS
	ldi	ZH,high(SETPASSWORD_PARSE_STRING<<1)
	ldi	ZL,low(SETPASSWORD_PARSE_STRING<<1)
	cbr	FLAG_REGISTER,1<<FLAG_PARSE_NMEA
	ldi	CHAR,' '
	mov	DELIM,CHAR
	rcall	PARSE_NMEA_OR_COMMAND_LINE
; compare the two password fields
	ldi	XH,high(FLDADR_PASSWORD1)
	ldi	XL,low(FLDADR_PASSWORD1)
	ldi	ZH,high(FLDADR_PASSWORD2)
	ldi	ZL,low(FLDADR_PASSWORD2)
	ldi	SCRATCH0,FLDLEN_PASSWORD1
CSP_COMPARE_LOOP:
	ld	SCRATCH1,X+
	ld	SCRATCH2,Z+
	cp	SCRATCH1,SCRATCH2
	brne	CSP_PASSWORD_NOGOOD
	tst	SCRATCH1
	breq	CSP_COMPARE_GOOD
	dec	SCRATCH0
	brne	CSP_COMPARE_LOOP
CSP_COMPARE_GOOD:
	cpi	SCRATCH0,FLDLEN_PASSWORD1-MIN_PASSWORD_LENGTH+1 ; password long enough?
	brsh	CSP_PASSWORD_NOGOOD ; too short
; write password to EEPROM
	ldi	ZH,high(FLDADR_PASSWORD1)
	ldi	ZL,low(FLDADR_PASSWORD1)
	ldi	XH,high(EEADR_PASSWORD)
	ldi	XL,low(EEADR_PASSWORD)
	ldi	SCRATCH0,FLDLEN_PASSWORD1
CSP_WRITE_LOOP:
	ld	CHAR,Z+
	rcall	EEPROM_WRITEBYTE
	dec	SCRATCH0
	brne	CSP_WRITE_LOOP ; field was already null terminated or correct length
;
	ldi	SCRATCH1,high(MULTI_SEND_PASSWORD_CHANGED)
	ldi	SCRATCH0,low(MULTI_SEND_PASSWORD_CHANGED)
	rjmp	CMD_SAVE_PENDING_AND_RET
CSP_PASSWORD_NOGOOD:
	ldi	SCRATCH1,high(MULTI_SEND_PASSWORD_CHANGE_FAILED)
	ldi	SCRATCH0,low(MULTI_SEND_PASSWORD_CHANGE_FAILED)
	rjmp	CMD_SAVE_PENDING_AND_RET

; Turn off the tracking mode
; This is also called at startup to set default parameters
CMD_POWERON:
CMD_TRACKOFF:
	ldi	SCRATCH1,high(MULTI_SEND_COMMAND_EXECUTED)
	ldi	SCRATCH0,low(MULTI_SEND_COMMAND_EXECUTED)
	rcall	CMD_SAVE_PENDING_AND_RET
CMD_TRACKOFF_ATSTART:
	ldi	SCRATCH0,high(DEFAULT_PHONE_POLL_INTERVAL)
	sts	POLL_INTERVAL,SCRATCH0
	ldi	SCRATCH0,low(DEFAULT_PHONE_POLL_INTERVAL)
	sts	POLL_INTERVAL+1,SCRATCH0
	ldi	SCRATCH0,TRACK_STATE_OFF
	sts	TRACK_STATE,SCRATCH0
	ret

; Turn on the tracking mode
; No reply sent here. Locate should be sent shortly.
CMD_TRACKON:
	sts	POLL_INTERVAL,REG_ZERO
	sts	POLL_INTERVAL+1,REG_ZERO ; should re-poll immediately
	sts	FASTEST_SPEED,REG_ZERO
	sts	FASTEST_SPEED+1,REG_ZERO
	ldi	SCRATCH0,TRACK_STATE_INITIAL
	sts	TRACK_STATE,SCRATCH0
	ldi	SCRATCH0,0xf0
	sts	TRACK_COUNT,SCRATCH0 ; make sure it notifies immediately
	sts	MOVING_LOCATE_COUNT,SCRATCH0 ; whether moving or stopped
	ret

CMD_POWERSAVE:
	sts	POLL_INTERVAL,REG_ZERO
	sts	POLL_INTERVAL+1,REG_ZERO ; should re-poll immediately
	ldi	SCRATCH0,TRACK_STATE_POWERSAVE_PHONE_ON
	sts	TRACK_STATE,SCRATCH0
	rjmp	CMD_SAVE_COMMAND_EXECUTED_AND_RET

CMD_SETSPEED:
	ldi	ZH,high(SETSPEED_DECODE_STRING<<1)
	ldi	ZL,low(SETSPEED_DECODE_STRING<<1)
	rjmp	CMD_SETTRACK_SETPOWER_SETSPEED
CMD_SETTRACK:
	ldi	ZH,high(SETTRACK_DECODE_STRING<<1)
	ldi	ZL,low(SETTRACK_DECODE_STRING<<1)
	rjmp	CMD_SETTRACK_SETPOWER_SETSPEED
CMD_SETPOWER:
	ldi	ZH,high(SETPOWER_DECODE_STRING<<1)
	ldi	ZL,low(SETPOWER_DECODE_STRING<<1)
CMD_SETTRACK_SETPOWER_SETSPEED:
	rcall	SAVE_SETTINGS
; Generate and send settings and status message
CMD_STATUS:
	ldi	SCRATCH1,high(MULTI_SEND_STATUS)
	ldi	SCRATCH0,low(MULTI_SEND_STATUS)
	rjmp	CMD_SAVE_PENDING_AND_RET

; Reject unrecognized command
CMD_UNRECOGNIZED:
	rcall	SKIP_TO_NEXT_LINE
#ifdef	DEBUG_KEEP_INVALID_MESSAGES
	ldi	SCRATCH0,0xff
	sts	DEBUG_INVALID_MESSAGE_FLAG,SCRATCH0
	ret
#endif
	ldi	SCRATCH1,high(MULTI_SEND_UNKNOWN_COMMAND)
	ldi	SCRATCH0,low(MULTI_SEND_UNKNOWN_COMMAND)
	rjmp	CMD_SAVE_PENDING_AND_RET

; Reboot or reinit the system
; Runs during command loop
CMD_REINIT:
	ldi	SCRATCH1,high(CMD_REINIT_DO)
	ldi	SCRATCH0,low(CMD_REINIT_DO)
	rjmp	CMD_SAVE_PENDING_AND_RET
CMD_REBOOT:
	ldi	SCRATCH1,high(CMD_REBOOT_DO)
	ldi	SCRATCH0,low(CMD_REBOOT_DO)
	rjmp	CMD_SAVE_PENDING_AND_RET

CMD_SAVE_COMMAND_EXECUTED_AND_RET:
	ldi	SCRATCH1,high(MULTI_SEND_COMMAND_EXECUTED)
	ldi	SCRATCH0,low(MULTI_SEND_COMMAND_EXECUTED)
CMD_SAVE_PENDING_AND_RET:
	sts	COMMAND_PENDING,SCRATCH1
	sts	COMMAND_PENDING+1,SCRATCH0
	ret


; Reboot or reinit the system
; Runs after command loop
CMD_REINIT_DO:
	rcall	CLEAR_EEPROM_FLAG
CMD_REBOOT_DO:
	rcall	MULTI_SEND_COMMAND_EXECUTED
#ifdef	RESET_PHONE_PERIODICALLY
	ldi	DELAY,(PHONE_RESET_WAKEUP_DELAY * 4)
	rcall	SAFE_DELAY ; wait for message to be sent
	rcall	PHONE_RESET
#endif
CLEAR_RAM_AND_REBOOT:
	cli
	wdr
	ldi	SCRATCH1,high(SRAM_SIZE)
	ldi	SCRATCH0,low(SRAM_SIZE)
	ldi	ZH,high(SRAM_START)
	ldi	ZL,low(SRAM_START)
CMD_REBOOT_LOOP:
	st	Z+,REG_ZERO ; erase all SRAM
	dec	SCRATCH0
	brne	CMD_REBOOT_LOOP
	dec	SCRATCH1
	brne	CMD_REBOOT_LOOP
	rjmp	V_RESET ; bye-bye
CLEAR_EEPROM_FLAG:
	ldi	XH,high(EEADR_CHECKCODE)
	ldi	XL,low(EEADR_CHECKCODE)
	clr	CHAR
	rcall	EEPROM_WRITEBYTE
	rcall	EEPROM_WRITEBYTE ; wipe out the EEPROM good flag
	ret

; Cause a watchdog reset for testing (deliberately hang the machine here)
CMD_CAUSEWDR:
	cli
	rjmp	CMD_CAUSEWDR


; Activate GPS and get location
; Result code will be in SCRATCH0 as well as saved in memory
GPS_LOCATE:
	sts	FLDADR_GPS_LINE_COUNT_FLAGS,REG_ZERO ; count of lines after fix valid
	sts	GPS_TIMER,REG_ZERO ; it counts up at 4-second intervals
;
	cbi	PORTB,GPS_POWER_OUT ; GPS power on
	ldi	YH,high(RECEIVE_BUFFER)
	mov	YL,SERIAL_POINTER ; dump all characters in buffer
	ldi	SCRATCH0,GPS_DATASTREAM_WAIT_TIME
	sts	RECEIVE_TIMEOUT,SCRATCH0
	rcall	CLEAR_PARSE_FIELDS
	rcall	SERIAL_RECEIVE_ENABLE_FROM_GPS
;
GPS_NEXT_LINE:
	wdr
	ldi	ZH,high(GPS_MATCH_PATTERN<<1)
	ldi	ZL,low(GPS_MATCH_PATTERN<<1)
	clt	; clear the error flag 
	rjmp	BRANCH_ON_STRING
;
GPS_PARSE_OTHER:
	brts	GPS_DATASTREAM_TIMEOUT
	rcall	SKIP_TO_NEXT_LINE
	rjmp	GPS_CHECK_TIMEOUT_AND_NEXT_LINE
;
GPS_DATASTREAM_TIMEOUT:
	ldi	SCRATCH0,GPS_STATUS_DATASTREAM_TIMEOUT
	rjmp	GPS_SAVE_AND_OUT
;
GPS_PARSE_GPRMC:
	ldi	ZH,high(GPRMC_PARSE_STRING<<1)
	ldi	ZL,low(GPRMC_PARSE_STRING<<1)
	rjmp	GPS_PARSE_GO	
;	
GPS_PARSE_GPGGA:
	ldi	ZH,high(GPGGA_PARSE_STRING<<1)
	ldi	ZL,low(GPGGA_PARSE_STRING<<1)
;	rjmp	GPS_PARSE_GO	
;
GPS_PARSE_GO:
	mov	YL,BASE ; parse whole string for checksum (BASE from BRANCH_ON_STRING)
	sbr	FLAG_REGISTER,1<<FLAG_PARSE_NMEA
	ldi	CHAR,','
	mov	DELIM,CHAR
	rcall	PARSE_NMEA_OR_COMMAND_LINE
	rcall	SKIP_WHITESPACE
	brts	GPS_DATASTREAM_TIMEOUT
	cp	CHECKSUM,VERIFY_CHECKSUM
	brne	GPS_FIX_NOT_VALID ; checksum invalid, keep trying
	lds	SCRATCH0,FLDADR_GPRMC_FIXVALID
	cpi	SCRATCH0,'A' ; see if fix is valid yet
	brne	GPS_FIX_NOT_VALID
	lds	SCRATCH0,FLDADR_GPGGA_FIXVALID
	cpi	SCRATCH0,'1'
	brsh	GPS_FIX_VALID
GPS_FIX_NOT_VALID:

GPS_CHECK_TIMEOUT_AND_NEXT_LINE:
; check timeout
	ldi	XH,high(EEADR_GPS_FIX_WAIT_TIME)
	ldi	XL,low(EEADR_GPS_FIX_WAIT_TIME)
	lds	SCRATCH1,TRACK_STATE
	cpi	SCRATCH1,TRACK_STATE_BLOCKED
	brne	GPS_NOT_BLOCKED_1
	ldi	XL,low(EEADR_BLOCKED_GPS_FIX_WAIT_TIME) ; same page assumption
GPS_NOT_BLOCKED_1:
	rcall	EEPROM_READBYTE_X_TO_SCRATCH0
	lds	SCRATCH1,GPS_TIMER
	cp	SCRATCH1,SCRATCH0
	brlo	GPS_NEXT_LINE 
; timeout
	ldi	SCRATCH0,GPS_STATUS_FIX_TIMEOUT
	rjmp	GPS_SAVE_AND_OUT

GPS_FIX_VALID:
; Do we have enough lines? (let fix stabilize)
	lds	SCRATCH0,FLDADR_GPS_LINE_COUNT_FLAGS
	inc	SCRATCH0
	sts	FLDADR_GPS_LINE_COUNT_FLAGS,SCRATCH0 ; count of lines after fix valid
	andi	SCRATCH0,0x3f ; bit 7 = moving slowly last time, bit 6 = displaced
	cpi	SCRATCH0,GPS_LINES_AFTER_FIX ; got enough?
	brlo	GPS_CHECK_TIMEOUT_AND_NEXT_LINE ; keep looking
; Is it four or better sat fix?
	ldi	XH,high(FLDADR_GPGGA_SATS)
	ldi	XL,low(FLDADR_GPGGA_SATS)
	ldi	SCRATCH0,FLDLEN_GPGGA_SATS
	mov	PAT,SCRATCH0
	ldi	SCRATCH0,0xff
	mov	DELIM,SCRATCH0 ; do not parse .
	rcall	PARSE_DECIMAL
	cpi	SCRATCH0,4
	brsh	GPS_FIX_GOOD
; Is it time to accept a less than 4 sat fix?
	lds	SCRATCH1,TRACK_STATE
	cpi	SCRATCH1,TRACK_STATE_BLOCKED
	breq	GPS_FIX_GOOD ; always accept 3-sat fix if previously blocked
;	
	ldi	XH,high(EEADR_GPS_FOUR_SAT_WAIT_TIME)
	ldi	XL,low(EEADR_GPS_FOUR_SAT_WAIT_TIME)
	rcall	EEPROM_READBYTE_X_TO_SCRATCH0
	lds	SCRATCH1,GPS_TIMER
	cp	SCRATCH1,SCRATCH0
	brsh	GPS_FIX_GOOD
	rjmp	GPS_CHECK_TIMEOUT_AND_NEXT_LINE

GPS_FIX_GOOD:
; If course is null, put in a zero
	lds	SCRATCH0,FLDADR_GPRMC_COURSE
	tst	SCRATCH0
	brne	GPS_COURSE_NOT_NULL
	ldi	SCRATCH0,'0'
	sts	FLDADR_GPRMC_COURSE,SCRATCH0
	sts	FLDADR_GPRMC_COURSE+1,REG_ZERO
GPS_COURSE_NOT_NULL:
; Check displaced and retry the first time if so
; GPS sometimes returns a wild fix, causing a lot of false "moved" reports
	lds	SCRATCH0,TRACK_STATE
	cpi	SCRATCH0,TRACK_STATE_MOVING
	brsh	GPS_DISPLACED_OUT ; only care about displacement in stopped and blocked
;
	ldi	XH,high(FLDADR_GPRMC_LATITUDE+2)
	ldi	XL,low(FLDADR_GPRMC_LATITUDE+2)
	ldi	ZH,high(LAST_GOOD_FIX_LATITUDE+2)
	ldi	ZL,low(LAST_GOOD_FIX_LATITUDE+2)
	rcall	CHECK_FOR_DISPLACEMENT ; check for latitude displacement
	brcc	GPS_DISPLACED ; moved since last fix
;
	ldi	XH,high(FLDADR_GPRMC_LONGITUDE+3)
	ldi	XL,low(FLDADR_GPRMC_LONGITUDE+3)
	ldi	ZH,high(LAST_GOOD_FIX_LONGITUDE+3)
	ldi	ZL,low(LAST_GOOD_FIX_LONGITUDE+3)
	rcall	CHECK_FOR_DISPLACEMENT ; check for longitude displacement
	brcc	GPS_DISPLACED ; moved since last fix
; not displaced
	lds	SCRATCH0,FLDADR_GPS_LINE_COUNT_FLAGS ; bit 6 is displaced flag
	cbr	SCRATCH0,1<<GPS_FLAG_DISPLACED ; clear displaced flag
	sts	FLDADR_GPS_LINE_COUNT_FLAGS,SCRATCH0
	rjmp	GPS_DISPLACED_OUT
;
GPS_DISPLACED:
	lds	SCRATCH0,FLDADR_GPS_LINE_COUNT_FLAGS ; bit 6 is displaced flag
	sbrc	SCRATCH0,GPS_FLAG_DISPLACED
	rjmp	GPS_DISPLACED_OUT ; displacement flag already set, so leave it set
	sbr	SCRATCH0,1<<GPS_FLAG_DISPLACED ; set displaced flag
GPS_MASK_FLAGS_AND_RETRY:
	andi	SCRATCH0,0xC0 ; Clear counter but keep flags
	sts	FLDADR_GPS_LINE_COUNT_FLAGS,SCRATCH0
	sts	GPS_TIMER,REG_ZERO ; avoid timing out
	rjmp	GPS_NEXT_LINE ; get more data
GPS_DISPLACED_OUT:
;
; Parse altitude
	ldi	XH,high(FLDADR_GPGGA_ALT)
	ldi	XL,low(FLDADR_GPGGA_ALT)
	ldi	SCRATCH0,FLDLEN_GPGGA_ALT
	mov	PAT,SCRATCH0
	ldi	SCRATCH0,0xff
	mov	DELIM,SCRATCH0 ; do not parse decimal point
	rcall	PARSE_DECIMAL
;
#ifdef	ALT_IN_FEET
	ldi	XH,high(CF_M_FT)
	ldi	XL,low(CF_M_FT)
	rcall	UNITS_CONVERT_X4 ; multiplies SCRATCH1:SCRATCH0 by four
	sts	FLDADR_GPGGA_ALT_NUMERIC,ZH
	sts	FLDADR_GPGGA_ALT_NUMERIC+1,ZL
#endif
;
#ifdef	ALT_IN_METERS
	sts	FLDADR_GPGGA_ALT_NUMERIC,SCRATCH1
	sts	FLDADR_GPGGA_ALT_NUMERIC+1,SCRATCH0
#endif
;
; Parse speed
	ldi	XH,high(FLDADR_GPRMC_SPEED)
	ldi	XL,low(FLDADR_GPRMC_SPEED)
	ldi	SCRATCH0,FLDLEN_GPRMC_SPEED
	mov	PAT,SCRATCH0
	ldi	SCRATCH0,'.'
	mov	DELIM,SCRATCH0 ; parse decimal point
	rcall	PARSE_DECIMAL ; parsing decimal speed
;
#ifdef	SPEED_IN_MPH
	ldi	XH,high(CF_KNOT_MPH)
	ldi	XL,low(CF_KNOT_MPH)
	rcall	UNITS_CONVERT_X2 ; multiplies SCRATCH1:SCRATCH0 by two
	sts	FLDADR_GPRMC_SPEED_NUMERIC,ZH
	sts	FLDADR_GPRMC_SPEED_NUMERIC+1,ZL
#endif
;
#ifdef	SPEED_IN_KPH
	ldi	XH,high(CF_KNOT_KPH)
	ldi	XL,low(CF_KNOT_KPH)
	rcall	UNITS_CONVERT_X2 ; multiplies SCRATCH1:SCRATCH0 by two
	sts	FLDADR_GPRMC_SPEED_NUMERIC,ZH
	sts	FLDADR_GPRMC_SPEED_NUMERIC+1,ZL
#endif
;
#ifdef	SPEED_IN_KNOTS
	sts	FLDADR_GPRMC_SPEED_NUMERIC,SCRATCH1
	sts	FLDADR_GPRMC_SPEED_NUMERIC+1,SCRATCH0
	mov	ZH,SCRATCH1
	mov	ZL,SCRATCH0
#endif
;
; Check moving/stopped status
	ldi	XH,high(EEADR_GPS_MOVING_THRESHOLD)
	ldi	XL,low(EEADR_GPS_MOVING_THRESHOLD)
	rcall	EEPROM_READWORD_X_TO_SCRATCH10 ; assuming these two are adjacent
; SCRATCH1 = MOVING_THRESHOLD, SCRATCH0 = MOVING_RETRY_THRESHOLD
	tst	ZH ; high byte
	brne	GPS_MOVING
	cp	ZL,SCRATCH0 ; high enough to trust on first try
	brsh	GPS_MOVING
	cp	ZL,SCRATCH1 ; above moving threshold
	brlo	GPS_NOT_MOVING
	lds	SCRATCH0,FLDADR_GPS_LINE_COUNT_FLAGS
	sbrc	SCRATCH0,GPS_FLAG_LOW_SPEED_RETRY ; test high bit flag
	rjmp	GPS_MOVING ; already retried once so accept as moving
; Here GPS reported a low speed, but fast enough to report as moving.
; To avoid false motion reports in tracking mode, we go back and get
; more lines. High bit flag causes us to only backtrack once per locate.
	sbr	SCRATCH0,1<<GPS_FLAG_LOW_SPEED_RETRY
	rjmp	GPS_MASK_FLAGS_AND_RETRY ; we have the same thing above, so reuse it
;	andi	SCRATCH0,0xC0 ; Clear counter but keep flags
;	sts	FLDADR_GPS_LINE_COUNT_FLAGS,SCRATCH0 ; store new value
;	sts	GPS_TIMER,REG_ZERO ; avoid timing out
;	rjmp	GPS_NEXT_LINE
;
GPS_MOVING:
	ldi	SCRATCH0,GPS_STATUS_FIX_MOVING
	rjmp	GPS_SAVE_AND_OUT
GPS_NOT_MOVING:
	ldi	SCRATCH0,GPS_STATUS_FIX_STOPPED
GPS_SAVE_AND_OUT:
	sts	GPS_STATUS,SCRATCH0
	sbi	PORTB,GPS_POWER_OUT ; GPS power off
; Save last good fix if we got a valid fix
	cpi	SCRATCH0,GPS_STATUS_FIX_STOPPED
	brlo	GPS_SLGF_OUT ; don't save invalid fix data
	ldi	XH,high(FLDADR_GPRMC_TIME)
	ldi	XL,low(FLDADR_GPRMC_TIME)
	ldi	ZH,high(LAST_GOOD_FIX)
	ldi	ZL,low(LAST_GOOD_FIX)
	ldi	SCRATCH1,LAST_GOOD_FIX_LEN
GPS_SLGF_LOOP:
	ld	SCRATCH2,X+
	st	Z+,SCRATCH2
	dec	SCRATCH1
	brne	GPS_SLGF_LOOP
GPS_SLGF_OUT:
	ret


; Checks the current fix against the LAST GOOD fix to see if we have moved between fixes
; Call with current fix address (+2 or +3 for decimal part) in X
; and last known good fix address (+2 or +3 for decimal part) in Z
; Carry set if we have NOT moved
CHECK_FOR_DISPLACEMENT:
; first get current lat/lon
	push	ZH
	push	ZL ; last known good address
	ldi	SCRATCH2,6 ; length = six characters (xx.xxx)
	mov	PAT,SCRATCH2
	clr	DELIM ; skip . and treat as integer
	rcall	PARSE_DECIMAL
;
	pop	XL
	pop	XH ; get back last known good address
	push	SCRATCH1
	push	SCRATCH0 ; store current lat/lon value
; now get last good fix lat/lon
	ldi	SCRATCH2,6 ; length = six characters (xx.xxx)
	mov	PAT,SCRATCH2
	clr	DELIM ; skip . and treat as integer
	rcall	PARSE_DECIMAL
	pop	SCRATCH2
	pop	SCRATCH3 ; get current back into SCRATCH3:2
; Now SCRATCH3:SCRATCH2 has current and SCRATCH1:SCRATCH0 has last known good
; Subtract smaller from larger, result ending up in SCRATCH3:SCRATCH2
	cp	SCRATCH0,SCRATCH2
	cpc	SCRATCH1,SCRATCH3
	brcs	CPD_SFL1
	sub	SCRATCH0,SCRATCH2
	sbc	SCRATCH1,SCRATCH3
	mov	SCRATCH2,SCRATCH0
	mov	SCRATCH3,SCRATCH1
	rjmp	CPD_SFL2
CPD_SFL1:
	sub	SCRATCH2,SCRATCH0
	sbc	SCRATCH3,SCRATCH1
CPD_SFL2:
; Result is between 0 and 59,999
; If result >= 30000, assume it is a rollover and take 60,000 - result
	ldi	SCRATCH1,high(30000)
	ldi	SCRATCH0,low(30000) ; no cpci
	cp	SCRATCH2,SCRATCH0
	cpc	SCRATCH3,SCRATCH1
	brlo	CPD_NOT_ROLLOVER
	mov	SCRATCH0,SCRATCH2
	mov	SCRATCH1,SCRATCH3
	ldi	SCRATCH3,high(60000)
	ldi	SCRATCH2,low(60000)
	sub	SCRATCH2,SCRATCH0
	sbc	SCRATCH3,SCRATCH1
CPD_NOT_ROLLOVER:
; Get minimum displacement in SCRATCH1:SCRATCH0
	ldi	XH,high(EEADR_TRACK_MIN_DISPLACEMENT)
	ldi	XL,low(EEADR_TRACK_MIN_DISPLACEMENT)
	rcall	EEPROM_READWORD_X_TO_SCRATCH10
; Compare with calculated displacement
	cp	SCRATCH2,SCRATCH0
	cpc	SCRATCH3,SCRATCH1
; Here carry is SET if we have NOT moved
	ret

#ifdef	ALT_IN_FEET
#define	NEED_UNITS_CONVERT 1
#endif
#ifdef	SPEED_IN_MPH
#define	NEED_UNITS_CONVERT 1
#endif
#ifdef	SPEED_IN_KPH
#define	NEED_UNITS_CONVERT 1
#endif
#ifdef	NEED_UNITS_CONVERT
; Units conversion
; Call with number in SCRATCH1:SCRATCH0 (from PARSE_DECIMAL)
; Conversion factor in XH:XL
; Result returned in ZH:ZL
; Does a 16x16 bit multiply, retaining only the high order word of the product
; This took quite a while to figure out! The trick is to move carry into MSB.
UNITS_CONVERT_X4:
	lsl	SCRATCH0
	rol	SCRATCH1
UNITS_CONVERT_X2:
	lsl	SCRATCH0
	rol	SCRATCH1
UNITS_CONVERT:
	ldi	SCRATCH2,16
	clr	ZH
	clr	ZL
UC_LOOP:
	lsr	XH
	ror	XL
	brcc	UC_ZERO
	add	ZL,SCRATCH0
	adc	ZH,SCRATCH1
UC_ZERO:
	ror	ZH ; carry from previous addition becomes MSB
	ror	ZL
	dec	SCRATCH2
	brne	UC_LOOP
	ret
#endif

; Locate and send, called from CMD_LOCATE via scheduled command
; TRACK_STATE tests are to prevent a double locate when requested in tracking mode
GPS_LOCATE_AND_SEND:
;	lds	SCRATCH0,TRACK_STATE
;	cpi	SCRATCH0,TRACK_STATE_POWERSAVE
;	brsh	GLS_LOCATE_REQUIRED
;	cpi	SCRATCH0,TRACK_STATE_OFF
;	breq	GLS_LOCATE_REQUIRED
;	rjmp	GLS_ALREADY_LOCATED
;GLS_LOCATE_REQUIRED:
	rcall	GPS_LOCATE
;GLS_ALREADY_LOCATED:
	ldi	SCRATCH0,FIX_TYPE_LOCATE - FIX_TYPE_BASE
	sts	SEND_STRING_OFFSET_1,SCRATCH0
; GPS_SEND_LOCATION called by tracking function, assumes we have a locate in memory
GPS_SEND_LOCATION:
	lds	SCRATCH1,GPS_STATUS
	ldi	SCRATCH0,PHONE_STRING_GPS_DATASTREAM_TIMEOUT - PHONE_STRING_BASE
	cpi	SCRATCH1,GPS_STATUS_DATASTREAM_TIMEOUT
	breq	MULTI_SEND_DO
	ldi	SCRATCH0,PHONE_STRING_GPS_FIX_TIMEOUT - PHONE_STRING_BASE
	cpi	SCRATCH1,GPS_STATUS_FIX_TIMEOUT
	breq	MULTI_SEND_DO
	ldi	SCRATCH0,PHONE_STRING_GPS_LOCATION - PHONE_STRING_BASE
	rjmp	MULTI_SEND_DO
	
; Send a variety of messages depending on entry point
; Called from COMMAND_PENDING
MULTI_SEND_COMMAND_EXECUTED:
	ldi	SCRATCH0,PHONE_STRING_COMMAND_EXECUTED - PHONE_STRING_BASE
	rjmp	MULTI_SEND_DO
MULTI_SEND_PASSWORD_CHANGED:
	ldi	SCRATCH0,PHONE_STRING_PASSWORD_CHANGED - PHONE_STRING_BASE
	rjmp	MULTI_SEND_DO
MULTI_SEND_PASSWORD_CHANGE_FAILED:
	ldi	SCRATCH0,PHONE_STRING_PASSWORD_CHANGE_FAILED - PHONE_STRING_BASE
	rjmp	MULTI_SEND_DO
MULTI_SEND_UNKNOWN_COMMAND:
	ldi	SCRATCH0,PHONE_STRING_UNKNOWN_COMMAND - PHONE_STRING_BASE
	rjmp	MULTI_SEND_DO
MULTI_SEND_STATUS:
	rcall	CLEAR_PARSE_FIELDS
	ldi	YH,high(PARSE_FIELDS)
	ldi	YL,low(PARSE_FIELDS)
	ldi	ZH,high(SETSPEED_DECODE_STRING<<1)
	ldi	ZL,low(SETSPEED_DECODE_STRING<<1)
	clr	CHAR ; to determine which string we are on
SSM_COPY_LOOP:
	lpm	SCRATCH2,Z+
	tst	SCRATCH2 ; done with string?
	breq	SSM_DONE_WITH_STRING
	mov	XH,SCRATCH2
	andi	XH,15
	lpm	XL,Z+ ; now have address of value in X
	rcall	EEPROM_READBYTE_X_TO_SCRATCH0 ; X incremented here
;
	clr	SCRATCH1
	sbrs	SCRATCH2,DECODE_SETTING_ONE_BYTE_X4
	rjmp	SSM_NOT_X4
	lsl	SCRATCH0
	rol	SCRATCH1
	lsl	SCRATCH0
	rol	SCRATCH1 ; X4
SSM_NOT_X4:
	sbrs	SCRATCH2,DECODE_SETTING_TWO_BYTE
	st	Y+,SCRATCH1 ; write high byte if one byte value
	st	Y+,SCRATCH0
;
	sbrs	SCRATCH2,DECODE_SETTING_TWO_BYTE
	rjmp	SSM_COPY_LOOP
	rcall	EEPROM_READBYTE_X_TO_SCRATCH0 ; X incremented here
	st	Y+,SCRATCH0 ; write second byte of two-byte value
	rjmp	SSM_COPY_LOOP
SSM_DONE_WITH_STRING:
	inc	CHAR
	cpi	CHAR,2 ; which string are we on?
	breq	SSM_NOW_DO_POWER
	brsh	SSM_DONE_WITH_ALL_STRINGS
; now do track
	ldi	ZH,high(SETTRACK_DECODE_STRING<<1)
	ldi	ZL,low(SETTRACK_DECODE_STRING<<1)
	rjmp	SSM_COPY_LOOP
SSM_NOW_DO_POWER:
	ldi	ZH,high(SETPOWER_DECODE_STRING<<1)
	ldi	ZL,low(SETPOWER_DECODE_STRING<<1)
	rjmp	SSM_COPY_LOOP
SSM_DONE_WITH_ALL_STRINGS:	
	st	Y+,REG_ZERO ; high byte of WDR count
	lds	SCRATCH0,WDR_COUNT
	st	Y+,SCRATCH0 ; low byte of WDR count
;
	ldi	ZH,high(PHONE_GET_BATTERY_AND_SIGNAL_STRING<<1)
	ldi	ZL,low(PHONE_GET_BATTERY_AND_SIGNAL_STRING<<1)
	rcall	DO_PHONE_OPERATION
	ldi	SCRATCH0,PHONE_STRING_STATUS_MESSAGE - PHONE_STRING_BASE
;	rjmp	MULTI_SEND_DO
MULTI_SEND_DO:
#ifdef DEBUG_MESSAGE_COUNT_ENABLE
	lds	SCRATCH1,DEBUG_MESSAGE_COUNT+1 
	inc	SCRATCH1
	sts	DEBUG_MESSAGE_COUNT+1,SCRATCH1
	brne	MSD_NO_INCHIGH
	lds	SCRATCH1,DEBUG_MESSAGE_COUNT
	inc	SCRATCH1
	sts	DEBUG_MESSAGE_COUNT,SCRATCH1
MSD_NO_INCHIGH:
#endif
;
	sts	SEND_STRING_OFFSET_0,SCRATCH0
;
	ldi	XH,high(EEADR_PHONE_NUMBER)
	ldi	XL,low(EEADR_PHONE_NUMBER)
	rcall	EEPROM_READBYTE_X_TO_SCRATCH0
	tst	SCRATCH0
	brne	MSD_PHONENUMBER
; no phone number defined
	ldi	SCRATCH0,0x14 ; BLINK CODE no phone number defined
	rcall	SEND_BLINK_CODE
	rjmp	MSD_END
MSD_PHONENUMBER:
; store
	ldi	SCRATCH0,3 ; how many times to retry
MSD_STORE_RETRY:
	sts	CHECKSUM_RETRY_COUNT,SCRATCH0
	ldi	ZH,high(PHONE_SEND_STORE_MESSAGE_STRING<<1)
	ldi	ZL,low(PHONE_SEND_STORE_MESSAGE_STRING<<1)
	rcall	DO_PHONE_OPERATION ; send checksum is in VERIFY_CHECKSUM
	brts	MSD_FAILED_128
; verify checksum and length
	ldi	ZH,high(PHONE_READ_MESSAGE_STRING<<1)
	ldi	ZL,low(PHONE_READ_MESSAGE_STRING<<1) ; send checksum moved to CHECKSUM
	rcall	DO_PHONE_OPERATION ; read checksum in VERIFY_CHECKSUM
	brts	MSD_FAILED_64
;
	cp	CHECKSUM,VERIFY_CHECKSUM
	brne	MSD_STORE_CHECKSUM_BAD
	lds	SCRATCH0,SEND_CHECKSUM_BYTES
	lds	SCRATCH1,READ_CHECKSUM_BYTES
	cp	SCRATCH0,SCRATCH1
	breq	MSD_STORE_DONE
;
MSD_STORE_CHECKSUM_BAD:
	ldi	ZH,high(PHONE_DELETE_MESSAGE_STRING<<1)
	ldi	ZL,low(PHONE_DELETE_MESSAGE_STRING<<1)
	rcall	DO_PHONE_OPERATION ; delete corrupt message
;
	lds	SCRATCH0,CHECKSUM_RETRY_COUNT
	dec	SCRATCH0
	breq	MSD_FAILED_32 ; checksum bad too may times
	rjmp	MSD_STORE_RETRY
MSD_STORE_DONE:
; send
	cbr	FLAG_REGISTER,1<<FLAG_MESSAGE_SENT
	ldi	ZH,high(PHONE_SEND_TRANSMIT_MESSAGE_STRING<<1)
	ldi	ZL,low(PHONE_SEND_TRANSMIT_MESSAGE_STRING<<1)
	rcall	DO_PHONE_OPERATION
	brts	MSD_FAILED_16
; clear and send pending
	sbrc	FLAG_REGISTER,FLAG_MESSAGE_SENT ; skip if out of coverage
	rcall	MSD_CLEAR_SENT_AND_SEND_PENDING
	ret
;
; BEGIN DEBUG
MSD_FAILED_128:
#ifdef	DEBUG_SEND_MESSAGES
	lds	SCRATCH0,WDR_COUNT
	ori	SCRATCH0,128
	sts	WDR_COUNT,SCRATCH0
	rjmp	MSD_FAILED
#endif
MSD_FAILED_64:
#ifdef	DEBUG_SEND_MESSAGES
	lds	SCRATCH0,WDR_COUNT
	ori	SCRATCH0,64
	sts	WDR_COUNT,SCRATCH0
	rjmp	MSD_FAILED
#endif
MSD_FAILED_32:
#ifdef	DEBUG_SEND_MESSAGES
	lds	SCRATCH0,WDR_COUNT
	ori	SCRATCH0,32
	sts	WDR_COUNT,SCRATCH0
	rjmp	MSD_FAILED
#endif
MSD_FAILED_16:
#ifdef	DEBUG_SEND_MESSAGES
	lds	SCRATCH0,WDR_COUNT
	ori	SCRATCH0,16
	sts	WDR_COUNT,SCRATCH0
	rjmp	MSD_FAILED
#endif
; END DEBUG
MSD_FAILED:
	ldi	SCRATCH0,0x12 ; BLINK CODE send message failed
	rcall	SEND_BLINK_CODE
MSD_END:
	ret

; Attempts to send unsent messages, and deletes sent ones
; First send, then delete
; If PHONE_FULL flag is set from polling, will delete the first unsent message
; and clear the PHONE_FULL flag. Parse fields have up to seven UNSENT numbers,
; a null, up to seven SENT numbers, and a null.
MSD_CLEAR_SENT_AND_SEND_PENDING:
; phone string clears parse fields, and they get re-cleared after an error
	ldi	ZH,high(PHONE_LIST_STO_STRING<<1)
	ldi	ZL,low(PHONE_LIST_STO_STRING<<1)
	rcall	DO_PHONE_OPERATION
;
	ldi	ZH,high(FLDADR_CMGL_UNSENT)
	ldi	ZL,low(FLDADR_CMGL_UNSENT)
	sbr	FLAG_REGISTER,1<<FLAG_MESSAGE_SENT ; send until one fails
MSD_CSSP_LOOP:
	ldi	XH,high(CURRENT_SMS)
	ldi	XL,low(CURRENT_SMS)
	ldi	SCRATCH0,4
MSD_CSSP_COPY:
	ld	SCRATCH1,Z+
	st	X+,SCRATCH1
	dec	SCRATCH0
	brne	MSD_CSSP_COPY
;
	lds	SCRATCH0,CURRENT_SMS
	tst	SCRATCH0
	breq	MSD_CSPP_RECORD_NULL
;
	push	ZH
	push	ZL
;
	cpi	ZL,low(FLDADR_CMGL_SENT) ; sending section? ( assume fields on same page )
	sbrs	FLAG_REGISTER,FLAG_PHONE_FULL ; Delete unsent message if phone is full
	brlo	MSD_CSPP_SENDING
;
	ldi	ZH,high(PHONE_DELETE_MESSAGE_STRING<<1)
	ldi	ZL,low(PHONE_DELETE_MESSAGE_STRING<<1)
	cbr	FLAG_REGISTER,1<<FLAG_PHONE_FULL ; only delete one of them
	rjmp	MSD_CSPP_DO_PHONE_OP
MSD_CSPP_SENDING:
	sbrs	FLAG_REGISTER,FLAG_MESSAGE_SENT ; skip if last one failed
	rjmp	MSD_CSPP_SKIP_PHONE_OP
	cbr	FLAG_REGISTER,1<<FLAG_MESSAGE_SENT
	ldi	ZH,high(PHONE_SEND_TRANSMIT_MESSAGE_STRING<<1)
	ldi	ZL,low(PHONE_SEND_TRANSMIT_MESSAGE_STRING<<1)
MSD_CSPP_DO_PHONE_OP:
	rcall	DO_PHONE_OPERATION
MSD_CSPP_SKIP_PHONE_OP:
	pop	ZL
	pop	ZH
MSD_CSPP_RECORD_NULL:
;
	cpi	ZL,low(FLDADR_CMGL_STOP)
	brlo	MSD_CSSP_LOOP
	ret
	

EEPROM_READWORD_X_TO_SCRATCH10:
	out	EEARH,XH ; EEPROM high register
	out	EEARL,XL ; EEPROM low register
	sbi	EECR,EERE ; EEPROM read signal
	in	SCRATCH1,EEDR ; EEPROM data register
	adiw	XH:XL,1
EEPROM_READBYTE_X_TO_SCRATCH0:
	out	EEARH,XH ; EEPROM high register
	out	EEARL,XL ; EEPROM low register
	sbi	EECR,EERE ; EEPROM read signal
	in	SCRATCH0,EEDR ; EEPROM data register
	adiw	XH:XL,1
	ret


; Write up to SCRATCH0 bytes stopping at CR or space, null terminate if not full
; Leaves NEXT_CHAR backed up one so we can see the space or CR
EEPROM_WRITE_FIELD:
	rcall	GET_NEXT_CHAR
	brts	EWF_DONE_WRITING ; should not happen
	cpi	CHAR,33
	brlo	EWF_DONE_WRITING ; space or CR - done with field
	rcall	EEPROM_WRITEBYTE
	dec	SCRATCH0 ; byte count
	brne	EEPROM_WRITE_FIELD
EWF_DONE_WRITING:
	dec	YL ; back up a character
	clr	CHAR ; to null terminate
	tst	SCRATCH0 ; field full?
	brne	EEPROM_WRITEBYTE ; write null and return
	ret
	
; Write CHAR to address X in EEPROM, post-inc X
EEPROM_WRITEBYTE:	
	out	EEARH,XH ; EEPROM address high
	out	EEARL,XL ; EEPROM address low
	out	EEDR,CHAR ; received character
	in	CHAR,SREG
	cli	; next instructions have to be atomic
	sbi	EECR,EEMPE
	sbi	EECR,EEPE ; writing byte now
	out	SREG,CHAR ; end atomic section
EWB_WAIT:
	sbic	EECR,EEPE ; wait for write to finish
	rjmp	EWB_WAIT
	adiw	XH:XL,1 ; increment EEPROM pointer
	ret

; Decodes and saves the SETPOWER, SETTRACK, and SETSPEED commands
; Command string in Y pointer
; Pattern in Z pointer
; Control string is HIGH, LOW EEPROM address with setting types 
; in high bits of HIGH byte, 0 byte terminated.
SAVE_SETTINGS:
	push	ZH
	push	ZL
	rcall	CLEAR_PARSE_FIELDS
	ldi	ZH,high(SETTINGS_PARSE_STRING<<1)
	ldi	ZL,low(SETTINGS_PARSE_STRING<<1)
	cbr	FLAG_REGISTER,1<<FLAG_PARSE_NMEA
	ldi	CHAR,' '
	mov	DELIM,CHAR
	rcall	PARSE_NMEA_OR_COMMAND_LINE
	pop	ZL
	pop	ZH ; get pattern address back
	ldi	XH,high(PARSE_FIELDS) ; address of first
	ldi	XL,low(PARSE_FIELDS) ; numeric string (every 8 bytes)
;
SSET_LOOP:
	push	XH
	push	XL ; keep address of decimal string
	ld	SCRATCH0,X
	tst	SCRATCH0 ; make sure there is a string there
	breq	SSET_OUT ; if not, we're done
;
	ldi	SCRATCH0,8 ; field length
	mov	PAT,SCRATCH0
	ldi	SCRATCH0,0xff
	mov	DELIM,SCRATCH0 ; do not parse decimal point
	rcall	PARSE_DECIMAL ; SCRATCH1:SCRATCH0 now has numeric value
	lpm	SCRATCH2,Z+ ; EEPROM address high + data type flags
	tst	SCRATCH2 ; end of pattern?
	breq	SSET_OUT
;
	mov	XH,SCRATCH2
	andi	XH,15 ; EEPROM address
	lpm	XL,Z+ ; EEPROM address low
;
	sbrs	SCRATCH2,DECODE_SETTING_ONE_BYTE_X4
	rjmp	SSET_NO_DIV_X4
	lsr	SCRATCH1
	ror	SCRATCH0
	lsr	SCRATCH1 ; Divide by 4 if X4 setting
	ror	SCRATCH0
SSET_NO_DIV_X4:
;
	mov	CHAR,SCRATCH1 ; high byte
	sbrc	SCRATCH2,DECODE_SETTING_TWO_BYTE
	rcall	EEPROM_WRITEBYTE ; write high byte if this is a two-byte setting
	mov	CHAR,SCRATCH0 ; low byte
	rcall	EEPROM_WRITEBYTE ; write low byte
;
	pop	XL
	pop	XH ; decimal string pointer
	adiw	XH:XL,8 ; next field
	rjmp	SSET_LOOP
;
SSET_OUT:
	pop	XL
	pop	XH ; discard from stack
	ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; End of assembly language code - everything below this is data and 
; interpreted control strings. 

#ifdef	SERIAL_DEBUG
DEBUG_STARTUP_MESSAGE:
	.db	13,"MACHINE STARTED IN DEBUG MODE",13,10,0
TIMEOUT_MESSAGE:
	.db	"OPERATION TIMED OUT",13,10,0
FAILED_MESSAGE:
	.db	"OPERATION FAILED",13,10,0
SUCCEEDED_MESSAGE:
	.db	"OPERATION SUCCEEDED",13,10,0
POLL_INTERVAL_MESSAGE:
	.db	"POLL INTERVAL IS ",0xf8,high(POLL_INTERVAL),low(POLL_INTERVAL),13,10,0
SPACE_USED_MESSAGE:
	.db	"PHONE SPACE USED IS ",0xf8,high(FLDADR_CPMS_USED_NUMERIC), \
		low(FLDADR_CPMS_USED_NUMERIC)," OF ",0xf8, \
		high(FLDADR_CPMS_TOTAL_NUMERIC),low(FLDADR_CPMS_TOTAL_NUMERIC), \
		13,10,0
CRLF:
	.db	13,10,0
#endif

; Commands null terminated, double null at end of last one
PHONE_CHECK_STATUS_STRING:
	.db	0xf0,INIT_RECEIVE_TIMEOUT,26,"ATE0",13,0,"ATE0",13,0, \
		0xf0,DEFAULT_RECEIVE_TIMEOUT,"AT+CFUN?",13,0,0

#ifdef	DEBUG_KEEP_INVALID_MESSAGES
PHONE_INIT_POLL_STRING:
	.db	0xf0,INIT_RECEIVE_TIMEOUT,"ATE0",13,0, \
		0xf0,DEFAULT_RECEIVE_TIMEOUT,"AT+CMGF=1",13,0, \
		"AT+CNMI=2,1,0,0,0",13,0, \
		0xf1,"AT+CPMS=",34,"ME",34,",",34,"ME",34,",",34,"ME",34,13,0, \
		"AT+CMGL=",34,"REC UNREAD",34,13,0,0 
#else
PHONE_INIT_POLL_STRING:
	.db	0xf0,INIT_RECEIVE_TIMEOUT,"ATE0",13,0, \
		0xf0,DEFAULT_RECEIVE_TIMEOUT,"AT+CMGF=1",13,0, \
		"AT+CNMI=2,1,0,0,0",13,0, \
		0xf1,"AT+CPMS=",34,"ME",34,",",34,"ME",34,",",34,"ME",34,13,0, \
		"AT+CMGL=",34,"REC READ",34,13,0, \
		"AT+CMGL=",34,"REC UNREAD",34,13,0,0 
#endif

PHONE_GET_BATTERY_AND_SIGNAL_STRING:
	.db	0xf0,INIT_RECEIVE_TIMEOUT,"ATE0",13,0, \
		0xf0,DEFAULT_RECEIVE_TIMEOUT,"AT+CBC",13,0,"AT+CSQ",13,0,0

PHONE_READ_MESSAGE_STRING:
	.db	0xf0,INIT_RECEIVE_TIMEOUT,"ATE0",13,0,0xf0,DEFAULT_RECEIVE_TIMEOUT, \
                "AT+CMGR=",0xfd,(4<<2)+high(CURRENT_SMS),low(CURRENT_SMS),13,0,0

PHONE_DELETE_MESSAGE_STRING:
	.db	0xf0,INIT_RECEIVE_TIMEOUT,"ATE0",13,0,0xf0,DEFAULT_RECEIVE_TIMEOUT, \
		"AT+CMGD=",0xfd,(4<<2)+high(CURRENT_SMS),low(CURRENT_SMS),13,0,0

PHONE_LIST_STO_STRING:
	.db	0xf0,INIT_RECEIVE_TIMEOUT,"ATE0",13,0, \
		0xf1,"AT+CMGL=",34,"STO UNSENT",34,13,0, \
		"AT+CMGL=",34,"STO SENT",34,13,0,0 

PHONE_POWEROFF_STRING:
	.db	0xf0,INIT_RECEIVE_TIMEOUT,"ATE0",13,0,0xf0,SMS_SEND_RECEIVE_TIMEOUT, \
		"AT+CFUN=0",13,0,0

#ifdef	RESET_PHONE_PERIODICALLY
; Notice double zero in the middle of this
PHONE_RESET_STRING:
	.db	0xf0,INIT_RECEIVE_TIMEOUT,"ATE0",13,0,0,"AT*SWRESET",13,0
#else
PHONE_POWERON_STRING:
	.db	0xf0,INIT_RECEIVE_TIMEOUT,"ATE0",13,0,0xf0,SMS_SEND_RECEIVE_TIMEOUT, \
		"AT+CFUN=1",13,0,0
#endif
PHONE_MATCH_PATTERN:
	.db	low(PHONE_INIT_ERROR),high(PHONE_INIT_ERROR),"ERROR",13,0, \
		low(PHONE_INIT_OK),high(PHONE_INIT_OK),"OK",13,0, \
		low(PHONE_INIT_ISOFF),high(PHONE_INIT_ISOFF),"+CFUN: 0",0, \
		low(PHONE_INIT_IGNORE),high(PHONE_INIT_IGNORE),"+CFUN: 1",0, \
		low(PHONE_MSG_SENT),high(PHONE_MSG_SENT),"+CMSS:",0, \
		low(PHONE_INIT_IGNORE),high(PHONE_INIT_IGNORE),"+CMS ERROR",0, \
		low(PHONE_INIT_CPMS),high(PHONE_INIT_CPMS),"+CPMS:",0, \
		low(PHONE_SEND_CMGW),high(PHONE_SEND_CMGW),"+CMGW:",0, \
		low(PHONE_INIT_CMGL),high(PHONE_INIT_CMGL),"+CMGL:",0, \
		low(PHONE_READ_CMGR),high(PHONE_READ_CMGR),"+CMGR:",0, \
		low(PHONE_GOT_CMTI),high(PHONE_GOT_CMTI),"+CMTI:",0, \
		low(PHONE_SEND_PROMPT),high(PHONE_SEND_PROMPT),"> ",0, \
		low(PHONE_GOT_BATTERY_CHARGE),high(PHONE_GOT_BATTERY_CHARGE),"+CBC: 0,",0, \
		low(PHONE_GOT_BATTERY_CHARGE),high(PHONE_GOT_BATTERY_CHARGE),"+CBC: 1,",0, \
		low(PHONE_GOT_SIGNAL_STRENGTH),high(PHONE_GOT_SIGNAL_STRENGTH),"+CSQ: ",0, \
		low(PHONE_INIT_ERROR),high(PHONE_INIT_ERROR),0

; These functions should not mess up the Y pointer.
COMMAND_MATCH_PATTERN:
	.db	low(CMD_SETADDRESS),high(CMD_SETADDRESS),"SETADDRESS",0, \
		low(CMD_SETNAME),high(CMD_SETNAME),"SETNAME",0, \
		low(CMD_SETPASSWORD),high(CMD_SETPASSWORD),"SETPASSWORD",0, \
		low(CMD_SETTRACK),high(CMD_SETTRACK),"SETTRACK",0, \
		low(CMD_SETPOWER),high(CMD_SETPOWER),"SETPOWER",0, \
		low(CMD_SETSPEED),high(CMD_SETSPEED),"SETSPEED",0, \
		low(CMD_STATUS),high(CMD_STATUS),"STATUS",0, \
		low(CMD_LOCATE),high(CMD_LOCATE),"LOCATE",0, \
		low(CMD_SALOCATE),high(CMD_SALOCATE),"SALOCATE",0, \
		low(CMD_TRACKOFF),high(CMD_TRACKOFF),"TRACKOFF",0, \
		low(CMD_TRACKON),high(CMD_TRACKON),"TRACKON",0, \
		low(CMD_POWERSAVE),high(CMD_POWERSAVE),"POWERSAVE",0, \
		low(CMD_POWERON),high(CMD_POWERON),"POWERON",0, \
		low(CMD_REBOOT),high(CMD_REBOOT),"REBOOT",0, \
		low(CMD_REINIT),high(CMD_REINIT),"REINIT",0, \
		low(CMD_CAUSEWDR),high(CMD_CAUSEWDR),"CAUSEWDR",0, \
		low(CMD_UNRECOGNIZED),high(CMD_UNRECOGNIZED),0

CMGL_MATCH_PATTERN:
	.db	low(PPMN_CSMS_MATCH),high(PPMN_CSMS_MATCH),",",0, \
		low(PPMN_CSMS_MATCH),high(PPMN_CSMS_MATCH),34,0, \
		low(PPMN_CSMS_SENT),high(PPMN_CSMS_SENT),"STO SENT",0, \
		low(PPMN_CSMS_UNSENT),high(PPMN_CSMS_UNSENT),"STO UNSENT",0, \
		low(PPMN_CSMS_RECEIVED),high(PPMN_CSMS_RECEIVED),"REC ",0, \
		low(PPMN_OUT),high(PPMN_OUT),0

; <field number><max length and high address bits><low address byte>
SETNAME_PARSE_STRING:
	.db	1,(FLDLEN_DEVICENAME<<2)+high(FLDADR_DEVICENAME),low(FLDADR_DEVICENAME), \
		0xff

SETPASSWORD_PARSE_STRING:
	.db	1,(FLDLEN_PASSWORD1<<2)+high(FLDADR_PASSWORD1),low(FLDADR_PASSWORD1), \
		2,(FLDLEN_PASSWORD2<<2)+high(FLDADR_PASSWORD2),low(FLDADR_PASSWORD2), \
		0xff

#ifdef DEBUG_MESSAGE_COUNT_ENABLE
PHONE_SEND_STORE_MESSAGE_STRING:
	.db	0xf0,INIT_RECEIVE_TIMEOUT,"ATE0",13,0, \
		0xf0,DEFAULT_RECEIVE_TIMEOUT, \
		"AT+CMGW=",34,0xfe, \
		high(EEADR_PHONE_NUMBER)+(EELEN_PHONE_NUMBER<<2), \
		low(EEADR_PHONE_NUMBER),34,13,0, \
		0xf7,0xff,high(EEADR_EMAIL_ADDR)+(EELEN_EMAIL_ADDR<<2), \
		low(EEADR_EMAIL_ADDR), \
		0xff,high(EEADR_DEVICENAME)+(EELEN_DEVICENAME<<2), \
		low(EEADR_DEVICENAME), \
		"MC=",0xf8,high(DEBUG_MESSAGE_COUNT),low(DEBUG_MESSAGE_COUNT)," ", \
		0xfa,high(PHONE_STRING_BASE<<1),low(PHONE_STRING_BASE<<1),0xf7,26,13,0,0
#else
PHONE_SEND_STORE_MESSAGE_STRING:
	.db	0xf0,INIT_RECEIVE_TIMEOUT,"ATE0",13,0, \
		0xf0,DEFAULT_RECEIVE_TIMEOUT, \
		"AT+CMGW=",34,0xfe, \
		high(EEADR_PHONE_NUMBER)+(EELEN_PHONE_NUMBER<<2), \
		low(EEADR_PHONE_NUMBER),34,13,0, \
		0xf7,0xff,high(EEADR_EMAIL_ADDR)+(EELEN_EMAIL_ADDR<<2), \
		low(EEADR_EMAIL_ADDR), \
		0xff,high(EEADR_DEVICENAME)+(EELEN_DEVICENAME<<2), \
		low(EEADR_DEVICENAME), \
		0xfa,high(PHONE_STRING_BASE<<1),low(PHONE_STRING_BASE<<1),0xf7,26,13,0,0
#endif

PHONE_SEND_TRANSMIT_MESSAGE_STRING:
	.db	0xf0,INIT_RECEIVE_TIMEOUT,"ATE0",13,0,0xf0,SMS_SEND_RECEIVE_TIMEOUT, \
		"AT+CMSS=",0xfd,(4<<2)+high(CURRENT_SMS),low(CURRENT_SMS),13,0,0

; Phone string offsets from one base can be up to 512 bytes due to word addressing
PHONE_STRING_BASE:
PHONE_STRING_COMMAND_EXECUTED:
	.db	"COMMAND EXECUTED",0

PHONE_STRING_PASSWORD_CHANGED:
	.db	"PASSWORD CHANGED",0

PHONE_STRING_PASSWORD_CHANGE_FAILED:
	.db	"PASSWORD CHANGE FAILED",0

PHONE_STRING_UNKNOWN_COMMAND:
	.db	"UNKNOWN COMMAND",0

PHONE_STRING_GPS_DATASTREAM_TIMEOUT:
	.db	"NO DATASTREAM FROM GPS DEVICE", \
		0xfc,high(LAST_GOOD_FIX_STRING<<1),low(LAST_GOOD_FIX_STRING<<1),0

PHONE_STRING_GPS_FIX_TIMEOUT:
	.db	"GPS TIMED OUT WAITING FOR FIX", \
		0xfc,high(LAST_GOOD_FIX_STRING<<1),low(LAST_GOOD_FIX_STRING<<1),0

; There are three different speed units, and two different altitude units options.
; The assembler will add a null byte after a string if there is an odd number of bytes.
; As far as I know there is no way to incorporare an externally defined string into
; a .db structure. That means I have to define all six cases here, instead of having
; ifdefs inside the string. :-(
#ifdef SPEED_IN_MPH
#ifdef ALT_IN_FEET
PHONE_STRING_GPS_LOCATION:
	.db	0xfb,high(FIX_TYPE_BASE<<1),low(FIX_TYPE_BASE<<1), \
		" POS ",0xfd,(2<<2)+high(FLDADR_GPRMC_LATITUDE),low(FLDADR_GPRMC_LATITUDE)," ", \
		0xfd,((FLDLEN_GPRMC_LATITUDE-2)<<2)+high(FLDADR_GPRMC_LATITUDE+2),low(FLDADR_GPRMC_LATITUDE+2), \
		" ",0xfd,(FLDLEN_GPRMC_LATDIR<<2)+high(FLDADR_GPRMC_LATDIR),low(FLDADR_GPRMC_LATDIR), \
		" ",0xfd,(3<<2)+high(FLDADR_GPRMC_LONGITUDE),low(FLDADR_GPRMC_LONGITUDE), \
		" ",0xfd,((FLDLEN_GPRMC_LONGITUDE-3)<<2)+high(FLDADR_GPRMC_LONGITUDE+3),low(FLDADR_GPRMC_LONGITUDE+3), \
		" ",0xfd,(FLDLEN_GPRMC_LONGDIR<<2)+high(FLDADR_GPRMC_LONGDIR),low(FLDADR_GPRMC_LONGDIR), \
		" ALT ",0xf8,high(FLDADR_GPGGA_ALT_NUMERIC),low(FLDADR_GPGGA_ALT_NUMERIC)," FT", \
		" SPEED ",0xf9,high(FLDADR_GPRMC_SPEED_NUMERIC),low(FLDADR_GPRMC_SPEED_NUMERIC)," MPH", \
               	" COURSE ",0xfd,(FLDLEN_GPRMC_COURSE<<2)+high(FLDADR_GPRMC_COURSE),low(FLDADR_GPRMC_COURSE), \
		" AT ",0xfd,(2<<2)+high(FLDADR_GPRMC_DATE+4),low(FLDADR_GPRMC_DATE+4),"/", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_DATE+2),low(FLDADR_GPRMC_DATE+2),"/", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_DATE),low(FLDADR_GPRMC_DATE)," ", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME),low(FLDADR_GPRMC_TIME),":", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME+2),low(FLDADR_GPRMC_TIME+2),":", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME+4),low(FLDADR_GPRMC_TIME+4)," UTC", \
		" SATS ",0xfd,(FLDLEN_GPGGA_SATS<<2)+high(FLDADR_GPGGA_SATS),low(FLDADR_GPGGA_SATS),0
#endif
#endif

#ifdef SPEED_IN_KPH
#ifdef ALT_IN_FEET
PHONE_STRING_GPS_LOCATION:
	.db	0xfb,high(FIX_TYPE_BASE<<1),low(FIX_TYPE_BASE<<1), \
		" POS ",0xfd,(2<<2)+high(FLDADR_GPRMC_LATITUDE),low(FLDADR_GPRMC_LATITUDE)," ", \
		0xfd,((FLDLEN_GPRMC_LATITUDE-2)<<2)+high(FLDADR_GPRMC_LATITUDE+2),low(FLDADR_GPRMC_LATITUDE+2), \
		" ",0xfd,(FLDLEN_GPRMC_LATDIR<<2)+high(FLDADR_GPRMC_LATDIR),low(FLDADR_GPRMC_LATDIR), \
		" ",0xfd,(3<<2)+high(FLDADR_GPRMC_LONGITUDE),low(FLDADR_GPRMC_LONGITUDE), \
		" ",0xfd,((FLDLEN_GPRMC_LONGITUDE-3)<<2)+high(FLDADR_GPRMC_LONGITUDE+3),low(FLDADR_GPRMC_LONGITUDE+3), \
		" ",0xfd,(FLDLEN_GPRMC_LONGDIR<<2)+high(FLDADR_GPRMC_LONGDIR),low(FLDADR_GPRMC_LONGDIR), \
		" ALT ",0xf8,high(FLDADR_GPGGA_ALT_NUMERIC),low(FLDADR_GPGGA_ALT_NUMERIC)," FT", \
		" SPEED ",0xf9,high(FLDADR_GPRMC_SPEED_NUMERIC),low(FLDADR_GPRMC_SPEED_NUMERIC)," KPH", \
               	" COURSE ",0xfd,(FLDLEN_GPRMC_COURSE<<2)+high(FLDADR_GPRMC_COURSE),low(FLDADR_GPRMC_COURSE), \
		" AT ",0xfd,(2<<2)+high(FLDADR_GPRMC_DATE+4),low(FLDADR_GPRMC_DATE+4),"/", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_DATE+2),low(FLDADR_GPRMC_DATE+2),"/", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_DATE),low(FLDADR_GPRMC_DATE)," ", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME),low(FLDADR_GPRMC_TIME),":", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME+2),low(FLDADR_GPRMC_TIME+2),":", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME+4),low(FLDADR_GPRMC_TIME+4)," UTC", \
		" SATS ",0xfd,(FLDLEN_GPGGA_SATS<<2)+high(FLDADR_GPGGA_SATS),low(FLDADR_GPGGA_SATS),0
#endif
#endif

#ifdef SPEED_IN_KNOTS
#ifdef ALT_IN_FEET
PHONE_STRING_GPS_LOCATION:
	.db	0xfb,high(FIX_TYPE_BASE<<1),low(FIX_TYPE_BASE<<1), \
		" POS ",0xfd,(2<<2)+high(FLDADR_GPRMC_LATITUDE),low(FLDADR_GPRMC_LATITUDE)," ", \
		0xfd,((FLDLEN_GPRMC_LATITUDE-2)<<2)+high(FLDADR_GPRMC_LATITUDE+2),low(FLDADR_GPRMC_LATITUDE+2), \
		" ",0xfd,(FLDLEN_GPRMC_LATDIR<<2)+high(FLDADR_GPRMC_LATDIR),low(FLDADR_GPRMC_LATDIR), \
		" ",0xfd,(3<<2)+high(FLDADR_GPRMC_LONGITUDE),low(FLDADR_GPRMC_LONGITUDE), \
		" ",0xfd,((FLDLEN_GPRMC_LONGITUDE-3)<<2)+high(FLDADR_GPRMC_LONGITUDE+3),low(FLDADR_GPRMC_LONGITUDE+3), \
		" ",0xfd,(FLDLEN_GPRMC_LONGDIR<<2)+high(FLDADR_GPRMC_LONGDIR),low(FLDADR_GPRMC_LONGDIR), \
		" ALT ",0xf8,high(FLDADR_GPGGA_ALT_NUMERIC),low(FLDADR_GPGGA_ALT_NUMERIC)," FT", \
		" SPEED ",0xf9,high(FLDADR_GPRMC_SPEED_NUMERIC),low(FLDADR_GPRMC_SPEED_NUMERIC)," KNOTS", \
               	" COURSE ",0xfd,(FLDLEN_GPRMC_COURSE<<2)+high(FLDADR_GPRMC_COURSE),low(FLDADR_GPRMC_COURSE), \
		" AT ",0xfd,(2<<2)+high(FLDADR_GPRMC_DATE+4),low(FLDADR_GPRMC_DATE+4),"/", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_DATE+2),low(FLDADR_GPRMC_DATE+2),"/", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_DATE),low(FLDADR_GPRMC_DATE)," ", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME),low(FLDADR_GPRMC_TIME),":", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME+2),low(FLDADR_GPRMC_TIME+2),":", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME+4),low(FLDADR_GPRMC_TIME+4)," UTC", \
		" SATS ",0xfd,(FLDLEN_GPGGA_SATS<<2)+high(FLDADR_GPGGA_SATS),low(FLDADR_GPGGA_SATS),0
#endif
#endif

#ifdef SPEED_IN_MPH
#ifdef ALT_IN_METERS
PHONE_STRING_GPS_LOCATION:
	.db	0xfb,high(FIX_TYPE_BASE<<1),low(FIX_TYPE_BASE<<1), \
		" POS ",0xfd,(2<<2)+high(FLDADR_GPRMC_LATITUDE),low(FLDADR_GPRMC_LATITUDE)," ", \
		0xfd,((FLDLEN_GPRMC_LATITUDE-2)<<2)+high(FLDADR_GPRMC_LATITUDE+2),low(FLDADR_GPRMC_LATITUDE+2), \
		" ",0xfd,(FLDLEN_GPRMC_LATDIR<<2)+high(FLDADR_GPRMC_LATDIR),low(FLDADR_GPRMC_LATDIR), \
		" ",0xfd,(3<<2)+high(FLDADR_GPRMC_LONGITUDE),low(FLDADR_GPRMC_LONGITUDE), \
		" ",0xfd,((FLDLEN_GPRMC_LONGITUDE-3)<<2)+high(FLDADR_GPRMC_LONGITUDE+3),low(FLDADR_GPRMC_LONGITUDE+3), \
		" ",0xfd,(FLDLEN_GPRMC_LONGDIR<<2)+high(FLDADR_GPRMC_LONGDIR),low(FLDADR_GPRMC_LONGDIR), \
		" ALT ",0xf8,high(FLDADR_GPGGA_ALT_NUMERIC),low(FLDADR_GPGGA_ALT_NUMERIC)," M", \
		" SPEED ",0xf9,high(FLDADR_GPRMC_SPEED_NUMERIC),low(FLDADR_GPRMC_SPEED_NUMERIC)," MPH", \
               	" COURSE ",0xfd,(FLDLEN_GPRMC_COURSE<<2)+high(FLDADR_GPRMC_COURSE),low(FLDADR_GPRMC_COURSE), \
		" AT ",0xfd,(2<<2)+high(FLDADR_GPRMC_DATE+4),low(FLDADR_GPRMC_DATE+4),"/", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_DATE+2),low(FLDADR_GPRMC_DATE+2),"/", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_DATE),low(FLDADR_GPRMC_DATE)," ", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME),low(FLDADR_GPRMC_TIME),":", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME+2),low(FLDADR_GPRMC_TIME+2),":", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME+4),low(FLDADR_GPRMC_TIME+4)," UTC", \
		" SATS ",0xfd,(FLDLEN_GPGGA_SATS<<2)+high(FLDADR_GPGGA_SATS),low(FLDADR_GPGGA_SATS),0
#endif
#endif

#ifdef SPEED_IN_KPH
#ifdef ALT_IN_METERS
PHONE_STRING_GPS_LOCATION:
	.db	0xfb,high(FIX_TYPE_BASE<<1),low(FIX_TYPE_BASE<<1), \
		" POS ",0xfd,(2<<2)+high(FLDADR_GPRMC_LATITUDE),low(FLDADR_GPRMC_LATITUDE)," ", \
		0xfd,((FLDLEN_GPRMC_LATITUDE-2)<<2)+high(FLDADR_GPRMC_LATITUDE+2),low(FLDADR_GPRMC_LATITUDE+2), \
		" ",0xfd,(FLDLEN_GPRMC_LATDIR<<2)+high(FLDADR_GPRMC_LATDIR),low(FLDADR_GPRMC_LATDIR), \
		" ",0xfd,(3<<2)+high(FLDADR_GPRMC_LONGITUDE),low(FLDADR_GPRMC_LONGITUDE), \
		" ",0xfd,((FLDLEN_GPRMC_LONGITUDE-3)<<2)+high(FLDADR_GPRMC_LONGITUDE+3),low(FLDADR_GPRMC_LONGITUDE+3), \
		" ",0xfd,(FLDLEN_GPRMC_LONGDIR<<2)+high(FLDADR_GPRMC_LONGDIR),low(FLDADR_GPRMC_LONGDIR), \
		" ALT ",0xf8,high(FLDADR_GPGGA_ALT_NUMERIC),low(FLDADR_GPGGA_ALT_NUMERIC)," M", \
		" SPEED ",0xf9,high(FLDADR_GPRMC_SPEED_NUMERIC),low(FLDADR_GPRMC_SPEED_NUMERIC)," KPH", \
               	" COURSE ",0xfd,(FLDLEN_GPRMC_COURSE<<2)+high(FLDADR_GPRMC_COURSE),low(FLDADR_GPRMC_COURSE), \
		" AT ",0xfd,(2<<2)+high(FLDADR_GPRMC_DATE+4),low(FLDADR_GPRMC_DATE+4),"/", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_DATE+2),low(FLDADR_GPRMC_DATE+2),"/", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_DATE),low(FLDADR_GPRMC_DATE)," ", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME),low(FLDADR_GPRMC_TIME),":", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME+2),low(FLDADR_GPRMC_TIME+2),":", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME+4),low(FLDADR_GPRMC_TIME+4)," UTC", \
		" SATS ",0xfd,(FLDLEN_GPGGA_SATS<<2)+high(FLDADR_GPGGA_SATS),low(FLDADR_GPGGA_SATS),0
#endif
#endif

#ifdef SPEED_IN_KNOTS
#ifdef ALT_IN_METERS
PHONE_STRING_GPS_LOCATION:
	.db	0xfb,high(FIX_TYPE_BASE<<1),low(FIX_TYPE_BASE<<1), \
		" POS ",0xfd,(2<<2)+high(FLDADR_GPRMC_LATITUDE),low(FLDADR_GPRMC_LATITUDE)," ", \
		0xfd,((FLDLEN_GPRMC_LATITUDE-2)<<2)+high(FLDADR_GPRMC_LATITUDE+2),low(FLDADR_GPRMC_LATITUDE+2), \
		" ",0xfd,(FLDLEN_GPRMC_LATDIR<<2)+high(FLDADR_GPRMC_LATDIR),low(FLDADR_GPRMC_LATDIR), \
		" ",0xfd,(3<<2)+high(FLDADR_GPRMC_LONGITUDE),low(FLDADR_GPRMC_LONGITUDE), \
		" ",0xfd,((FLDLEN_GPRMC_LONGITUDE-3)<<2)+high(FLDADR_GPRMC_LONGITUDE+3),low(FLDADR_GPRMC_LONGITUDE+3), \
		" ",0xfd,(FLDLEN_GPRMC_LONGDIR<<2)+high(FLDADR_GPRMC_LONGDIR),low(FLDADR_GPRMC_LONGDIR), \
		" ALT ",0xf8,high(FLDADR_GPGGA_ALT_NUMERIC),low(FLDADR_GPGGA_ALT_NUMERIC)," M", \
		" SPEED ",0xf9,high(FLDADR_GPRMC_SPEED_NUMERIC),low(FLDADR_GPRMC_SPEED_NUMERIC)," KNOTS", \
               	" COURSE ",0xfd,(FLDLEN_GPRMC_COURSE<<2)+high(FLDADR_GPRMC_COURSE),low(FLDADR_GPRMC_COURSE), \
		" AT ",0xfd,(2<<2)+high(FLDADR_GPRMC_DATE+4),low(FLDADR_GPRMC_DATE+4),"/", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_DATE+2),low(FLDADR_GPRMC_DATE+2),"/", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_DATE),low(FLDADR_GPRMC_DATE)," ", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME),low(FLDADR_GPRMC_TIME),":", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME+2),low(FLDADR_GPRMC_TIME+2),":", \
		0xfd,(2<<2)+high(FLDADR_GPRMC_TIME+4),low(FLDADR_GPRMC_TIME+4)," UTC", \
		" SATS ",0xfd,(FLDLEN_GPGGA_SATS<<2)+high(FLDADR_GPGGA_SATS),low(FLDADR_GPGGA_SATS),0
#endif
#endif


PHONE_STRING_STATUS_MESSAGE:
	.db	"SPDLMT=", \
		0xf8,high(STATUSMSG_SPEED_LIMIT), \
		low(STATUSMSG_SPEED_LIMIT), " STOPD=", \
		0xf8,high(STATUSMSG_TRACK_STOPPED_FIX_INTERVAL), \
		low(STATUSMSG_TRACK_STOPPED_FIX_INTERVAL),"/", \
		0xf8,high(STATUSMSG_TRACK_STOPPED_NOTIFY_DELAY), \
		low(STATUSMSG_TRACK_STOPPED_NOTIFY_DELAY)," BLKD=", \
		0xf8,high(STATUSMSG_TRACK_BLOCKED_FIX_INTERVAL), \
		low(STATUSMSG_TRACK_BLOCKED_FIX_INTERVAL),"/", \
		0xf8,high(STATUSMSG_TRACK_BLOCKED_NOTIFY_DELAY), \
		low(STATUSMSG_TRACK_BLOCKED_NOTIFY_DELAY)," MOVNG=", \
		0xf8,high(STATUSMSG_TRACK_MOVING_FIX_INTERVAL), \
		low(STATUSMSG_TRACK_MOVING_FIX_INTERVAL),"/", \
		0xf8,high(STATUSMSG_TRACK_MOVING_NOTIFY_FREQ), \
		low(STATUSMSG_TRACK_MOVING_NOTIFY_FREQ)," PSV=", \
		0xf8,high(STATUSMSG_POWERSAVE_PHONE_OFF_INTERVAL), \
		low(STATUSMSG_POWERSAVE_PHONE_OFF_INTERVAL),"/", \
		0xf8,high(STATUSMSG_POWERSAVE_PHONE_ON_INTERVAL), \
		low(STATUSMSG_POWERSAVE_PHONE_ON_INTERVAL)," 3D/FIX/BLKD=", \
		0xf8,high(STATUSMSG_GPS_FOUR_SAT_WAIT_TIME), \
		low(STATUSMSG_GPS_FOUR_SAT_WAIT_TIME),"/", \
		0xf8,high(STATUSMSG_GPS_FIX_WAIT_TIME), \
		low(STATUSMSG_GPS_FIX_WAIT_TIME),"/", \
		0xf8,high(STATUSMSG_BLOCKED_GPS_FIX_WAIT_TIME), \
		low(STATUSMSG_BLOCKED_GPS_FIX_WAIT_TIME)," MD/MS=", \
		0xf8,high(STATUSMSG_MIN_DISPLACEMENT), \
		low(STATUSMSG_MIN_DISPLACEMENT),"/", \
		0xf8,high(STATUSMSG_GPS_MOVING_THRESHOLD), \
		low(STATUSMSG_GPS_MOVING_THRESHOLD),"/", \
		0xf8,high(STATUSMSG_GPS_MOVING_RETRY_THRESHOLD), \
		low(STATUSMSG_GPS_MOVING_RETRY_THRESHOLD)," BAT=", \
		0xf8,high(STATUSMSG_BATTERY_CHARGE), \
		low(STATUSMSG_BATTERY_CHARGE)," SIG=", \
		0xf8,high(STATUSMSG_SIGNAL_STRENGTH), \
		low(STATUSMSG_SIGNAL_STRENGTH)," WDR=", \
		0xf8,high(STATUSMSG_WDR_COUNT), \
		low(STATUSMSG_WDR_COUNT)," V=0.17A Open GPS Tracker",0

;PHONE_STRING_WATCHDOG_RESET:
;	.db	"WATCHDOG RESET",0

	
FIX_TYPE_BASE:
FIX_TYPE_STOPPED:
	.db	"STOPPED",0
FIX_TYPE_STARTED:
	.db	"STARTED",0
FIX_TYPE_MOVING:
	.db	"MOVING",0
FIX_TYPE_MOVED:
	.db	"MOVED",0
FIX_TYPE_SPEEDING:
	.db	"SPEEDING",0
FIX_TYPE_LOCATE:
	.db	"LOCATE",0

LAST_GOOD_FIX_STRING:
	.db	", LAST GOOD FIX ", \
		0xfd,(2<<2)+high(LAST_GOOD_FIX_LATITUDE),low(LAST_GOOD_FIX_LATITUDE)," ", \
		0xfd,((FLDLEN_GPRMC_LATITUDE-2)<<2)+high(LAST_GOOD_FIX_LATITUDE+2),low(LAST_GOOD_FIX_LATITUDE+2)," ", \
		0xfd,(FLDLEN_GPRMC_LATDIR<<2)+high(LAST_GOOD_FIX_LATDIR),low(LAST_GOOD_FIX_LATDIR)," ", \
		0xfd,(3<<2)+high(LAST_GOOD_FIX_LONGITUDE),low(LAST_GOOD_FIX_LONGITUDE)," ", \
		0xfd,((FLDLEN_GPRMC_LONGITUDE-3)<<2)+high(LAST_GOOD_FIX_LONGITUDE+3),low(LAST_GOOD_FIX_LONGITUDE+3)," ", \
		0xfd,(FLDLEN_GPRMC_LONGDIR<<2)+high(LAST_GOOD_FIX_LONGDIR),low(LAST_GOOD_FIX_LONGDIR)," AT ", \
		0xfd,(2<<2)+high(LAST_GOOD_FIX_TIME),low(LAST_GOOD_FIX_TIME),":", \
		0xfd,(2<<2)+high(LAST_GOOD_FIX_TIME+2),low(LAST_GOOD_FIX_TIME+2),":", \
		0xfd,(2<<2)+high(LAST_GOOD_FIX_TIME+4),low(LAST_GOOD_FIX_TIME+4)," UTC",0

GPS_MATCH_PATTERN:
	.db	low(GPS_PARSE_GPRMC),high(GPS_PARSE_GPRMC),"$GPRMC",0, \
		low(GPS_PARSE_GPGGA),high(GPS_PARSE_GPGGA),"$GPGGA",0, \
		low(GPS_PARSE_OTHER),high(GPS_PARSE_OTHER),0

GPRMC_PARSE_STRING:
	.db	1,(FLDLEN_GPRMC_TIME<<2)+high(FLDADR_GPRMC_TIME),low(FLDADR_GPRMC_TIME), \
		2,(FLDLEN_GPRMC_FIXVALID<<2)+high(FLDADR_GPRMC_FIXVALID),low(FLDADR_GPRMC_FIXVALID), \
		3,(FLDLEN_GPRMC_LATITUDE<<2)+high(FLDADR_GPRMC_LATITUDE),low(FLDADR_GPRMC_LATITUDE), \
		4,(FLDLEN_GPRMC_LATDIR<<2)+high(FLDADR_GPRMC_LATDIR),low(FLDADR_GPRMC_LATDIR), \
		5,(FLDLEN_GPRMC_LONGITUDE<<2)+high(FLDADR_GPRMC_LONGITUDE),low(FLDADR_GPRMC_LONGITUDE), \
		6,(FLDLEN_GPRMC_LONGDIR<<2)+high(FLDADR_GPRMC_LONGDIR),low(FLDADR_GPRMC_LONGDIR), \
		7,(FLDLEN_GPRMC_SPEED<<2)+high(FLDADR_GPRMC_SPEED),low(FLDADR_GPRMC_SPEED), \
		8,(FLDLEN_GPRMC_COURSE<<2)+high(FLDADR_GPRMC_COURSE),low(FLDADR_GPRMC_COURSE), \
		9,(FLDLEN_GPRMC_DATE<<2)+high(FLDADR_GPRMC_DATE),low(FLDADR_GPRMC_DATE), \
		0xff

GPGGA_PARSE_STRING:
	.db	6,(FLDLEN_GPGGA_FIXVALID<<2)+high(FLDADR_GPGGA_FIXVALID),low(FLDADR_GPGGA_FIXVALID), \
		7,(FLDLEN_GPGGA_SATS<<2)+high(FLDADR_GPGGA_SATS),low(FLDADR_GPGGA_SATS), \
		9,(FLDLEN_GPGGA_ALT<<2)+high(FLDADR_GPGGA_ALT),low(FLDADR_GPGGA_ALT), \
		0xff

; Used for SETSPEED, SETTRACK, and SETPOWER
SETTINGS_PARSE_STRING:
	.db	1,(8<<2)+high(PARSE_FIELDS),low(PARSE_FIELDS), \
		2,(8<<2)+high(PARSE_FIELDS+8),low(PARSE_FIELDS+8), \
		3,(8<<2)+high(PARSE_FIELDS+16),low(PARSE_FIELDS+16), \
		4,(8<<2)+high(PARSE_FIELDS+24),low(PARSE_FIELDS+24), \
		5,(8<<2)+high(PARSE_FIELDS+32),low(PARSE_FIELDS+32), \
		6,(8<<2)+high(PARSE_FIELDS+40),low(PARSE_FIELDS+40), \
		7,(8<<2)+high(PARSE_FIELDS+48),low(PARSE_FIELDS+48), \
		8,(8<<2)+high(PARSE_FIELDS+56),low(PARSE_FIELDS+56), \
		0xff

SETSPEED_DECODE_STRING:
	.db	high(EEADR_SPEED_LIMIT) + (1<<DECODE_SETTING_ONE_BYTE), \
		low(EEADR_SPEED_LIMIT), \
		high(EEADR_GPS_MOVING_THRESHOLD) + (1<<DECODE_SETTING_ONE_BYTE), \
		low(EEADR_GPS_MOVING_THRESHOLD), \
		high(EEADR_GPS_MOVING_RETRY_THRESHOLD) + (1<<DECODE_SETTING_ONE_BYTE), \
		low(EEADR_GPS_MOVING_RETRY_THRESHOLD), \
		0

SETTRACK_DECODE_STRING:
	.db	high(EEADR_TRACK_STOPPED_FIX_INTERVAL) + (1<<DECODE_SETTING_TWO_BYTE), \
		low(EEADR_TRACK_STOPPED_FIX_INTERVAL), \
		high(EEADR_TRACK_STOPPED_NOTIFY_DELAY) + (1<<DECODE_SETTING_ONE_BYTE), \
		low(EEADR_TRACK_STOPPED_NOTIFY_DELAY), \
		high(EEADR_TRACK_BLOCKED_FIX_INTERVAL) + (1<<DECODE_SETTING_TWO_BYTE), \
		low(EEADR_TRACK_BLOCKED_FIX_INTERVAL), \
		high(EEADR_TRACK_BLOCKED_NOTIFY_DELAY) + (1<<DECODE_SETTING_ONE_BYTE), \
		low(EEADR_TRACK_BLOCKED_NOTIFY_DELAY), \
		high(EEADR_TRACK_MOVING_FIX_INTERVAL) + (1<<DECODE_SETTING_TWO_BYTE), \
		low(EEADR_TRACK_MOVING_FIX_INTERVAL), \
		high(EEADR_TRACK_MOVING_NOTIFY_FREQ) + (1<<DECODE_SETTING_ONE_BYTE), \
		low(EEADR_TRACK_MOVING_NOTIFY_FREQ), \
		high(EEADR_TRACK_MIN_DISPLACEMENT) + (1<<DECODE_SETTING_TWO_BYTE), \
		low(EEADR_TRACK_MIN_DISPLACEMENT), \
                0

SETPOWER_DECODE_STRING:
	.db	high(EEADR_POWERSAVE_PHONE_OFF_INTERVAL) + (1<<DECODE_SETTING_TWO_BYTE), \
		low(EEADR_POWERSAVE_PHONE_OFF_INTERVAL), \
		high(EEADR_POWERSAVE_PHONE_ON_INTERVAL) + (1<<DECODE_SETTING_TWO_BYTE), \
		low(EEADR_POWERSAVE_PHONE_ON_INTERVAL), \
		high(EEADR_GPS_FOUR_SAT_WAIT_TIME) + (1<<DECODE_SETTING_ONE_BYTE_X4), \
		low(EEADR_GPS_FOUR_SAT_WAIT_TIME), \
		high(EEADR_GPS_FIX_WAIT_TIME) + (1<<DECODE_SETTING_ONE_BYTE_X4), \
		low(EEADR_GPS_FIX_WAIT_TIME), \
		high(EEADR_BLOCKED_GPS_FIX_WAIT_TIME) + (1<<DECODE_SETTING_ONE_BYTE_X4), \
		low(EEADR_BLOCKED_GPS_FIX_WAIT_TIME), \
		0

; Format: Address high (2 bits) + length << 2, Address low, data
; Zero address high terminates
EEPROM_DEFAULTS:
	.db	high(EEADR_PASSWORD) + ( 4 << 2 ), \
		low(EEADR_PASSWORD),"GPS",0, \
		high(EEADR_DEVICENAME) + ( 1 << 2 ), \
		low(EEADR_DEVICENAME),0, \
		high(EEADR_EMAIL_ADDR) + ( 1 << 2 ), \
		low(EEADR_EMAIL_ADDR),0, \
		high(EEADR_GPS_FOUR_SAT_WAIT_TIME) + ( 21 << 2 ), \
		low(EEADR_GPS_FOUR_SAT_WAIT_TIME), \
		DEFAULT_FOUR_SAT_WAIT_TIME, \
		DEFAULT_GPS_FIX_WAIT_TIME, \
		DEFAULT_BLOCKED_GPS_FIX_WAIT_TIME, \
		DEFAULT_SPEED_LIMIT, \
		DEFAULT_GPS_MOVING_THRESHOLD, \
		DEFAULT_GPS_MOVING_RETRY_THRESHOLD, \
		high(DEFAULT_TRACK_STOPPED_FIX_INTERVAL), \
		low(DEFAULT_TRACK_STOPPED_FIX_INTERVAL), \
		DEFAULT_TRACK_STOPPED_NOTIFY_DELAY, \
		high(DEFAULT_TRACK_BLOCKED_FIX_INTERVAL), \
		low(DEFAULT_TRACK_BLOCKED_FIX_INTERVAL), \
		DEFAULT_TRACK_BLOCKED_NOTIFY_DELAY, \
		high(DEFAULT_TRACK_MOVING_FIX_INTERVAL), \
		low(DEFAULT_TRACK_MOVING_FIX_INTERVAL), \
		DEFAULT_TRACK_MOVING_NOTIFY_FREQ, \
		high(DEFAULT_POWERSAVE_PHONE_OFF_INTERVAL), \
		low(DEFAULT_POWERSAVE_PHONE_OFF_INTERVAL), \
		high(DEFAULT_POWERSAVE_PHONE_ON_INTERVAL), \
		low(DEFAULT_POWERSAVE_PHONE_ON_INTERVAL), \
		high(DEFAULT_TRACK_MIN_DISPLACEMENT), \
		low(DEFAULT_TRACK_MIN_DISPLACEMENT), \
		high(EEADR_PHONE_NUMBER) + ( 1 << 2 ), \
		low(EEADR_PHONE_NUMBER),0, \
		high(EEADR_EMAIL_ADDR) + ( 1 << 2 ), \
		low(EEADR_EMAIL_ADDR),0, \
		high(EEADR_CHECKCODE) + ( 2 << 2 ), \
		low(EEADR_CHECKCODE),"MI",0


; EOF
