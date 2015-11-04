; 	 ____________________________________________________________________________________________
;	|////////////////////////////////////////////////////////////////////////////////////////////|
;	|///|											|////|
;	|///| DRIVE	 : PS2 KEYBOARD DEVICE							|////|
;	|///| VERSAO	 : 2.0									|////|
;	|///| PROGRAMADOR: JOÃO DARTAGNAN ANTUNES OLIVEIRA				   	|////|
;	|///| DATA 	 : 10 de Julho de 2007 (19:39)						|////|
;	|///| DESCRIÇÃO	 : Emula Keyboard e Mouse PS2 						|////|
;	|///| OBJETIVO:	   Emular interface PS2 processar todos os comandos e enviar codigos    |////|
;	|///| 		   de teclado e rotinas de emulação de translação de ASCII codes para	|////|
;	|///|	           keyboard scan codes.							|////|
;	|///| RESOURCES	 : This drive use TIMER0 and 4 I/O bits					|////|
;	|///|											|////|
;	|///| OBS:	   This software is inteded to use in AVR with SRAM			|////|
;	|///|		   PS2 LINE RECQUIRE 10K RESISTORS AT CLOCK AND DATA LINES		|////|
;	|///|___________________________________________________________________________________|////|
;-------|////////////////////////////////////////////////////////////////////////////////////////////|
;	>--------------------------------------------------------------------------------------------<
;-------|											     |
;	|   		P S 2  K E Y B O A R D  D E V I C E	 				     |
;	| 											     |
;+------+--------------------------------------------------------------------------------------------+
;|
;| PS2 PROTOCOL USED TO IMPLEMENT PS2_KEYBOARD_AND_MOUSE_DEVICE
;|
;|
;|	COMMUNICATION BETWEEN HOST AND PS2 DEVICE
;|
;|	Before the keyboard microcontroller drives the PS2 DEVICE
;|	Clock and Data lines, it sets both lines to a high level to
;|	check the current line status. The three line status modes
;|	between the HOST and the PS2 DEVICE are the following:
;|
;|	- Two-way communication between PS2 DEVICE and HOST
;|	  if both Clock and Data lines are high
;|
;|	- HOST sends to PS2 DEVICE if Data line is low
;|
;|	- HOST inhibits communication if Clock is low
;|
;|	The HOST always drives the Clock and inhibits the
;|	communication by forcing the Clock line to a low level
;|	(inactive level), which keeps the keyboard from sending
;|	any data packet and from generating Clock pulses during
;|	this stage.
;|
;|	Once the Clock line releases high (active level), the
;|	keyboard has to check the Data line. When the Data line is
;|	inactive, the HOST requests to send the serial data to the
;|	keyboard. The keyboard has to send serial Clock streams
;|	to receive the data packet from the HOST. When both data
;|	and Clock lines are active, the keyboard sends the scan
;|	codes to the HOST at any time.
;|
;|	SERIAL DATA STREAM(PS2 DEVICE TO HOST)     TH    TL
;|						   v     v
;|      PS2 CLOCK ---+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-----------------
;|		     | | | | | | | | | | | | | | | | | | | | | |			
;|		     +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ 		
;|										
;|      PS2 DATA  --+---+---+---+---+---+---+---+---+---+---+---+---------------
;|		    | 0 | D0| D1| D2| D3| D4| D5| D6| D7| P | 1 |
;|		    +---+---+---+---+---+---+---+---+---+---+---+		
;|		    <-------------------- TB ------------------->
;|
;|	The serial data bit stream consists of 11 bits, which include
;|	a start bit, 8 data bits, odd parity bit, and a stop bit. When
;|	bit stream sends data to the PC, all the data bits are
;|	guaranteed while the Clock line is low. The data changes
;|	during a high level of the Clock line. When the bit stream
;|	arrives from the PC, the data fetches at the leading edge
;|	of the Clock. After the stop bit detects high, the KBC forces
;|	the Data line to a low level for one bit period. The start bit
;|	is always low and the stop bit is high. The 8-bit data
;|	transmits from the least significant bit (LSB). The odd parity
;|	bit means that the number of 1s for the data bit and the
;|	parity bit must be odd all the time.
;|
;|	SERIAL DATA COMMUNICATION FROM HOST TO PS2 LINE
;|
;|	Three kinds of data are handled between the PS2 DEVICE and the HOST
;|
;|	- Command and Acknowledge
;|	- Optional Data
;|	- Key Scan Code from the PS2 KEYBOARD FIFO Buffer
;|
;|	The command reception has the highest priority of the
;|	data communication. A new command always overrides
;|	to the old command even during communication. The HOST
;|	starts the command transfer by lowering the Data line.
;|	Then, the microcontroller sends eleven Clock pulses to
;|	receive the serial data packet from the PC. When the data
;|	arrives, the microcontroller sends an acknowledge (0FAh).
;|	The optional data arrives or departs after sending the
;|	acknowledge. The key scan code only sends from the
;|	keyboard FIFO buffer when no data is coming from the PC
;|
;|	SERIAL DATA STREAM(HOST TO PS2 DEVICE)			  TH     TL
;|								   v	 v
;|      PS2 CLOCK --+      +---------+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +---------------------
;|		    |      |   ^     | | | | | | | | | | | | | | | | | | | | | |
;|		    +------+   |     +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ 		
;|		    <- TI ->   |								
;| 		               +- RECEIVE CONDITION
;|			       |
;|      PS2 DATA  ---------+   |    +---+---+---+---+---+---+---+---+---+---+---+-------------------	
;|		           |   v    |D0 | D1| D2| D3| D4| D5| D6| D7| P | 1 | 0 |
;|		           +--------+---+---+---+---+---+---+---+---+---+---+---+
;|			   <- TREQ ->
;|				    <----------------- TB ---------------------->
;|
;|	TI 	= Inhibit Transmission from PS2 DEVICE to HOST time ~100us
;|	TREQ	= Resquest data transmission from HOST to DEVICE time 80us..10000us
;|	TB	= Byte transmission time ~880us
;|	TH	= Clock High time ~40us
;|	TL	= Clock Low time ~40us
;|
;|	TABLE OF COMMANDS SEND BY HOST TO PS2 KEYBOARD
;|
;|	COMMAND NAME		HEX VALUES		FUNCTION
;|
;|	STATUS_IND		ED,ACK,XX,ACK 		Set/Reset Lock Status indicators
;|	ECHO 			EE,EE (=ACK) 		Echo Command
;|	ALT_SCAN 		F0,ACK,XX,ACK 		Select Alternate Scan Codes
;|	TYPE_RATE DELAY 	F3,ACK,XX,ACK 		Set Typematic Rate/Delay
;|	ENABLE 			F4,ACK 			Enable Key Scanning
;|	DISABLE 		F5,ACK 			Default Disable
;|	SET_DEFAULT 		F6,ACK 			Set Default Value
;|	ALL_MAKE_TYPE 		F7,ACK 			Set All Keys - Typematic
;|	ALL_MAKE_BREAK 		F8,ACK 			Sent All Keys - Make/Break
;|	ALL MAKE 		F9,ACK 			Set All Keys - Make
;|	ALL_M_T_B 		FA,ACK 			Set All Keys Typematic/Make/Break
;|	KEY_MAKE_TYPE 		FB,ACK,XX,ACK 		Set Key Type - Typematic
;|	KEY_MAKE_BREAK 		FC,ACK,XX,ACK 		Set Key Type - Make/Break
;|	KEY MAKE 		FD,ACK,XX,ACK 		Set Key Type - Make
;|	RESEND 			FE 			Resend Command
;|	RESET 			FF,ACK,YY 		Reset Command
;|	READ ID 		F2,ACK, AB,83 		Read ID Command
;|
;|	Notes:
;|		ACK 	= Acknowledge Data to PC (0FAh)
;|		XX 	= Received Data from PC.
;|		YY 	= Result of Basic Assurance Test
;|		AB 83 	= ID Number
;|
;|	TABLE OF COMMANDS SEND BY PS2 KEYBOARD TO HOST
;|
;|	COMMAND NAME			HEX VALUE
;|
;|	Key Detection Error/Overrun 	00 (Code Set 2)
;|	Keyboard ID 			AB 83
;|	Bat Completion Code 		AA
;|	Bat Failure Core 		FC
;|	Echo 				EE
;|	Acknowledge 			FA	
;|	Resend 				FE
;|	Key Detection Error/Overrun 	FF (Code Set 1)
;|
;|
;|	TABLE OF COMMANDS SEND BY HOST TO PS2 MOUSE
;|
;|	COMMAND NAME		HEX VALUES		FUNCTION
;|
;|	RESET			FF,ACK,YY,ID		Reset 
;|	RESEND			FE,ACK			Resend
;|	SET_DEFAULT 		F6,ACK			Set Default
;|	ENABLE 			F4,ACK			Enable 
;|	DISABLE 		F5,ACK			Disable
;|	SET_SAMPLE_RATE		F3,ACK,SR,ACK		Set Sampling Rate
;|	READ_DEVICE_TYPE	F2,ACK,ID		Read Device Type
;|	SET_REMOTE_MODE 	F0,ACK			Set Remote Mode
;|	SET_WRAP_MODE 		EE,ACK			Set Wrap Mode
;|	RESET_WRAP_MODE		EC,ACK			Reset Wrap Mode
;|	READ_DATA		EB,ACK,DT1,DT2,DT3	Read data
;|	SET_STREAM_MODE		EA,ACK			Set Stream Mode
;|	STATUS_REQUEST		E9,ACK,ST1,ST2,ST3	Status Request
;|	SET_RESOLUTION		E8,ACK,RS,ACK		Set Resolution
;|	SET_SCALING_2X1		E7,ACK			Set Scaling 2x1
;|	RESET_SCALING		E6,ACK	 		Reset Scaling
;|
;|	Notes:
;|		YY 		= Result of Basic Assurance Test
;|		ACK 		= Acknowledge Data to PC (0FAh)
;|		SR		= SAMPLE RATE
;|		ID		= PS2(00h)
;|		DT1,DT2,DT3	= data packet formatted bellow
;|				  DT1
;|				  	bit 0=Right Button 1=pressed
;|				  	bit 1=Middle Button 1=pressed
;|				  	bit 2=Left Button 1=pressed
;|				  	bit 3=1
;|				  	bit 4=x sign bit 1=negative
;|				  	bit 5=y sign bit 1=negative
;|				 	bit 6=x overflow 1=overflow
;|				  	bit 7=y overflow 1=overflow
;|				  DT2=X movement
;|				  DT3=Y movement
;|		ST1,ST2,ST3	= status packet formatted bellow
;|				  ST1
;|				  	bit 0=Right Button 1=pressed
;|				  	bit 1=Middle Button 1=pressed
;|				  	bit 2=Left Button 1=pressed
;|				  	bit 3=0
;|				  	bit 4=Scaling 1=2x1 0=1x1
;|				  	bit 5=Enable 1=Enabled
;|				 	bit 6=Mode 0=Remote 1=Stream
;|				  	bit 7=0
;|				  ST2=Resolution
;|				  ST3=Sample Rate
;|				
;|		RS		= Resolution 
;|				  0=1 count/mm
;|				  1=2 count/mm
;|				  2=4 count/mm
;|				  3=8 count/mm
;|




