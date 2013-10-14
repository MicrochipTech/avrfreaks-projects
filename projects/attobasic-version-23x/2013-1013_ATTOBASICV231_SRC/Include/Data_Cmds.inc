;//////////BEGIN COMMANDS//////////

;Note about command routines: Clear XL bit 0 to have variables leave their value
;on the stack. This flag is set again at the start of each line.

;Format for command list:
	;1. A .db line with three characters which are the first 
	;three letters of the command plus a fourth to fill out the word.
	;Commands less than three bytes need to be righ-justfied, with 
	;blank leading bytes having the value $50. Compare values are 
	;anded with $5F.
	;2. A .dw line that is the label for the routine to be executed.

commandlist:
	.db	"LIS "
commandlist1:
	.dw	listcommand
commandlist2:

.if HELP
	.db	"HELP"
	.dw	HELPcommand
.endif
	
	.db	"RUN "
	.dw	runcommand
	
	.db	"PRI "		;PRINT
	.dw	printcommand

	.db	"PRX "		;PRX -Print in hexidecimal
	.dw	printhexcommand

	.db	"PRB "		;PRB -Print in binary
	.dw	prbcommand

	.db	"EMIT"		;EMIT -send byte on stack as ascii to terminal. 
	.dw	emitcommand	

	.db	"FOR "
	.dw	forcommand
	
	.db	$50,"TO "		; TO
	.dw	tocommand

	.db	"NEXT"
	.dw	nextcommand
	
	.db	"FRE "			;FREE bytes or ram remaining
	.dw	FREEcommand
	
	.db	"NEW "			;NEW	clear progam space, start over
	.dw	newprogram
		
	.db	$50,"IF "		; IF
	.dw	ifcommand
	
	.db	"THE "			;THEN
	.dw	thencommand
		
	.db	"END "
	.dw	endcommand
	
.if RENUMBR
	.db	"RENU"		; RENUM - renumbers the current program in memory
	.dw	RENUmcommand
.endif

	.db	"DATA"		; DATA - saves DATA for later program use
	.dw	DATAcommand
	
	.db	"READ"		; READ - read next data byte from DATA statement
	.dw	READcommand
	
	.db	"REST"		; RESTore - resetes the pointer to the DATA array
	.dw	RESTcommand
	
	.db	$50,(':' & $5F),('=' & $5F),$20	; :=
	.dw	setequalscommand
	
	.db	$50,$50,('>' & $5F),$20	; > greater-than operator
;	.dw	greaterthancommand
	.dw	diffcommand
	
	.db	$50,$50,('<' & $5F),$20	; < less-than operator
;	.dw	lessthancommand
	.dw	diffcommand
	
	.db	$50,('<'& $5F),('>'& $5F),$20	; "<>" - not equal to
	.dw	notequalcommand