;
;			 ///////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\
;			//						\\
;----------------------||  	       	 PS2 DEVICE DRIVE		||---------------------------
;			\\						//
			 \\\\\\\\\\\\\\\\\\\\\\\/////////////////////////

#ifndef __PS2_DEVICE_DRIVE__

	#define __AVR_DEVICE_DRIVE__

	.message "+-----------------------------+"
	.message "|            			|"
	.message "| PS2 KEYBOARD DEVICE DRIVE 	|"
	.message "|            			|"
	.message "+-----------------------------+"

	;+------------------------------------
	;| NEED INCLUDES
	;+------------------------------------
	
	#ifndef __CQUEUE8__
		.INCLUDE	"QUEUE\CIRCULAR_QUEUE\QUEUE_8BITS\CIRCULAR_QUEUE_8BITS.INC"
	#endif

	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: This object has a lot of function to control	|
	;|		PS2 line in Speaker mode allowing read and wri- |
	;|		te data thru this interface used commonly in 	|
	;|		MOUSE and KEYBOARD				|
	;>--------------------------------------------------------------<

	;>--------------------------------------------------------------<
	;| OBJECT DEFINITIONS						|
	;| COPY BELOW COMMENTS LINE CODE AND PUT INTO MAIN PROGRAM	|
	;| AND SET PORTS AND SAMPLE BELOW PORTD IS USED			|
	;>--------------------------------------------------------------<


	;-------------------------< PORT DEFINITONS >----------------------------

	;.EQU	_PS2_DEVICE_KEYBOARD_PORT_OUTPUT	= PORTD				;PS2 KEYBOARD PORTS
	;.EQU	_PS2_DEVICE_KEYBOARD_PORT_INPUT	= PIND
	;.EQU	_PS2_DEVICE_KEYBOARD_PORT_DIR		= DDRD


	;-------------------------< CONSTANT DEFINITONS >------------------------

	;.EQU	_PS2_DEVICE_KEYBOARD_BIT_CLOCK		= 2				;PS2 KEYBOARD CLOCK LINE
	;.EQU	_PS2_DEVICE_KEYBOARD_BIT_DATA		= 4				;PS2 KEYBOARD DATA LINE

	;-------------------------< DEVICE DEFINITIONS >--------------------------
	;
	;DEVICES NAMES
	;
	.EQU	_PS2_DEVICE_DEVICE_BIT			= 0
	.EQU	_PS2_DEVICE_DEVICE_KEYBOARD		= 0
	;
	;PS2 LINE STATUS
	;
	.EQU	_PS2_DEVICE_STATE_INVALID		= 0
	.EQU	_PS2_DEVICE_STATE_IDLE			= 1				;STATE=(DATA=HIGH,CLOCK=HIGH) ready to transmite data
	.EQU	_PS2_DEVICE_STATE_INHIBIT		= 2				;STATE=(DATA=HIGH,CLOCK=LOW) transmisions inhibit
	.EQU	_PS2_DEVICE_STATE_HOST_REQUEST		= 3				;STATE=(DATA=LOW,CLOCK=HIGH) host request transmision
	;
	;PS2 ERROR CODES
	;
	.EQU	_PS2_DEVICE_ERROR_OK			= 0				;no error
	.EQU	_PS2_DEVICE_ERROR_PARITY		= 1				;parity error
	;
	;PS2 COMMUNICATIONS PROCESS EVENTS
	;
	.EQU	_PS2_DEVICE_PROCESS_INTERRUPT_FREQUENCY	= 1000			;interrupt process frequency
	.EQU	_PS2_DEVICE_PROCESS_INTERRUPT_TIMER0_DIV	= _AVR_CLOCK/(64*_PS2_DEVICE_PROCESS_INTERRUPT_FREQUENCY)

	.EQU	_PS2_DEVICE_PROCESS_EVENT_KEYBOARD_HOST_TO_PS2	= 0
	.EQU	_PS2_DEVICE_PROCESS_EVENT_MOUSE_HOST_TO_PS2	= 2
	.EQU	_PS2_DEVICE_PROCESS_EVENT_KEYBOARD_PS2_TO_HOST	= 1
	.EQU	_PS2_DEVICE_PROCESS_EVENT_MOUSE_PS2_TO_HOST	= 3

	;
	;PS2 KEYBOARD FLAGS BITS
	;
	.EQU	_PS2_DEVICE_KEYBOARD_FLAG_SCROLL_LOCK_BIT	= 0				;SCROLL LOCK STATE 1=ON
	.EQU	_PS2_DEVICE_KEYBOARD_FLAG_NUM_LOCK_BIT		= 1				;NUM LOCK STATE 1=ON
	.EQU	_PS2_DEVICE_KEYBOARD_FLAG_CAPS_LOCK_BIT		= 2				;CAPS LOCK STATE 1=ON
	.EQU	_PS2_DEVICE_KEYBOARD_FLAG_SCAN_STATE_BIT	= 0				;SCANNING STATE 1=ENABLE
	.EQU	_PS2_DEVICE_KEYBOARD_FLAG_RESET_BIT		= 1				;RESET STATE 1=RESET
	;
	;KEYBOARD INPUT COMMANDS
	;
	.EQU	_PS2_DEVICE_KEYBOARD_CMDIN_STATE_IND		= 0XED			;Set/Reset Lock Status indicators
	.EQU	_PS2_DEVICE_KEYBOARD_CMDIN_ECHO			= 0XEE			;Echo Command
	.EQU	_PS2_DEVICE_KEYBOARD_CMDIN_ALT_SCAN 		= 0XF0			;Select Alternate Scan Codes
	.EQU	_PS2_DEVICE_KEYBOARD_CMDIN_TYPE_RATE_DELAY 	= 0XF3			;Set Typematic Rate/Delay
	.EQU	_PS2_DEVICE_KEYBOARD_CMDIN_ENABLE 		= 0XF4			;Enable Key Scanning
	.EQU	_PS2_DEVICE_KEYBOARD_CMDIN_DISABLE 		= 0XF5			;Default Disable
	.EQU	_PS2_DEVICE_KEYBOARD_CMDIN_SET_DEFAULT 		= 0XF6			;Set Default Value
	.EQU	_PS2_DEVICE_KEYBOARD_CMDIN_ALL_MAKE_TYPE 	= 0XF7			;Set All Keys - Typematic
	.EQU	_PS2_DEVICE_KEYBOARD_CMDIN_ALL_MAKE_BREAK 	= 0XF8			;Sent All Keys - Make/Break
	.EQU	_PS2_DEVICE_KEYBOARD_CMDIN_ALL_MAKE 		= 0XF9			;Set All Keys - Make
	.EQU	_PS2_DEVICE_KEYBOARD_CMDIN_ALL_MAKE_TYPE_BREAK	= 0XFA			;Set All Keys Typematic/Make/Break
	.EQU	_PS2_DEVICE_KEYBOARD_CMDIN_KEY_MAKE_TYPE 	= 0XFB			;Set Key Type - Typematic
	.EQU	_PS2_DEVICE_KEYBOARD_CMDIN_KEY_MAKE_BREAK	= 0XFC			;Set Key Type - Make/Break
	.EQU	_PS2_DEVICE_KEYBOARD_CMDIN_KEY_MAKE 		= 0XFD			;Set Key Type - Make
	.EQU	_PS2_DEVICE_KEYBOARD_CMDIN_RESEND 		= 0XFE 			;Resend Command
	.EQU	_PS2_DEVICE_KEYBOARD_CMDIN_RESET		= 0XFF			;Reset Command
	.EQU	_PS2_DEVICE_KEYBOARD_CMDIN_READ_ID		= 0XF2	 		;Read ID Command
	;
	;KEYBOARD OUTPUT COMMANDS
	;
	.EQU	_PS2_DEVICE_KEYBOARD_CMDOUT_OVERRUN_SET_2	= 0X00			;Key Detection Error/Overrun (Code Set 2)
	.EQU	_PS2_DEVICE_KEYBOARD_CMDOUT_ID_1		= 0XAB			;Keyboard ID 1st byte
	.EQU	_PS2_DEVICE_KEYBOARD_CMDOUT_ID_2		= 0X83			;Keyboard ID 2nd byte
	.EQU	_PS2_DEVICE_KEYBOARD_CMDOUT_BAT_COMPLETE	= 0XAA			;Bat Completion Code
	.EQU	_PS2_DEVICE_KEYBOARD_CMDOUT_BAT_FAILURE		= 0XFC			;Bat Failure Core
	.EQU	_PS2_DEVICE_KEYBOARD_CMDOUT_ECHO		= 0XEE			;Echo
	.EQU	_PS2_DEVICE_KEYBOARD_CMDOUT_ACK			= 0XFA			;Acknowledge
	.EQU	_PS2_DEVICE_KEYBOARD_CMDOUT_RESEND		= 0XFE			;Resend
	.EQU	_PS2_DEVICE_KEYBOARD_CMDOUT_OVERRUN_SET_1	= 0XFF			;key Detection Error/Overrun (Code Set 1)

	.EQU	_PS2_DEVICE_KEYBOARD_QUEUE_SIZE			= 16			;This size is fixed d'ont change


;-------------------------< SRAM  DEFINITONS >------------------------

.DSEG
	_PS2_DEVICE_DATA_OUT:			.BYTE	_BYTE			;PS2 data out
	_PS2_DEVICE_DATA_IN:			.BYTE	_BYTE			;PS2 data in
	_PS2_DEVICE_EVENT_COUNT:		.BYTE	_BYTE			;PS2 interrupt haddle event count
	;/////////////////////////////
	;// KEYBOARD DEFINITIONS
	;/////////////////////////////
	_PS2_DEVICE_KEYBOARD_DATA_OUT:		.BYTE	_BYTE			;KEYBOARD data to send via PS2 line
	_PS2_DEVICE_KEYBOARD_DATA_IN:		.BYTE	_BYTE			;KEYBOARD data to be read from PS2 line
	_PS2_DEVICE_KEYBOARD_STATE_INDICATOR:	.BYTE	_BYTE			;KEYBOARD STATUS INDICATOR
										;BIT 0 Scroll Lock Indicator
										;BIT 1 Num Lock Indicator
										;BIT 2 Caps Lock Indicator
	_PS2_DEVICE_KEYBOARD_KEY3:		.BYTE	_BYTE			;SCAN KEY CODE FOR SCAN 3
	_PS2_DEVICE_KEYBOARD_SCAN_CODE_TYPE:	.BYTE	_BYTE			;ACTUAL SCAN TYPE 1,2 OR 3
	_PS2_DEVICE_KEYBOARD_TYPE_RATE_DELAY:	.BYTE	_BYTE			;ACTUAL TYPE RATE AND DELAY
	_PS2_DEVICE_KEYBOARD_FLAGS	:	.BYTE	_BYTE			;BIT 0=SCAN STATE
										;BIT 1=RESET
	_PS2_DEVICE_KEYBOARD_PC_STATE:		.BYTE	_WORD			;KEYBOARD process PC address status
	_PS2_DEVICE_KEYBOARD_QUEUE:		.BYTE	_PS2_DEVICE_KEYBOARD_QUEUE_SIZE+4	;KEYBOARD queue size 
												;4+ because CQUEUE8 recquired 
.CSEG

;-------------------------< MACROS  DEFINITONS >------------------------

	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_KEYBOARD_INIT			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Initialize PS2_LISTENER engine			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	Interrupt are disabled during interrupt		|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_PS2_DEVICE_INIT:
		push	Acc						;save Acc
		in	Acc,SREG					;save SREG
		cli							;disable interrupts
		push	Acc
		;////////////////////////////////////////
		;// PROGRAM PS2 KEYBOARD AND MOUSE LINE
		;// AS INPUT AND HI-IMPEDANCE
		;////////////////////////////////////////
		cbi	_PS2_DEVICE_KEYBOARD_PORT_OUTPUT,	\
			_PS2_DEVICE_KEYBOARD_BIT_CLOCK			;set KEYBOARD clock line to hiz
		cbi	_PS2_DEVICE_KEYBOARD_PORT_OUTPUT,	\
			_PS2_DEVICE_KEYBOARD_BIT_DATA			;set KEYBOARD data line to hiz
		cbi	_PS2_DEVICE_KEYBOARD_PORT_DIR,		\
			_PS2_DEVICE_KEYBOARD_BIT_CLOCK			;set KEYBOARD clock line as input
		cbi	_PS2_DEVICE_KEYBOARD_PORT_DIR,		\
			_PS2_DEVICE_KEYBOARD_BIT_DATA			;set KEYBOARD data line as input
		;////////////////////////////////////////
		;// INIT SETTINGS
		;////////////////////////////////////////
		ldi	Acc,_PS2_DEVICE_PROCESS_EVENT_KEYBOARD_HOST_TO_PS2	;initialize event count
		sts	_PS2_DEVICE_EVENT_COUNT,Acc
		rcall	_PS2_DEVICE_KEYBOARD_SET_VARIABLES		;set default variables
		rcall	_PS2_DEVICE_KEYBOARD_SET_DEFAULT		;set default parameters
		;////////////////////////////////////////
		;// PROGRAM TIMER0 TO GENERATE 1Khz 
		;// OF INTERRUPT DRIVE ROUTINE PROCESS
		;//
		;// prescaler 64x
		;// TIMER0_DIV = CRISTAL/64/(Khz)
		;////////////////////////////////////////
		ldi	Acc,_TIMER_DIV_64				;prescaler  64x
		out	TCCR0,Acc
		ldi	Acc,-_PS2_DEVICE_PROCESS_INTERRUPT_TIMER0_DIV	;division factor to obtain correcty interrupt
									;frequency
		out	TCNT0,Acc
		in	Acc,TIMSK					;enable timer0 interrupt
		ori	Acc,(1<<TOV0)
		out	TIMSK,Acc
		_SET_HANDDLE _HDC_TIMER0_OVF_VECT,_PS2_DEVICE_PROCESS	;set Drive Process handdle routine
		;////////////////////////////////////////
		;// WAIT 20ms for POWER stabilish
		;////////////////////////////////////////
		_M_DELAY_MILLISECONDS	20				;wait 20 to PS2 line stabilize
		pop	Acc
		out	SREG,Acc					;restore SREG
		pop	Acc						;restore Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_KEYBOARD_SET_DEFAULT			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Set KEYBOARD default state			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_PS2_DEVICE_KEYBOARD_SET_DEFAULT:
		push	Acc
		ldi	Acc,0b00101011							;set rate=10.9char/sec delay=500ms
		sts	_PS2_DEVICE_KEYBOARD_TYPE_RATE_DELAY,Acc
		ldi	Acc,2								;Scan type 2
		sts	_PS2_DEVICE_KEYBOARD_SCAN_CODE_TYPE,Acc
		ldi	Acc,0								;All Indicadors off
		sts	_PS2_DEVICE_KEYBOARD_STATE_INDICATOR,Acc
		ldi	Acc,(1<<_PS2_DEVICE_KEYBOARD_FLAG_SCAN_STATE_BIT)| \
			    (0<<_PS2_DEVICE_KEYBOARD_FLAG_RESET_BIT) 			;SCAN<-1,RESET<-0
		sts	_PS2_DEVICE_KEYBOARD_FLAGS,Acc
		pop	Acc
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_KEYBOARD_SET_VARIABLES		|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Set KEYBOARD default drive variables		|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_PS2_DEVICE_KEYBOARD_SET_VARIABLES:
		push	Acc
		pushw	Z
		;//////////////////////
		;// KEYBOARD
		;//////////////////////
		ldi	Acc,0
		sts	_PS2_DEVICE_KEYBOARD_PC_STATE+0,Acc			;clear PC status
		sts	_PS2_DEVICE_KEYBOARD_PC_STATE+1,Acc
		ldiw	Z,_PS2_DEVICE_KEYBOARD_QUEUE
		ldi	Acc,_PS2_DEVICE_KEYBOARD_QUEUE_SIZE
		rcall	_CQUEUE8_INIT						;init KEYBOARD queue
		popw	Z
		pop	Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_RESTORE_PC_STATE			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Restore PC state and jump to it or branch if 0	|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z---> AREA WHERE PC IS RESTORED			|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	Acc,AccH,Z					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|		
	_PS2_DEVICE_RESTORE_PC_STATE:
		ld	Acc,Z+						;get PC state
		ld	AccH,Z+
;		movw	ZL,Acc						;Z<-ump addres
		mov	ZL,Acc
		mov	ZH,AccH
		cpi	Acc,0
		ldi	Acc,0
		cpc	AccH,Acc
		breq	_PS2_DEVICE_RESTORE_PC_STATE_10			;yes, branch
		popaw							;restore call return point
		ijmp							;jump to address
	_PS2_DEVICE_RESTORE_PC_STATE_10:
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_CLEAR_PC_STATE			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Clear PC				 	|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z---> AREA WHERE PC IS CLEAD			|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	Z						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|		
	_PS2_DEVICE_CLEAR_PC_STATE:
		push	Acc
		ldi	Acc,0
		st	Z+,Acc
		st	Z+,Acc	
		pop	Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_CLEAR_PC_STATE			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: save PC and jump to address		 	|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z---> AREA WHERE PC IS CLEAD			|
	;|		X---> AREA TO JUMP				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	Acc,AccH					|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|		
	_PS2_DEVICE_SAVE_PC_STATE:
		popaw							;restore PC address
		st	Z+,Acc						;save
		st	Z+,AccH