;	.db	$50,$1C,$1E,$20			; <>
	.db	$50,('!'& $5F),('='& $5F),$20	; "!=" - not equal to
	.dw	notequalcommand

	.db	$50,$50,('=' & $5F),$20	;  =	equal sign. Evaluate expression
	.dw	evaluatecommand
	
	.db	$50,$50,('+' & $5F),$20	; +
	.dw	addcommand

	.db	$50,$50,('-' & $5F),$20	; -
	.dw	subtractcommand

	.db	$50,$50,('*' & $5F),$20	; *
	.dw	multiplycommand

	.db	$50,$50,('/' & $5F),$20	; /
	.dw	DIVIDEcommand

	.db	"MOD "					; modulus
	.dw	MODULUScommand

	.db	$50,$50,('%' & $5F),$20	; % modulus
	.dw	MODULUScommand

	.db	$50,$50,('$' & $5F),$20	; $ Hex command - interpret following as hexidecimal.
	.dw	HEXcommand

	.db	$50,$50,(''' & $5F),$20	; $ binary command - interpret following as 8-bit binary
	.dw	BINcommand

	.db	$50,$50,('"' & $5F),$20	; $ String command - print following as a string 
	.dw	STRINGcommand

.if	( INTREG && (DHT || RTC || DATA) )
	.db	$50,$50,('@' & $5F),$20	;next is a constant
	.dw	FETCHcommand
.endif

	.db	"LSR "
	.dw	lsrcommand
	
	.db	"LSL "
	.dw	lslcommand
	
	.db	$50,$50,('&' & $5F),$20	; & - alternate Logical AND
	.dw	andcommand
	
	.db	"AND "			;AND - Logical AND
	.dw	andcommand

	.db	$50,$50,('|' & $5F),$20	; | - alternate Logical OR
	.dw	orcommand

	.db	$50,"OR "		;OR -Logical OR
	.dw	orcommand
	
	.db	$50,$50,('^' & $5F),$20	; ^ - alternate Logical XOR
	.dw	XORcommand
	
	.db	"XOR "			;XOR - Logical XOR
	.dw	XORcommand

	.db	"COM "		;Logical COMpliment (1's compliment)
	.dw	comcommand

	.db	$50,$50,('!' & $5F),$20	; ~ - alternate Logical NEGate (2's compliment
	.dw	negcommand

	.db	"NEG "		;NEG - Logical NEGate (2's compliment
	.dw	negcommand

	.db	"SWAP"		;SWAP - SWAP the high and low nibbles
	.dw	SWAPcommand

	.db	"NBH "		;NBH - Return the high nibble
	.dw	NBHcommand

	.db	"NBL "		;NBL - Return the low nibble
	.dw	NBLcommand

	.db	"AOV "		;AOV - enable/disable arithmetic over/underflow
	.dw	aovcommand	; detection.  Enabled by default.
	
	.db "RND "		;generate 8-bit random # (w/ optional value passed)
	.dw	RNDcommand

	.db "CRC "		;generate 8-bit CRC from command parameters
	.dw	CRCcommand

.if	CRC16R
	.db "CRW "		;generate 16-bit CRC from command parameters
	.dw	CRWcommand
.endif

	.db "OSC "		;Set/read AVR's oscillator calibration byte
	.dw	OSCcommand

	.db	"DELA"		;DELAY
	.dw	delaycommand

	.db	"GOT "
	.dw	gotocommand
	
	.db	"GOS "		;GOSUB
	.dw	gosubcommand
			
	.db	"RET "		;RETURN
	.dw	returncommand
	
	.db	"KEY "		;WAIT -pause execution until char received from terminal. Halt if control-C received
	.dw	keycommand
		
	.db	"ACO "		;ACO -Put analog comparator output on the stack
	.dw	acocommand

	.db	"ACI "		;ACI -enable or disable the analog comparator's interrupt
	.dw	acicommand

.if DATAREC	
	.db	"ACS "		;ACS -select ch 0 or ch 1 to the analog comparator's "-" interrupt
	.dw	acscommand

	.db	"ACR "		;ACR - set 8 bit pwm channel as analog comparator reference (same as "PWM8")
	.dw	ACRcommand
.endif
		
	.db	"SAVE"		;SAVE	-Save contents of variable space and program memory to EEPROM
	.dw	savecommand

	.db	"LOAD"		;LOAD	-Load variable space and program memory from EEPROM
	.dw	loadcommand
			
	.db	"PEEK"		;PEEK -return data pointed to by parameter
	.dw	peekcommand
	
	.db	"POKE"		;POKE - "POKE data, address"
	.dw	pokecommand
		
	.db	"EER "		;EER -read a byte from EEP
	.dw	EERcommand
	
	.db	"EEW "		;EEW -write a byte to EEP
	.dw	EEWcommand
		
.if	( INTREG && (DHT || RTC || DATA) )
	.db	"VPG "		;VPG - return the page in RAM internal variable are stored in
	.dw	VPGcommand
.endif
		
	.db	"RST "		;RST - causes a system reset
	.dw	RSTcommand

.if EFS		;file system support
	.db	"REM "		;REM - REMark used to identify a program
	.dw	REMcommand

	.db	"INIT"		;INIT - Initialize file system
	.dw	INITcommand

	.db	"CAT "		;CAT - CATalog a list of saved programs
	.dw	CATcommand

	.db	"ERA "		;ERA - erase a saved program
	.dw	ERAcommand
.endif

.if LPMD
	.db	"SLP "		;SLP - Enter Sleep Mode
	.dw	slpcommand
.endif

.if DATA			;Data file using Serial EEPROM
	.db	"DFW "		;DFW - write data to random address
	.dw	DFWcommand

	.db	"DFR "		;DFR - read data from random address
	.dw	DFRcommand

	.db	"DFL "		;DFL - log data to the address held in the internal
	.dw	DFLcommand	;  address counter

	.db	"DFX "		;DFX - reset the internal address counter to "0"
	.dw	DFXcommand

	.db	"DFF "		;DFF - set data dump format to "n" bytes 
	.dw	DFFcommand

	.db	"DFD "		;DFD - Dump sequential data from address
	.dw	DFDcommand

	.db	"DFV "		;DFV - set the initialization value for the DFI command
	.dw	DFVcommand

	.db	"DFI "		;DFI - Initialize Data File
	.dw	DFIcommand

  .if INTREG		;only if internal variables
	.db	"DFA "		;DFA - Returns the low-byte of the address in RAM of 
	.dw	DFAcommand	; the logging command
  .endif
.endif
;
.if (NRF && RTC)	;NRF and RTC commands
	.db	"RFI "		;RFI - initialize the nRF24L01
	.dw	RFIcommand

	.db	"RFA "		;RFA - set the nRF24L01 addresses
	.dw	RFAcommand

	.db	"RFC "		;RFP - set the nRF24L01's TX/RX channel
	.dw	RFCcommand

	.db	"RFP "		;RFP - set the nRF24L01's TX power level
	.dw	RFPcommand

	.db	"RFB "		;RFB - set the nRF24L01's RX buffer sizes
	.dw	RFBcommand

	.db	"RFE "		;RFE - enable/disable the nRF24L01's TX PA
	.dw	RFEcommand

	.db	"RFT "		;RFT - transmit data
	.dw	RFTcommand

	.db	"RFX "		;RFX - transfer the nRF24L01's RX buffer to 
	.dw	RFXcommand	; the DATA staement buffer

	.db	"RFF "		;RFF - flush the nRF24L01's TX/RX buffers
	.dw	RFFcommand

	.db	"RFD "		;RFD - set or return the nRF24L01's data rate
	.dw	RFDcommand

	.db	"RFW "		;RFW - write a value to a register in the nRF24L01
	.dw	RFWcommand

	.db	"RFR "		;RFR - return the value of a nRF24L01 register or STATUS
	.dw	RFRcommand
.endif
;
.if (DHT && RTC)	;Acquire temperature and humidity from DHTxx device
	.db	"DHT "		;DHT - read DHTxx temperature
	.dw	DHTcommand

	.db	"DHH "		;DHH - read DHTxx humidity
	.dw	DHHcommand

	.db	"DHR "		;DHR - return sensor status
	.dw	DHRcommand

	.db	"DHU "		;DHU - set units flag (*C/*F)
	.dw	DHUcommand

	.db	"DHS "		;DHS - set ignore sign flag
	.dw	DHScommand

  .if INTREG		;only if internal variables
	.db	"DHI "		;DHI - returns the value of the low byte of RH_I@RAM
	.dw	DHIcommand

	.db	"DHD "		;DHD - returns the value of the low byte of RH_D@RAM
	.dw	DHDcommand

	.db	"DTI "		;DTI - returns the value of the low byte of RT_I@RAM
	.dw	DTIcommand

	.db	"DTD "		;DTD - returns the value of the low byte of RT_D@RAM
	.dw	DTDcommand
  .endif
.endif

.if RTC
	.db	"RTI "		;RTI - Set real-time counter interval
	.dw	RTIcommand

	.db	"RTR "		;RTR - Resets the real-time counter to zero
	.dw	RTRcommand

	.db	"RTP "		;RTP - Print real-time counter value or assign to variable
	.dw	RTPcommand

  .if INTREG		;only if internal variables
	.db	"RTC "		;RTC[3:0] - returns the value of the BYTE[n] of RT3@RAMRTCReg
	.dw	RTCcommand
  .endif

.endif

.if OWR	;1-wire interface
;	.db	"OWA "		;OWA - One-Wire Addess
;	.dw	OWAcommand

	.db	"OWI "		;OWI - One-Wire Initialize bus
	.dw	OWIcommand

	.db	"OWW "		;OWW - One-Wire Write data
	.dw	OWWcommand

	.db	"OWR "		;OWA - One-Wire Read data
	.dw	OWRcommand
.endif

.ifdef PORTA
	;direction register
	.db	"ODA "		;ODA -Out Data Direction A
	.dw	outdXcommand

	.db	"IDA "		;IDA -Input direction direction A 
	.dw	inDirectionX

	.db	"SDA "		;SDA -Set direction bit in A 
	.dw	setdirectionX
			
	.db	"CDA "		;CDA -Clear direction bit in A
	.dw	cleardirectionX

	;output
	.db	"OPA "		;OPA -Out Port A
	.dw	outpXcommand

	.db	"SBA "		;SBA -Set bit in A register
	.dw	setbitinX

	.db	"CBA "		;CBA -Clear bit in A register
	.dw	clearbitinX

	.db	"XBA "		;XBA -XOR bit in A register
	.dw	xorbitinX

	;input
	.db	"INA "		;INA -Input data from port A
	.dw	inXcommand

	.db	"IBA "		;IBA -Test bit in A
	.dw	testbitinX
.endif

.ifdef PORTB
	;direction register
	.db	"ODB "		;ODB -Out Data Direction A
	.dw	outdXcommand

	.db	"IDB "		;IDB -Input direction direction B
	.dw	inDirectionX

	.db	"SDB "		;SDB -Set direction bit in B 
	.dw	setdirectionX

	.db	"CDB "		;CDB -Clear direction bit in B
	.dw	cleardirectionX

	;output
	.db	"OPB "		;OPB -Out Port B
	.dw	outpXcommand

	.db	"SBB "		;SBD -Set bit in B register
	.dw	setbitinX

	.db	"CBB "		;CBD -Clear bit in B register
	.dw	clearbitinX

	.db	"XBB "		;XBB -XOR bit in B register
	.dw	xorbitinX

	;input
	.db	"INB "		;INB -Input data from port B
	.dw	inXcommand

	.db	"IBB "		;IBB -Test bit in B
	.dw	testbitinX
.endif

;*******************************************************
.ifdef PORTC
	;direction register
	.db	"ODC "		;ODC -Out Data Direction C
	.dw	outdXcommand

	.db	"IDC "		;IDC -Input direction direction C
	.dw	inDirectionX

	.db	"SDC "		;SDC -Set direction bit in C
	.dw	setdirectionX

	.db	"CDC "		;CDC -Clear direction bit in C
	.dw	cleardirectionX

	;output
	.db	"OPC "		;OPC -Out Port C
	.dw	outpXcommand

	.db	"SBC "		;SBD -Set bit in C register
	.dw	setbitinX

	.db	"CBC "		;CBD -Clear bit in C register
	.dw	clearbitinX

	.db	"XBC "		;XBC -XOR bit in C register
	.dw	xorbitinX

	;input
	.db	"INC "		;INC -Input data from port C
	.dw	inXcommand

	.db	"IBC "		;IBB -Test bit in C
	.dw	testbitinX
.endif

;*******************************************************
.ifdef PORTD
	;direction register
	.db	"ODD "		;ODD -Out Data Direction D
	.dw	outdXcommand

	.db	"IDD "		;IDD -Input direction direction D
	.dw	inDirectionX

	.db	"SDD "		;SDD -Set direction bit in D 
	.dw	setdirectionX

	.db	"CDD "		;CDD -Clear direction bit in D
	.dw	cleardirectionX

	;output	
	.db	"OPD "		;OPD -Out Port D
	.dw	outpXcommand

	.db	"SBD "		;SBD -Set bit in D register
	.dw	setbitinX

	.db	"CBD "		;CBD -Clear bit in D register
	.dw	clearbitinX

	.db	"XBD "		;XBD -XOR bit in D register
	.dw	xorbitinX

	;input
	.db	"IND "		;IND -Input data from port D
	.dw	inXcommand

	.db	"IBD "		;IBD -Test bit in D
	.dw	testbitinX
.endif

;**********************
.ifdef PORTE
	;direction register
	.db	"ODE "		;ODB -Out Data Direction E
	.dw	outdXcommand

	.db	"IDE "		;IDE -Input direction direction E
	.dw	inDirectionX

	.db	"SDE "		;SDE -Set direction bit in E
	.dw	setdirectionX

	.db	"CDE "		;CDE -Clear direction bit in E
	.dw	cleardirectionX

	;output
	.db	"OPE "		;OPE -Out Port E
	.dw	outpXcommand

	.db	"SBE "		;SBE -Set bit in E register
	.dw	setbitinX

	.db	"CBE "		;CBE -Clear bit in E register
	.dw	clearbitinX

	.db	"XBE "		;XBE -XOR bit in E register
	.dw	xorbitinX

	;input
	.db	"INE "		;INE -Input data from port E
	.dw	inXcommand

	.db	"IBE "		;IBE -Test bit in E
	.dw	testbitinX
.endif

;*******************************************************
.ifdef PORTF
	;direction register
	.db	"ODF "		;ODF -Out Data Direction F
	.dw	outdXcommand

	.db	"IDF "		;IDF -Input direction direction F
	.dw	inDirectionX

	.db	"SDF "		;SDF -Set direction bit in F
	.dw	setdirectionX

	.db	"CDF "		;CDF -Clear direction bit in F
	.dw	cleardirectionX

	;output
	.db	"OPF "		;OPF -Out Port F
	.dw	outpXcommand

	.db	"SBF "		;SBF -Set bit in F register
	.dw	setbitinX

	.db	"CBF "		;CBF -Clear bit in F register
	.dw	clearbitinX

	.db	"XBF "		;XBF -XOR bit in F register
	.dw	xorbitinX

	;input
	.db	"INF "		;INF -Input data from port F
	.dw	inXcommand

	.db	"IBF "		;IBF -Test bit in F
	.dw	testbitinX
.endif

;*******************************************************
.ifdef PORTG
	;direction register
	.db	"ODG "		;ODG -Out Data Direction G
	.dw	outdXcommand

	.db	"IDG "		;IDG -Input direction direction G
	.dw	inDirectionX

	.db	"SDG "		;SDG -Set direction bit in G
	.dw	setdirectionX

	.db	"CDG "		;CDG -Clear direction bit in G
	.dw	cleardirectionX

	;output
	.db	"OPG "		;OPG -Out Port G
	.dw	outpXcommand

	.db	"SBG "		;SBG -Set bit in G register
	.dw	setbitinX

	.db	"CBG "		;CBG -Clear bit in G register
	.dw	clearbitinX

	.db	"XBG "		;XBG -XOR bit in G register
	.dw	xorbitinX

	;input
	.db	"ING "		;ING -Input data from port G
	.dw	inXcommand

	.db	"IBG "		;IBG -Test bit in G
	.dw	testbitinX
.endif

;*******************************************************
.ifdef PORTH
	;direction register
	.db	"ODH "		;ODH -Out Data Direction H
	.dw	outdXcommand

	.db	"IDH "		;IDH -Input direction direction H
	.dw	inDirectionX

	.db	"SDH "		;SDH -Set direction bit in H
	.dw	setdirectionX

	.db	"CDH "		;CDH -Clear direction bit in H
	.dw	cleardirectionX

	;output
	.db	"OPH "		;OPH -Out Port H
	.dw	outpXcommand

	.db	"SBH "		;SBH -Set bit in H register
	.dw	setbitinX

	.db	"CBH "		;CBH -Clear bit in H register
	.dw	clearbitinX

	.db	"XBH "		;XBH -XOR bit in H register
	.dw	xorbitinX

	;input
	.db	"INH "		;INH -Input data from port H
	.dw	inXcommand

	.db	"IBH "		;IBH -Test bit in H
	.dw	testbitinX
.endif

;*******************************************************
.ifdef PORTJ
	;direction register
	.db	"ODJ "		;ODJ -Out Data Direction J
	.dw	outdXcommand

	.db	"IDJ "		;IDJ -Input direction direction J
	.dw	inDirectionX

	.db	"SDJ "		;SDJ -Set direction bit in J
	.dw	setdirectionX

	.db	"CDJ "		;CDJ -Clear direction bit in J
	.dw	cleardirectionX

	;output
	.db	"OPJ "		;OPJ -Out Port J
	.dw	outpXcommand

	.db	"SBJ "		;SBJ -Set bit in J register
	.dw	setbitinX

	.db	"CBJ "		;CBJ -Clear bit in J register
	.dw	clearbitinX

	.db	"XBJ "		;XBJ -XOR bit in J register
	.dw	xorbitinX

	;input
	.db	"INJ "		;INJ -Input data from port J
	.dw	inXcommand

	.db	"IBJ "		;IBJ -Test bit in J
	.dw	testbitinX
.endif

;*******************************************************
.ifdef PORTK
	;direction register
	.db	"ODK "		;ODK -Out Data Direction K
	.dw	outdXcommand

	.db	"IDK "		;IDK -Input direction direction K
	.dw	inDirectionX

	.db	"SDK "		;SDK -Set direction bit in K
	.dw	setdirectionX

	.db	"CDK "		;CDK -Clear direction bit in K
	.dw	cleardirectionX

	;output
	.db	"OPK "		;OPK -Out Port K
	.dw	outpXcommand

	.db	"SBK "		;SBK -Set bit in K register
	.dw	setbitinX

	.db	"CBK "		;CBK -Clear bit in K register
	.dw	clearbitinX

	.db	"XBK "		;XBK -XOR bit in K register
	.dw	xorbitinX

	;input
	.db	"INK "		;INK -Input data from port K
	.dw	inXcommand

	.db	"IBK "		;IBK -Test bit in K
	.dw	testbitinX
.endif


;*******************************************************
.ifdef PORTL
	;direction register
	.db	"ODL "		;ODL -Out Data Direction L
	.dw	outdXcommand

	.db	"IDL "		;IDL -Input direction direction L
	.dw	inDirectionX

	.db	"SDL "		;SDL -Set direction bit in L
	.dw	setdirectionX

	.db	"CDL "		;CDL -Clear direction bit in L
	.dw	cleardirectionX

	;output
	.db	"OPL "		;OPL -Out Port L
	.dw	outpXcommand

	.db	"SBL "		;SBL -Set bit in L register
	.dw	setbitinX

	.db	"CBL "		;CBL -Clear bit in L register
	.dw	clearbitinX

	.db	"XBL "		;XBL -XOR bit in L register
	.dw	xorbitinX

	;input
	.db	"INL "		;INL -Input data from port L
	.dw	inXcommand

	.db	"IBL "		;IBL -Test bit in L
	.dw	testbitinX
.endif

;*******************************************************
.if	DEBUG
	.db	"DUMP"
	.dw	dumppmembuffer

	.db	"EDUM"		;EDUM[P] - dump contents of EEP
	.dw	dumpemembuffer

	.db	"VDUM"		;VDUM[P] - dump contents of variables
	.dw	dumpvmembuffer

	.db	"IDUM"		;IDUM[P] - dump contents of I/O
	.dw	dumpio

	.db	"FILL"		;FILL - Fill contents of RAM
	.dw	FILLmembuffer
.endif
;*********************
.if	DSI
	.db	"DSRE"		;DSRECIVE - recieve by to TOS from DS Interface
	.dw	dsrcommand		
					
	.db	"DSCO"		;DSCOMMAND - Send TOS via DS Interface as command
	.dw	dsccommand		
					
	.db	"DSDE"		;DSDATA - Send TOS via DS Interface as data
	.dw	dsdcommand		
									
.endif
;************************
.ifdef TCNT1H	;enabled if 16-bit timer available
	.if ICP
	.db	"ICG "		;ICG - set input capture gate time to value [0..7]
	.dw	ICGcommand	; NOTE this command must be issued before issuing
					; a 'ICP' command

	.db	"ICE "		;ICG [x] - set input capture edge select to [0..1]
	.dw	ICEcommand	; where 0 = falling and 1 = rising edge.  
					; [x] is optional and defaults rising edge (1).

	.db	"ICP "		;ICP - start capture with 'ICG' gatetime and return
	.dw	ICPcommand	;  low-byte of ICP register, store high-byte in 'Z'
	.endif
.endif

;************************
.if PWM
	.db	"PWM "		;PWM  send value to 8 bit pwm channel, set DDRB,3 to 1
	.dw	PWMcommand

	.db	"PWE "		;PWE  send value to 10 bit pwm channel, set DDRB,3 to 1
	.dw	PWEcommand

	.db	"PWO "		;PWO   turn off pulse-width modulation. Do not change DDRB
	.dw	PWOcommand
.endif
;************************
.if DDS
	.db	"DDS "		;DDS - Enables DDS and sets frequency
	.dw	DDScommand
.endif
;************************
#if ( ( defined(__ATmega32U4__) || \
	defined(__AT90USB1286__) || \
	defined(__ATmega2560__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__) ) && (! DREC) ) 
	.db	"BLDR"		;BLDR - jump to bootloader
	.dw	BLDRcommand
#endif

;**********************
 .if DATAREC	
	.db	"DIG "		;DIG - Digital OUT for AVR Data Recorder
	.dw	DIGcommand

	.db	"LDD "		;LDD - Load default capture program for the AVR Data Recorder
	.dw	LDDcommand
.endif
;**********************
.if ADConv
	.db	"ADC "		;ADC -provide value from a to do converter
	.dw	ADCcommand

	.db	"ADR "		;ADR -select between external and internal ref
	.dw	ADRcommand

  .if DATAREC	
	.db	"ADS "		;ADS - select CH0 or CH1 to AD536
	.dw	ADScommand

	.db	"ADU "		;ADU - select TRMS or dBV from AD536
	.dw	ADUcommand

	.db	"ACR "		;ACR  send value to 8 bit pwm channel, same as "PWM8"
	.dw	ACRcommand

	.db	"ADG "		;ADG - select CH0 or CH1 gains
	.dw	ADGcommand
  .endif
.endif
;
;**********************
.ifdef SPDR
	.if SPI
; SPM [0..2] SPM must be called first to init the SPR
;	Init defaults to Master, Mode 0, F_CLK/16, MSB first
	.db	"SPM "		;SPR -set SPI mode [0..2]
	.dw	SPMcommand

; SPO [0..1] is optional  (see data sheet)
	.db	"SPO "		;SPR -set data order to U where U = [0..1]
	.dw	SPOcommand

; SPC [0..15] is optional (see data sheet)
	.db	"SPC "		;SPC -set the SPI clock rate
	.dw	SPCKcommand

; SPW [0..255] set the SS pin low and transmits U
;	Execute this command to write data to SPI
;	Note that user must set the SPI_SS on SPI_PORT high after last byte
	.db	"SPW "		;SPX -write a value to the SPI port
	.dw	SPWcommand

; SPR recieves a byte into U
;	Execute this command to recieve data from the SPI port
;	Note that user must set the SPI_SS on SPI_PORT high after last byte
	.db	"SPR "		;SPR -read a value from the SPI port
	.dw	SPRcommand

; SPS [0..1] is optional  (see data sheet)
	.db	"SPS "		;SPS -set SPI_SS pin to level to U where U = [0..1]
	.dw	SPScommand
	.endif
.endif

;**********************
.ifdef TWDR
	.if TWI
; TWI [0..2] TWI must be called first to init the TWI interface.  
;	Defaults to Master and TWI_CLK = 400K.  Where x =
;	  0 to disable TWI interface.  
;	  1 to enable TWI interface with 400K clock
;	  2 to enable TWI interface with 100K clock
;	  3 to enable TWI interface with  50K clock (really slow?)
	.db	"TWI "		;TWI -Init the TWI mode [0..2]
	.dw	TWIcommand

; TWS Set START condition
	.db	"TWS "
	.dw	TWScommand

; TWP set STOP condition
	.db	"TWP "
	.dw	TWPcommand

; TWA [x] - TWA sends the slave address to the bus.  
	.db	"TWA "
	.dw	TWAcommand

; TWW [0..255] 
;	Execute this command to write data "U" to TWI slave
	.db	"TWW "
	.dw	TWWcommand

; TWR receives a byte into U
;	Execute this command to recieve data from the TWI bus onto stack
	.db	"TWR "
	.dw	TWRcommand

; TWB receives the current bus status (lsr'ed by 3 bits) into U
	.db	"TWB "
	.dw	TWBcommand
	.endif
.endif
;
	.db	"WTF "
	.dw	WTFcommand
	.dw	$00,$00			;end of command list