;		movw	ZL,XL						;get exit address
		mov	ZL,XL
		mov	ZH,XH
		ijmp							;jump to it

	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_KEYBOARD_SET_FLAGS			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: SET KEYBOARD FLAGS			 	|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc mask of bits flags to be set		|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	one more two flag may be set at a time		|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|		
	_PS2_DEVICE_KEYBOARD_SET_FLAGS:
		push	AccH							;save useds
		push	Temp
		in	AccH,SREG						;save SREG
		cli
		lds	Temp,_PS2_DEVICE_KEYBOARD_FLAGS				;get flags
		or	Temp,Acc						;set bits
	_PS2_DEVICE_KEYBOARD_SET_FLAGS_10:
		sts	_PS2_DEVICE_KEYBOARD_FLAGS,Temp				;store back
		out	SREG,AccH						;restore SREG
		pop	Temp							;restore useds
		pop	AccH
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_KEYBOARD_RESET_FLAGS			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: RESET KEYBOARD FLAGS			 	|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc mask of bits flags to be reset		|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	one more two flag may be set at a time		|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|		
	_PS2_DEVICE_KEYBOARD_RESET_FLAGS:
		push	AccH							;save useds
		push	Temp
		in	AccH,SREG						;save SREG
		cli
		lds	Temp,_PS2_DEVICE_KEYBOARD_FLAGS				;get flags
		com	Acc
		and	Temp,Acc						;reset
		rjmp	_PS2_DEVICE_KEYBOARD_SET_FLAGS_10

	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_KEYBOARD_GET_FLAG_STATE		|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Get state of keyboard flag	 		|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc mask of bits of flags			|
	;| OUTPUT     :	Cy=1 if set					|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|		
	_PS2_DEVICE_KEYBOARD_GET_FLAG_STATE:
		push	Temp
		lds	Temp,_PS2_DEVICE_KEYBOARD_FLAGS				;get flags
		and	Temp,AccH						;mask flag			
		breq	_PS2_DEVICE_KEYBOARD_GET_FLAG_STATE_10			;branch if disable
		sec								;set enable
		pop	Temp
		ret
	_PS2_DEVICE_KEYBOARD_GET_FLAG_STATE_10:
		clc								;set disable
		pop	Temp
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_KEYBOARD_DEC_G			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Decrement G PS2 KEYBOARD circular queue		|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_PS2_DEVICE_KEYBOARD_DEC_G:
		pushw	Z
		ldiw	Z,_PS2_DEVICE_KEYBOARD_QUEUE
		rcall	_CQUEUE8_DEC_G						;G--
		popw	Z
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE		|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Put data into PS2 KEYBOARD circular queue	|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc  data					|
	;| OUTPUT     :	cy=1 if queue full				|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	if queue is full data dont placed into		|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE:
		pushw	Z
		ldiw	Z,_PS2_DEVICE_KEYBOARD_QUEUE
		rcall	_CQUEUE8_PUT_DATA					;put data into circular queue
		popw	Z
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_KEYBOARD_QUEUE_EMPTY			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Check if PS2 KEYBOARD circular queue is empty	|
	;>--------------------------------------------------------------<
	;| INPUT      :	----						|
	;| OUTPUT     :	cy=1 if empty					|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_PS2_DEVICE_KEYBOARD_QUEUE_EMPTY:
		pushw	Z
		ldiw	Z,_PS2_DEVICE_KEYBOARD_QUEUE
		rcall	_CQUEUE8_EMPTY					;check if empty
		popw	Z
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_KEYBOARD_GET_DATA_QUEUE		|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Get data from PS2 KEYBOARD circular queue	|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Acc Data					|
	;|		cy=1 if queue empty				|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_PS2_DEVICE_KEYBOARD_GET_DATA_QUEUE:
		pushw	Z
		ldiw	Z,_PS2_DEVICE_KEYBOARD_QUEUE
		rcall	_CQUEUE8_GET_DATA					;get data for circular queue
		popw	Z
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_GET_LINE_STATE			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Check status of PS2 line 			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Acc State					|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	States may be one as bellow			|
	;|								|
	;|		_PS2_DEVICE_STATE_IDLE				|
	;|		_PS2_DEVICE_STATE_INHIBIT			|
	;|		_PS2_DEVICE_STATE_HOST_REQUEST			|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_PS2_DEVICE_GET_LINE_STATE:
		;/////////////////////////////////
		;// RETURN STATUS OF KEYBOARD LINE
		;/////////////////////////////////
		in	Acc,_PS2_DEVICE_KEYBOARD_PORT_INPUT			;get KEYBOARD status
		andi	Acc,(1<<_PS2_DEVICE_KEYBOARD_BIT_DATA) | \
			    (1<<_PS2_DEVICE_KEYBOARD_BIT_CLOCK) 		;mask data and clock
		cpi	Acc,(1<<_PS2_DEVICE_KEYBOARD_BIT_DATA) | \
			    (1<<_PS2_DEVICE_KEYBOARD_BIT_CLOCK) 		;line IDLE?
		brne	_PS2_DEVICE_KEYBOARD_GET_LINE_STATE_10			;no, branch
		ldi	Acc,_PS2_DEVICE_STATE_IDLE				;else return IDLE state
		ret
	_PS2_DEVICE_KEYBOARD_GET_LINE_STATE_10:
		cpi	Acc,(1<<_PS2_DEVICE_KEYBOARD_BIT_DATA) | \
			    (0<<_PS2_DEVICE_KEYBOARD_BIT_CLOCK) 		;line INHIBIT?
		brne	_PS2_DEVICE_KEYBOARD_GET_LINE_STATE_20			;no, branch
		ldi	Acc,_PS2_DEVICE_STATE_INHIBIT				;else return INHIBIT
		ret
	_PS2_DEVICE_KEYBOARD_GET_LINE_STATE_20:
		cpi	Acc,(0<<_PS2_DEVICE_KEYBOARD_BIT_DATA) | \
			    (1<<_PS2_DEVICE_KEYBOARD_BIT_CLOCK) 		;line HOST REQUEST?
		brne	_PS2_DEVICE_KEYBOARD_GET_LINE_STATE_20A			;no, branch
		ldi	Acc,_PS2_DEVICE_STATE_HOST_REQUEST			;HOST REQUEST
		ret
	_PS2_DEVICE_KEYBOARD_GET_LINE_STATE_20A:
		ldi	Acc,_PS2_DEVICE_STATE_INVALID				;invalid 
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_DEVICE_TO_HOST			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Send DEVICE data to HOST			|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc Data					|
	;| OUTPUT     :	cy=1 if line in inhibit state or host request	|
	;|		     during transmision				|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	~880us						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	if this routine exit with cy=1 data not send out|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|						
	;/ KEYBOARD TRANSMITION EVENTS NUMBERS
	;|
	;| EVENT NUMBER -    	             1		         2
	;| 		   1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3	
	;| KEYBOARD CLOCK ---+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-----------------
	;|		     | | | | | | | | | | | | | | | | | | | | | |		
	;|		     +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ 	
	;|		     TL    TH
	;|										
	;| KEYBOARD DATA ---+---+---+---+---+---+---+---+---+---+---+---+---------------
	;|		    | 0 | D0| D1| D2| D3| D4| D5| D6| D7| P | 1 |		
	;|		    +---+---+---+---+---+---+---+---+---+---+---+		
	;|		    <----------------- TB ---------------------->
	;|
	;|	D0..D7  = DATA BITS
	;|	P	= PARITY BIT
	;|	TREQ	= Resquest data transmission from HOST to DEVICE time 80us..10000us
	;|	TB	= Byte transmission time ~880us
	;|	TH	= Clock High time ~40us
	;|	TL	= Clock Low time ~40us
	;|
	;| EVENT NUMBER	     	DESCRIPTION
	;| ------------		-----------------------------------
	;|								
	;|	1		PS2 Device check line, if in Idle start then start transmision else exit
	;|	2		PS2 Device clock=lo, data=start bit(low)
	;|	3		PS2 Device clock=hi,check line for host request then exit if so
	;|	4		PS2 Device clock=lo, data=DO
	;|	5		PS2 Device clock=hi,check line for host request then exit if so
	;|	6		PS2 Device clock=lo, data=D1
	;|	7		PS2 Device clock=hi,check line for host request then exit if so
	;|	8		PS2 Device clock=lo, data=D2
	;|	9		PS2 Device clock=hi,check line for host request then exit if so
	;|	10		PS2 Device clock=lo, data=D3
	;|	11		PS2 Device clock=hi,check line for host request then exit if so
	;|	12		PS2 Device clock=lo, data=D4
	;|	13		PS2 Device clock=hi,check line for host request then exit if so
	;|	14		PS2 Device clock=lo, data=D5
	;|	15		PS2 Device clock=hi,check line for host request then exit if so
	;|	16		PS2 Device clock=lo, data=D6
	;|	17		PS2 Device clock=hi,check line for host request then exit if so
	;|	18		PS2 Device clock=lo, data=D7
	;|	19		PS2 Device clock=hi,check line for host request then exit if so
	;|	20		PS2 Device clock=lo, data=Parity
	;|	21		PS2 Device clock=hi,check line for host request then exit if so
	;|	22		PS2 Device clock=lo, data=stop bit(high)
	;|	23		PS2 Device clock=hi
	;\---------------------------------------------------------------------------------------/
	_PS2_DEVICE_DEVICE_TO_HOST:
		pushaw								;save useds
		pushawt
		sts	_PS2_DEVICE_DATA_OUT,Acc				;set DATA temp
	_PS2_DEVICE_DEVICE_TO_HOST_10:
		rcall	_PS2_DEVICE_GET_LINE_STATE				;get PS2 line status
;		cpi	Acc,_PS2_DEVICE_STATE_INVALID				;invalid state?
;		breq	_PS2_DEVICE_DEVICE_TO_HOST_10				;wait
		cpi	Acc,_PS2_DEVICE_STATE_INHIBIT				;line INHIBIT?
		breq	_PS2_DEVICE_DEVICE_TO_HOST_10				;yes, wait line release
		cpi	Acc,_PS2_DEVICE_STATE_HOST_REQUEST			;host REQUEST ?
		lbreq	_PS2_DEVICE_DEVICE_TO_HOST_EXIT_REQUEST			;yes, exit
		;//////////////////////////////////
		;// START DATA TRANSMISSION PS2 LINE STATE
		;//////////////////////////////////
		lds	Acc,_PS2_DEVICE_DATA_OUT				;get data
		ldi	AccTH,0							;set no host request
		clc								;DATA=LO(START BIT)
		rcall	_PS2_DEVICE_SET_DATA
		clc
		rcall	_PS2_DEVICE_SET_CLOCK					;clock lo
		sec
		rcall	_PS2_DEVICE_SET_CLOCK					;clock hi
		sbrc	AccTH,0							;skip if line INHIBIT
		rjmp	_PS2_DEVICE_DEVICE_TO_HOST_EXIT_REQUEST			;else exit with host request
		ldi	AccH,0							;parity counter
		;//////////////////////////////////
		;// SEND DATA 8 DATA BITS
		;//////////////////////////////////
		ldi	AccT,8							;bit counter
	_PS2_DEVICE_DEVICE_TO_HOST_20:
		sbrc	Acc,0							;skip if bit=0
		inc	AccH							;else increment parity count
		lsr	Acc							;output a bit
		rcall	_PS2_DEVICE_SET_DATA
		clc
		rcall	_PS2_DEVICE_SET_CLOCK					;clock lo
		sec
		rcall	_PS2_DEVICE_SET_CLOCK					;clock hi
		sbrc	AccTH,0							;skip if no host request
		rjmp	_PS2_DEVICE_DEVICE_TO_HOST_EXIT_REQUEST			;else exit with  line INHIBIT
		dec	AccT							;bit.count--
		brne	_PS2_DEVICE_DEVICE_TO_HOST_20				;until all transfered
		;//////////////////////////////////
		;// SEND PARITY BIT
		;//////////////////////////////////
		ldi	AccT,1							;put parity bit into cy
		eor	AccH,AccT
		ror	AccH
		rcall	_PS2_DEVICE_SET_DATA
		clc
		rcall	_PS2_DEVICE_SET_CLOCK					;clock lo
		sec
		rcall	_PS2_DEVICE_SET_CLOCK					;clock hi
		sbrc	AccTH,0							;skip if no host request
		rjmp	_PS2_DEVICE_DEVICE_TO_HOST_EXIT_REQUEST			;else exit with line INHIBIT
		;//////////////////////////////////
		;// SEND STOP BIT
		;//////////////////////////////////
		sec
		rcall	_PS2_DEVICE_SET_DATA					;DATA=HI
		clc
		rcall	_PS2_DEVICE_SET_CLOCK					;clock lo
		sec
		rcall	_PS2_DEVICE_SET_CLOCK					;clock hi
		;//////////////////////////////////
		;// CHECK STATUS AGAIN
		;//////////////////////////////////
	_PS2_DEVICE_DEVICE_TO_HOST_30:
		rcall	_PS2_DEVICE_GET_LINE_STATE				;get PS2 line status
;		cpi	Acc,_PS2_DEVICE_STATE_INVALID				;invalid state?
;		breq	_PS2_DEVICE_DEVICE_TO_HOST_30				;wait
		cpi	Acc,_PS2_DEVICE_STATE_INHIBIT				;line INHIBIT?
		breq	_PS2_DEVICE_DEVICE_TO_HOST_30				;yes, wait line release
		cpi	Acc,_PS2_DEVICE_STATE_HOST_REQUEST			;host REQUEST ?
		breq	_PS2_DEVICE_DEVICE_TO_HOST_EXIT_REQUEST			;yes, exit
		clc								;set transmission ok
		popawt
		popaw
		ret
	_PS2_DEVICE_DEVICE_TO_HOST_EXIT_REQUEST:
		sec
		popawt
		popaw
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_GET_DATA				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: GET PS2 DATA LINE STATE				|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	CY=1 if data high				|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_PS2_DEVICE_GET_DATA:
		;//////////////////////////////////
		;// GET DATA FROM KEYBOARD
		;//////////////////////////////////
		clc								;assume data= low
		sbic	_PS2_DEVICE_KEYBOARD_PORT_INPUT,	\
			_PS2_DEVICE_KEYBOARD_BIT_DATA				;get data, skip if low
		sec								;else data=high
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_SET_DATA				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: SET PS2 DATA LINE LO OR HI			|
	;>--------------------------------------------------------------<
	;| INPUT      :	CY   0 data LO 1 data HI			|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_PS2_DEVICE_SET_DATA:
		;//////////////////////////////////
		;// SET KEYBOARD DATA
		;//////////////////////////////////
		brcc	_PS2_DEVICE_SET_DATA_10					;brach if carry=0
		cbi	_PS2_DEVICE_KEYBOARD_PORT_DIR,	\
			_PS2_DEVICE_KEYBOARD_BIT_DATA				;KEYBOARD DATA =HI
		ret
	_PS2_DEVICE_SET_DATA_10:
		sbi	_PS2_DEVICE_KEYBOARD_PORT_DIR,	\
			_PS2_DEVICE_KEYBOARD_BIT_DATA				;KEYBOARD DATA LO
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_SET_CLOCK				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: SET PS2 CLOCK LINE LO OR HI and make 30us Delay	|
	;>--------------------------------------------------------------<
	;| INPUT      :	CY   0 data LO 1 data HI			|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_PS2_DEVICE_SET_CLOCK:
		;//////////////////////////////////
		;// SET KEYBOARD CLOCK
		;//////////////////////////////////
		brcc	_PS2_DEVICE_SET_CLOCK_10				;brach if carry=0
		cbi	_PS2_DEVICE_KEYBOARD_PORT_DIR,	\
			_PS2_DEVICE_KEYBOARD_BIT_CLOCK				;KEYBOARD CLOCK HI
		rcall	_PS2_DEVICE_CLOCK_TIME					;Delay 20Us
		sbis	_PS2_DEVICE_KEYBOARD_PORT_INPUT, \
			_PS2_DEVICE_KEYBOARD_BIT_CLOCK				;skip if host not request
		ldi	AccTH,1							;else set host request
		rjmp	_PS2_DEVICE_SET_CLOCK_EXIT
	_PS2_DEVICE_SET_CLOCK_10:
		sbi	_PS2_DEVICE_KEYBOARD_PORT_DIR,	\
			_PS2_DEVICE_KEYBOARD_BIT_CLOCK				;KEYBOARD CLOCK LO
		rcall	_PS2_DEVICE_CLOCK_TIME					;Delay 20Us
		rjmp	_PS2_DEVICE_SET_CLOCK_EXIT
	_PS2_DEVICE_SET_CLOCK_EXIT:
		rcall	_PS2_DEVICE_CLOCK_TIME					;Delay 20Us
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_CLOCK_TIME				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: PS2 CLOCK DELAY 20us				|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------
	;| Make a 15us Delay (1/4 PS2 line period)
	;| time equation: (9+Acc*3+3(rcall))*----------- 
	;|				      AVR_CLOCK  
	_PS2_DEVICE_CLOCK_TIME:
		push	Acc
		ldi	Acc,((20*_AVR_CLOCK)/1000000-11)/3
		dec	Acc
		brne	PC-1
		pop	Acc
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_GET_RATE_DELAY INDEX			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: GET RATE DELAY					|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	AccH:Acc   Rate chars/sec x 10			|
	;|		AccTH:AccT Rate delay in milliseconds		|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	_PS2_DEVICE_GET_RATE_DELAY:
		push	r0
		pushw	Z
		lds	Acc,_PS2_DEVICE_KEYBOARD_TYPE_RATE_DELAY	;get rate delay
		andi	Acc,0b00011111					;mask rate
		lsl	Acc						;compute entry point
		lsl	Acc
		ldiw	Z,_PS2_DEVICE_GET_RATE_DELAY_TABLE*2		;Z-->rate table
		add	ZL,Acc
		ldi	Acc,0
		adc	ZH,Acc
		lpm							;get Rate
		mov	Acc,r0
		adiw	ZL,1
		lpm
		mov	AccH,r0
		adiw	ZL,1	
		lpm
		mov	AccT,r0						;get Rate time
		adiw	ZL,1
		lpm
		mov	AccTH,r0
		popw	Z
		pop	r0
		ret

	;@--------------------------------------@
	;| RATE DELAY TABLE WITH RATE AND TIME	|
	;@--------------------------------------@
	_PS2_DEVICE_GET_RATE_DELAY_TABLE:	
		.DW	300,34
		.DW	267,38
		.DW	240,42
		.DW	218,46
		.DW	200,50
		.DW	185,54
		.DW	171,59
		.DW	160,63
		.DW	150,67
		.DW	133,75
		.DW	120,83
		.DW	109,92
		.DW	100,100
		.DW	 92,109
		.DW	 86,116
		.DW	 80,125
		.DW	 75,133
		.DW	 67,149
		.DW	 60,167
		.DW	 55,182
		.DW	 50,200
		.DW	 46,217
		.DW	 43,233
		.DW	 40,250
		.DW	 37,270
		.DW	 33,303
		.DW	 30,333
		.DW	 27,370
		.DW	 25,400
		.DW	 23,435
		.DW	 21,476
		.DW	 20,500



	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_HOST_TO_DEVICE			|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Get Device data from Host			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	cy=1 if parity received with error,or host not	|
	;|		in Host Request state				|
	;|		Acc Data from host				|
	;| DESTROYED  :	Flags						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	Call this function only if PS2 line in Host	|
	;|		request state, not in this state routine exit	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;/ PS2 HOST TO PS2 DEVICE EVENTS NUMBERS
	;|					
	;| EVENT NUMBER  - 			         1 		     2
	;| 		    1      2       3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 
	;|      PS2 CLOCK --+      +-------+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +---------------------
	;|		    |      |   ^   | | | | | | | | | | | | | | | | | | | | | |
	;|		    +------+   |   +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ 		
	;|		    <- TI ->   |								
	;| 		               +- RECEIVE CONDITION
	;|			       |					      	
	;|      PS2 DATA  ---------+   |    +---+---+---+---+---+---+---+---+---+---+---+-------------------	
	;|		           |   v    |D0 | D1| D2| D3| D4| D5| D6| D7| P | 1 |ACK|
	;|		           +--------+---+---+---+---+---+---+---+---+---+---+---+
	;|			   <- TREQ ->
	;|				    <----------------- TB ---------------------->
	;|
	;|	D0..D7  = DATA BITS
	;|	P	= PARITY BIT
	;|	ACK	= LOW LEVEL, DEVICE PS2 SIGNALING TO HOST ACKNOWLEDGE
	;|	TI 	= Inhibit Transmission from PS2 DEVICE to HOST time ~100us
	;|	TREQ	= Resquest data transmission from HOST to DEVICE time 80us..10000us
	;|	TB	= Byte transmission time ~880us
	;|	TH	= Clock High time ~40us
	;|	TL	= Clock Low time ~40us
	;|
	;| EVENT NUMBER	     	DESCRIPTION
	;| ------------		-----------------------------------
	;|								
	;|	1		Host put clock low to disable any transmission in course
	;|	2		Host Release clock to high level and put data low for Host request state
	;|			PS2 Device check this line state no with more 10ms time sample
	;|	3		PS2 Device answer Host request put clock in low and put D0 state in data line
	;|	4		PS2 Device put line in High
	;|	5		PS2 Device put D1 in data line,clock low
	;|	6		PS2 Device put line in High
	;|	7		PS2 Device put D2 in data line,clock low
	;|	8		PS2 Device put line in High
	;|	9		PS2 Device put D3 in data line,clock low
	;|	10		PS2 Device put line in High
	;|	11		PS2 Device put D4 in data line,clock low
	;|	12		PS2 Device put line in High
	;|	13		PS2 Device put D5 in data line,clock low
	;|	14		PS2 Device put line in High
	;|	15		PS2 Device put D6 in data line,clock low
	;|	16		PS2 Device put line in High
	;|	17		PS2 Device put D7 in data line,clock low
	;|	18		PS2 Device clock high
	;|	19		PS2 Device put Parity in data line,clock low
	;|	20		PS2 Device clock high
	;|	21		PS2 Device put 1(stop bit) in data line,clock low
	;|	22		PS2 Device clock high
	;|	23		PS2 Device put low in data line if command received ok,clock low
	;|	24		PS2 Device clock high
	;|	25		PS2 Device data high
	;\---------------------------------------------------------------------------------------/

	_PS2_DEVICE_HOST_TO_DEVICE:
		push	AccH
		pushawt
		;//////////////////////////////////
		;// CHECK IF PS2 LINE IN HOST REQUEST
		;//////////////////////////////////
		rcall	_PS2_DEVICE_GET_LINE_STATE				;get PS2 line status
		cpi	Acc,_PS2_DEVICE_STATE_HOST_REQUEST			;host REQUEST ?
		sec								;assume with error
		brne	_PS2_DEVICE_HOST_TO_DEVICE_EXIT				;yes, exit
		rcall	_PS2_DEVICE_CLOCK_TIME					;wait 1/4 time
		;//////////////////////////////////
		;// GET DATA BITS FORM HOST
		;//////////////////////////////////
		ldi	Acc,0							;clear data input
		ldi	AccH,0							;clear parity count
		ldi	AccT,8							;bit count
	_PS2_DEVICE_HOST_TO_DEVICE_10:
		clc
		rcall	_PS2_DEVICE_SET_CLOCK					;clock lo
		rcall	_PS2_DEVICE_GET_DATA					;get data
		ror	Acc							;put data into Acc
		sbrc	Acc,7							;compute parity bit
		inc	AccH
		sec
		rcall	_PS2_DEVICE_SET_CLOCK					;clock hi
		dec	AccT
		brne	_PS2_DEVICE_HOST_TO_DEVICE_10				;until all bit read
		;//////////////////////////////////
		;// GET PARITY
		;//////////////////////////////////
		clc
		rcall	_PS2_DEVICE_SET_CLOCK					;clock lo
		rcall	_PS2_DEVICE_GET_DATA					;get parity
		rol	AccTH							;AccH=0 if parity ok
		eor	AccTH,AccH
		com	AccTH
		andi	AccTH,1
		mov	AccH,AccTH
		sec
		rcall	_PS2_DEVICE_SET_CLOCK					;clock hi
		;//////////////////////////////////
		;// GET STOP
		;//////////////////////////////////
		clc
		rcall	_PS2_DEVICE_SET_CLOCK
		sec								;clock lo
		rcall	_PS2_DEVICE_SET_CLOCK					;clock hi
		;//////////////////////////////////
		;// ANSWER ACK
		;//////////////////////////////////
		clc
		rcall	_PS2_DEVICE_SET_DATA					;data=lo
		clc
		rcall	_PS2_DEVICE_SET_CLOCK					;clock=lo
		sec
		rcall	_PS2_DEVICE_SET_CLOCK					;clock=hi
		sec
		rcall	_PS2_DEVICE_SET_DATA					;data=hi
		lsr	AccH							;set carry state if parity status
	_PS2_DEVICE_HOST_TO_DEVICE_EXIT:
		popawt
		pop	AccH
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : PS2_DEVICE					|
	;>--------------------------------------------------------------<
	;| FUNCTION   : PS2_DEVICE_PROCESS				|
	;>--------------------------------------------------------------<
	;| DESCRIPTION: Process Reception and Transmission of data	|
	;|		between PS2 DEVICE and Host			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	Interrupts are enabled after save context 	|
	;|		registers					|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|
	;| This process is called at 4khz and each time process a communication event as follow
	;|
	;| +----------------------+----------------------+
	;| | HOST TO PS2 KEYBOARD | PS2 KEYBOARD TO HOST |
	;| +----------------------+----------------------+
	;| <--------- P ---------->
	;|
	;| P~250us, P may be long a ~1ms if detected (HOST TO PS2) OR (PS2 TO HOST) communication
	;|
	;| 																	|
	_PS2_DEVICE_PROCESS:
		push	Acc							;save Acc
		in	Acc,SREG						;save SREG
		pushaw								;save useds
		pushawt
		pushw	X
		pushw	Y
		pushw	Z
		;//////////////////////////////
		;// ENABLE GLOBAL INTERRUPTS
		;// RELOAD TIMER0
		;// STOP TIMER0 COUNT
		;//////////////////////////////
		ldi	Acc,-_PS2_DEVICE_PROCESS_INTERRUPT_TIMER0_DIV		;reload division factor to obtain correcty interrupt
		out	TCNT0,Acc
		ldi	Acc,_TIMER_STOP
		out	TCCR0,Acc
		sei								;enable global interrupts
		;//////////////////////////////
		;// PUMPT EVENT COUNT
		;//////////////////////////////
		lds	Acc,_PS2_DEVICE_EVENT_COUNT				;get event count
		inc	Acc							;EVENT++
		andi	Acc,1							;EVENT in 0..3
		sts	_PS2_DEVICE_EVENT_COUNT,Acc
		cpi	Acc,_PS2_DEVICE_PROCESS_EVENT_KEYBOARD_HOST_TO_PS2	;EVENT=KEYBOARD HOST TO PS2 COMUNICATION ?
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_HOST_TO_PS2		;yes, branch
		cpi	Acc,_PS2_DEVICE_PROCESS_EVENT_KEYBOARD_PS2_TO_HOST	;EVENT=KEYBOARD PS2 TO HOST COMUNICATION ?
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_PS2_TO_HOST		;yes, branch
		;*************************************************
		;**						**
		;** PROCESS KEYBOARD HOST TO PS2 COMMUNICATIONS	**
		;**						**
;*****************************************************************************************************************************
	_PS2_DEVICE_PROCESS_KEYBOARD_HOST_TO_PS2:
		rcall	_PS2_DEVICE_GET_LINE_STATE				;get PS2 line status
		cpi	Acc,_PS2_DEVICE_STATE_HOST_REQUEST			;host REQUEST ?
		lbrne	_PS2_DEVICE_PROCESS_EXIT				;no, exit 
		rcall	_PS2_DEVICE_HOST_TO_DEVICE				;get data from HOST
		sts	_PS2_DEVICE_KEYBOARD_DATA_IN,Acc			;set data in
		brcc	_PS2_DEVICE_PROCESS_KEYBOARD_HOST_TO_PS2_10		;branch if no error
		ldi	Acc,_PS2_DEVICE_KEYBOARD_CMDOUT_RESEND			;put RESEND cmdout
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE
		rjmp	_PS2_DEVICE_PROCESS_EXIT				;exit
	_PS2_DEVICE_PROCESS_KEYBOARD_HOST_TO_PS2_10:
		;//////////////////////////////////////
		;//
		;// RESTORE PC STATUS AND PROCESS RECEIVED COMMAND/DATA
		;//
		;//////////////////////////////////////
		ldiw	Z,_PS2_DEVICE_KEYBOARD_PC_STATE				;Z-->PC Status
		rcall	_PS2_DEVICE_RESTORE_PC_STATE				;restore status
		
		ldi	Acc,(1<<_PS2_DEVICE_KEYBOARD_FLAG_SCAN_STATE_BIT)	;disable scan
		rcall	_PS2_DEVICE_KEYBOARD_RESET_FLAGS

		lds	Acc,_PS2_DEVICE_KEYBOARD_DATA_IN			;get data in

		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_STATE_IND
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_STATE_IND
		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_ECHO
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_ECHO
		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_ALT_SCAN
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_ALT_SCAN
		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_TYPE_RATE_DELAY
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_TYPE_RATE_DELAY
		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_ENABLE
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_ENABLE
		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_DISABLE
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_DISABLE
		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_SET_DEFAULT
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_SET_DEFAULT
		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_ALL_MAKE_TYPE
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_ALL_MAKE_TYPE
		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_ALL_MAKE_BREAK
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_ALL_MAKE_BREAK
		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_ALL_MAKE
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_ALL_MAKE
		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_ALL_MAKE_TYPE_BREAK
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_ALL_MAKE_TYPE_BREAK
		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_KEY_MAKE_TYPE
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_KEY_MAKE_TYPE
		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_KEY_MAKE_BREAK
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_KEY_MAKE_BREAK
		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_KEY_MAKE
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_KEY_MAKE
		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_RESEND
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_RESEND
		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_RESET
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_RESET
		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_READ_ID
		lbreq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_READ_ID
		;//////////////////////////////////////
		;//
		;// IF NONE OF COMMANDS ABOVE ISSUE RESEND COMMAND
		;//
		;//////////////////////////////////////
		ldiw	Z, _PS2_DEVICE_KEYBOARD_PC_STATE			;clear PC STATUS
		rcall	_PS2_DEVICE_CLEAR_PC_STATE
		ldi	Acc,_PS2_DEVICE_KEYBOARD_CMDOUT_RESEND			;put RESEND cmdout
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE
		ldi	Acc,(1<<_PS2_DEVICE_KEYBOARD_FLAG_SCAN_STATE_BIT)	;enable scan
		rcall	_PS2_DEVICE_KEYBOARD_SET_FLAGS
		rjmp	_PS2_DEVICE_PROCESS_EXIT
		;########################################
		;##  	SET/RESET STATUS INDICATOR	#
		;########################################---------------------------
		;|Three status indicators on the keyboard-Num Lock,
		;|Caps Lock, and Scroll Lock-are accessible by the host.
		;|The PS2 DEVICE activates or deactivates these indicators
		;|when it receives a valid command-code sequence
		;|from the system. The command sequence begins with
		;|the command byte (hex ED). The PS2 DEVICE responds
		;|to the command byte with ACK, discontinues scanning,
		;|assignments for this option byte are as follow:
		;|
		;|Bit 	Indicator
		;|0 	Scroll Lock Indicator
		;|1 	Num Lock Indicator
		;|2 	Caps Lock Indicator
		;|3~7 	Reserved (Must be 0)
		;|If a bit for an indicator is set to 1, the indicator is turned
		;|on. If a bit is set to 0, the indicator is turned off.
		;+------------------------------------------------------------------
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_STATE_IND:
		ldi	Acc,_PS2_DEVICE_KEYBOARD_CMDOUT_ACK			;answer ACK
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE
		ldiw	Z,_PS2_DEVICE_KEYBOARD_PC_STATE
		ldiw	X,_PS2_DEVICE_PROCESS_EXIT				;save PC STATUS
		rcall	_PS2_DEVICE_SAVE_PC_STATE
		lds	Acc,_PS2_DEVICE_KEYBOARD_DATA_IN			;get status
		sts	_PS2_DEVICE_KEYBOARD_STATE_INDICATOR,Acc		;set
		rjmp	_PS2_DEVICE_PROCESS_KEYBOARD_ANSWER_ACK_AND_CLEAR_STATE	;answer ACK and clear PC state
		;########################################
		;##  	    	   ECHO			#
		;########################################---------------------------
		;|Echo is a diagnostic aid. When the PS2 DEVICE receives
		;|this command, it issues a hex EE response and if
		;|the PS2 DEVICE was previously enabled, continues
		;|scanning.
		;+------------------------------------------------------------------
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_ECHO:
		ldi	Acc,_PS2_DEVICE_KEYBOARD_CMDOUT_ECHO			;answer ECHO
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE
		ldi	Acc,(1<<_PS2_DEVICE_KEYBOARD_FLAG_SCAN_STATE_BIT)	;enable scan
		rcall	_PS2_DEVICE_KEYBOARD_SET_FLAGS
		rjmp	_PS2_DEVICE_PROCESS_EXIT
		;########################################
		;##  	    ALTERNATE SCAN		#
		;########################################---------------------------
		;|This command instructs the PS2 DEVICE select one of
		;|the three sets of scan codes. The PS2 DEVICE acknowledges
		;|receipt of this command with ACK, clears both
		;|the output buffer and the typematic key. The host then
		;|sends the option byte and the keyboard responds with
		;|another ACK. An option byte value of hex 01 selects
		;|scan code set 1, hex 02 selects set 2.
		;|An option byte value of hex 00 causes the PS2 DEVICE
		;|to acknowledge with ACK and sends a byte telling the
		;|host which scan code set is currently in use.
		;|After establishing the new scan code set, the
		;|PS2 DEVICE returns to the scanning state it was in before
		;|receiving the Select Alternate Scan Codes command
		;+------------------------------------------------------------------
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_ALT_SCAN:
		ldi	Acc,_PS2_DEVICE_KEYBOARD_CMDOUT_ACK			;answer ACK
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE
		ldiw	Z,_PS2_DEVICE_KEYBOARD_PC_STATE
		ldiw	X,_PS2_DEVICE_PROCESS_EXIT				;save PC STATUS
		rcall	_PS2_DEVICE_SAVE_PC_STATE
		lds	Acc,_PS2_DEVICE_KEYBOARD_DATA_IN			;get scan number
		cpi	Acc,0							;requesting actual scan code type ?
		breq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_ALT_SCAN_10		;yes, branch
		sts	_PS2_DEVICE_KEYBOARD_SCAN_CODE_TYPE,Acc			;set
		rjmp	_PS2_DEVICE_PROCESS_KEYBOARD_ANSWER_ACK_AND_CLEAR_STATE	;answer ACK and clear PC state
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_ALT_SCAN_10:
		;/////////////////////////////////////////////
		;// case parameter in alternate scan is zero
		;/////////////////////////////////////////////
		ldi	Acc,_PS2_DEVICE_KEYBOARD_CMDOUT_ACK			;answer ACK
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE

		ldiw	Z,_PS2_DEVICE_KEYBOARD_PC_STATE
		ldiw	X,_PS2_DEVICE_PROCESS_EXIT				;save PC STATUS
		rcall	_PS2_DEVICE_SAVE_PC_STATE

		lds	Acc,_PS2_DEVICE_KEYBOARD_SCAN_CODE_TYPE			;get actual scan type
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE			;send out

		ldiw	Z,_PS2_DEVICE_KEYBOARD_PC_STATE
		ldiw	X,_PS2_DEVICE_PROCESS_EXIT				;save PC STATUS
		rcall	_PS2_DEVICE_SAVE_PC_STATE

		ldiw	Z,_PS2_DEVICE_KEYBOARD_PC_STATE				;reset PC STATUS
		rcall	_PS2_DEVICE_CLEAR_PC_STATE
		ldi	Acc,(1<<_PS2_DEVICE_KEYBOARD_FLAG_SCAN_STATE_BIT)	;enable scan
		rcall	_PS2_DEVICE_KEYBOARD_SET_FLAGS
		rjmp	_PS2_DEVICE_PROCESS_EXIT
		;########################################
		;##  	   SET TYPE RATE DELAY		#
		;########################################
		;|The host issues the Set Typematic Rate/Delay command
		;|to change the typematic rate and delay. The
		;|PS2 DEVICE responds to the command with ACK, stops
		;|scanning and waits for the system to issue the rate/delay
		;|value byte. The PS2 DEVICE responds to the value
		;|byte with another ACK, sets the rate and delay to the
		;|value indicated, and continues scanning (if it was previously
		;|enabled). Bits 6 and 5 indicate the delay, and bits
		;|4, 3, 2, 1 and 0 indicate the rate. Bit7 is always 0.
		;|
		;|delay values
		;|
		;| bit6 bit5	Time
		;|  0	 0	250ms
		;|  0	 1	500ms
		;|  1	 0	750ms
		;|  1    1	1000ms
		;|
		;|The period (interval from one typematic output to the
		;|next) is determined by the following equation:
		;|Period = (8+A)*(2^B)*0.00417 seconds.
		;|
		;|Where:
		;|
		;|A = binary value of bits 2, 1, and 0.
		;|B = binary value of bits 4, and 3.
		;|
		;|The typematic rate is 1 for each period and are listed as
		;|follows:
		;|Bit	Typematic Rate 	Bit	Typematic Rate
		;|
		;|00000 	30.0	10000 		7.5
		;|00001 	26.7 	10001 		6.7
		;|00010 	24.0 	10010 		6.0
		;|00011 	21.8	10011 		5.5
		;|00100 	20.0 	10100 		5.0
		;|00101 	18.5 	10101 		4.6
		;|00110 	17.1 	10110 		4.3
		;|00111 	16.0 	10111 		4.0
		;|01000 	15.0 	11000 		3.7
		;|01001 	13.3 	11001 		3.3
		;|01010 	12.0 	11010 		3.0
		;|01011 	10.9 	11011 		2.7
		;|01100 	10.0 	11100 		2.5
		;|01101 	 9.2 	11101 		2.3
		;|01110 	 8.0 	11110 		2.1
		;|01111 	 8.0 	11111 		2.0
		;|
		;|The default values for the PS2 DEVICE are as follows:
		;|Typematic rate= 10.9 characters per second
		;|Delay = 500ms
		;|The execution of this command stops without changes
		;|to the existing rate if another command is received instead
		;|of the rate/delay value byte.
		;+------------------------------------------------------------------
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_TYPE_RATE_DELAY:
		ldi	Acc,_PS2_DEVICE_KEYBOARD_CMDOUT_ACK			;answer ACK
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE
		ldiw	Z,_PS2_DEVICE_KEYBOARD_PC_STATE
		ldiw	X,_PS2_DEVICE_PROCESS_EXIT				;save PC STATUS
		rcall	_PS2_DEVICE_SAVE_PC_STATE
		lds	Acc,_PS2_DEVICE_KEYBOARD_DATA_IN			;get rate delay
		sts	_PS2_DEVICE_KEYBOARD_TYPE_RATE_DELAY,Acc		;set
		rjmp	_PS2_DEVICE_PROCESS_KEYBOARD_ANSWER_ACK_AND_CLEAR_STATE	;answer ACK and clear PC state
		;########################################
		;##  	    	ENABLE			#
		;########################################---------------------------
		;|Upon receipt of this command, the PS2 DEVICE will respond
		;|with ACK, clears its output buffer, clears the last
		;|typematic key, and starts scanning.
		;+------------------------------------------------------------------
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_ENABLE:
		ldi	Acc,(1<<_PS2_DEVICE_KEYBOARD_FLAG_SCAN_STATE_BIT)	;enable scan
		rcall	_PS2_DEVICE_KEYBOARD_SET_FLAGS
		ldi	Acc,_PS2_DEVICE_KEYBOARD_CMDOUT_ACK			;answer ACK
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE
		rjmp	_PS2_DEVICE_PROCESS_EXIT
		;########################################
		;##  	    	DISABLE			#
		;########################################---------------------------
		;|The Default Disable command resets all conditions to
		;|the power on default states. The PS2 DEVICE will
		;|respond with ACK, clears its output buffer, sets the default
		;|typematic rate/delay, and clears the last typematic
		;|key. The PS2 DEVICE then stops scanning and awaits
		;|further command.
		;+------------------------------------------------------------------
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_DISABLE:
		ldi	Acc,(1<<_PS2_DEVICE_KEYBOARD_FLAG_SCAN_STATE_BIT)	;disable scan
		rcall	_PS2_DEVICE_KEYBOARD_RESET_FLAGS
		ldi	Acc,_PS2_DEVICE_KEYBOARD_CMDOUT_ACK			;answer ACK
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE
		rjmp	_PS2_DEVICE_PROCESS_EXIT
		;########################################
		;##  	    	SET DEFAULT		#
		;########################################
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_SET_DEFAULT:
;		rjmp	_PS2_DEVICE_PROCESS_EXIT
		;########################################
		;##  	    SET ALL MAKE TYPE		#
		;########################################
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_ALL_MAKE_TYPE:
;		rjmp	_PS2_DEVICE_PROCESS_EXIT
		;########################################
		;##  	    SET ALL MAKE BREAK		#
		;########################################
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_ALL_MAKE_BREAK:
;		rjmp	_PS2_DEVICE_PROCESS_EXIT
		;########################################
		;##  	    SET ALL MAKE		#
		;########################################
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_ALL_MAKE:
;		rjmp	_PS2_DEVICE_PROCESS_EXIT
		;########################################
		;##  	    SET ALL MAKE TYPE BREAK	#
		;########################################
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_ALL_MAKE_TYPE_BREAK:
		ldi	Acc,_PS2_DEVICE_KEYBOARD_CMDOUT_ACK			;answer ACK
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE
		ldi	Acc,(1<<_PS2_DEVICE_KEYBOARD_FLAG_SCAN_STATE_BIT)	;enable scan
		rcall	_PS2_DEVICE_KEYBOARD_SET_FLAGS
		rjmp	_PS2_DEVICE_PROCESS_EXIT
		;########################################
		;##  	    SET KEY MAKE TYPE		#
		;########################################
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_KEY_MAKE_TYPE:
;		rjmp	_PS2_DEVICE_PROCESS_EXIT
		;########################################
		;##  	    SET KEY MAKE BREAK		#
		;########################################
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_KEY_MAKE_BREAK:
;		rjmp	_PS2_DEVICE_PROCESS_EXIT
		;########################################
		;##  	       SET KEY MAKE		#
		;########################################
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_KEY_MAKE:	
		ldi	Acc,_PS2_DEVICE_KEYBOARD_CMDOUT_ACK			;answer ACK
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE

		ldiw	Z,_PS2_DEVICE_KEYBOARD_PC_STATE
		ldiw	X,_PS2_DEVICE_PROCESS_EXIT				;save PC STATUS
		rcall	_PS2_DEVICE_SAVE_PC_STATE

		lds	Acc,_PS2_DEVICE_KEYBOARD_DATA_IN			;get rate delay
		sts	_PS2_DEVICE_KEYBOARD_KEY3,Acc				;set
		;/////////////////////////////////////////
		;// AND ACK TO HOST AND CLEAR PC STATE
		;/////////////////////////////////////////
	_PS2_DEVICE_PROCESS_KEYBOARD_ANSWER_ACK_AND_CLEAR_STATE:
		ldi	Acc,_PS2_DEVICE_KEYBOARD_CMDOUT_ACK			;answer ACK
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE

		ldiw	Z,_PS2_DEVICE_KEYBOARD_PC_STATE				;reset PC STATUS
		rcall	_PS2_DEVICE_CLEAR_PC_STATE
		ldi	Acc,(1<<_PS2_DEVICE_KEYBOARD_FLAG_SCAN_STATE_BIT)	;enable scan
		rcall	_PS2_DEVICE_KEYBOARD_SET_FLAGS
		rjmp	_PS2_DEVICE_PROCESS_EXIT
		;########################################
		;##  	    	RESEND			#
		;########################################---------------------------
		;|The host sends this command when it detects an error in
		;|any transmission from the PS2 DEVICE. It is sent only after
		;|a data transmission and before the host allows the
		;|next data output. When a Resend is received, the
		;|PS2 DEVICE sends the previous output again (unless
		;|the previous output was Resent, in which case the
		;|PS2 DEVICE sends the last byte before the Resend
		;|command).
		;+------------------------------------------------------------------
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_RESEND:
		rcall	_PS2_DEVICE_KEYBOARD_DEC_G				;point to last data
		rcall	_PS2_DEVICE_KEYBOARD_GET_DATA_QUEUE			;get data
		cpi	Acc,_PS2_DEVICE_KEYBOARD_CMDIN_RESEND			;last data is resend ?
		breq	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_RESEND_10		;yes, branch
		rcall	_PS2_DEVICE_KEYBOARD_DEC_G				;else point to last data
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_RESEND_10:
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE
		ldi	Acc,(1<<_PS2_DEVICE_KEYBOARD_FLAG_SCAN_STATE_BIT)	;enable scan
		rcall	_PS2_DEVICE_KEYBOARD_SET_FLAGS
		rjmp	_PS2_DEVICE_PROCESS_EXIT
		;########################################
		;##  	         RESET			#
		;########################################---------------------------
		;|PS2 DEVICE wait 500ms for interface test disable 
		;|CLOCK data DATA lines and answer ACK+BAT COMPLETE after
		;|delay
		;+------------------------------------------------------------------
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_RESET:
		;////////////////////////////////////////
		;// INIT SETTINGS
		;////////////////////////////////////////
		rcall	_PS2_DEVICE_KEYBOARD_SET_VARIABLES			;set default variables
		rcall	_PS2_DEVICE_KEYBOARD_SET_DEFAULT			;set default parameters
		ldi	Acc,(1<<_PS2_DEVICE_KEYBOARD_FLAG_RESET_BIT)		;FLAG.RESET<-1
		rcall	_PS2_DEVICE_KEYBOARD_SET_FLAGS
		;////////////////////////////////////////
		;// WAIT 500ms AND ANSWER BAT COMPLETE OK
		;////////////////////////////////////////
		;_M_DELAY_MILLISECONDS	500					;wait 500ms
		ldi	Acc,_PS2_DEVICE_KEYBOARD_CMDOUT_ACK			;answer ACK
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE
		ldi	Acc,_PS2_DEVICE_KEYBOARD_CMDOUT_BAT_COMPLETE		;answer BAT COMPLETE
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE
		rjmp	_PS2_DEVICE_PROCESS_EXIT
		;########################################
		;##  	    	READ ID			#
		;########################################---------------------------
		;|This command requests identification information from
		;|the PS2 DEVICE. The PS2 DEVICE responds with ACK,
		;|discontinues scanning and sends the two keyboard ID
		;|bytes hex 83h and ABh. After the output of the second
		;|ID byte, the PS2 DEVICE resumes scanning.
		;+------------------------------------------------------------------
	_PS2_DEVICE_PROCESS_KEYBOARD_CMD_READ_ID:
		ldi	Acc,_PS2_DEVICE_KEYBOARD_CMDOUT_ACK			;answer ACK
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE
		ldi	Acc,_PS2_DEVICE_KEYBOARD_CMDOUT_ID_1			;answer AB,83
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE
		ldi	Acc,_PS2_DEVICE_KEYBOARD_CMDOUT_ID_2
		rcall	_PS2_DEVICE_KEYBOARD_PUT_DATA_QUEUE
		ldi	Acc,(1<<_PS2_DEVICE_KEYBOARD_FLAG_SCAN_STATE_BIT)	;enable scan
		rcall	_PS2_DEVICE_KEYBOARD_SET_FLAGS
		rjmp	_PS2_DEVICE_PROCESS_EXIT
		;*************************************************
		;**						**
		;** PROCESS KEYBOARD PS2 TO HOST COMMUNICATIONS	**
		;**						**
		;<<< WRONG ERROR IN BELOW CODE WHEN LINE IS REQUESTING STATE FROM HOST >>>
		;<<< 8/4/2009 21:19
;*****************************************************************************************************************************
	_PS2_DEVICE_PROCESS_KEYBOARD_PS2_TO_HOST:
		rcall	_PS2_DEVICE_KEYBOARD_GET_DATA_QUEUE			;get data from queue
		brcs	_PS2_DEVICE_PROCESS_KEYBOARD_PS2_TO_HOST_10		;branch if no data 
		rcall	_PS2_DEVICE_DEVICE_TO_HOST				;else send data to host
		brcc	_PS2_DEVICE_PROCESS_KEYBOARD_PS2_TO_HOST_10		;branch if data transmitted ok
		rcall	_PS2_DEVICE_KEYBOARD_DEC_G
	_PS2_DEVICE_PROCESS_KEYBOARD_PS2_TO_HOST_10:
		rjmp	_PS2_DEVICE_PROCESS_EXIT
		;//////////////////////////////
		;// ENABLE TIMER0 COUNT
		;//////////////////////////////
	_PS2_DEVICE_PROCESS_EXIT:
		ldi	Acc,_TIMER_DIV_64
		out	TCCR0,Acc

		popw	Z
		popw	Y
		popw	X
		popawt								;restore useds
		popaw
		out	SREG,Acc						;restore SREG
		pop	Acc							;restore Acc
		ret

#endif
