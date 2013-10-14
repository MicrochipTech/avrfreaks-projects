;/////////REGISTER ALLOCATION////////
	.def	romreg			= r0	;Used by LPM and MUL command
;	.def					= r1	;Used by MUL command
	.def	bytecntr		= r2	;used to count bytes (TWI too!)
	.def	currentcast		= r3	;Used in parsing text.
	.def	currlooptgt		= r4	;FOR-NEXT
	.def	currloopvar		= r5	;FOR-NEXT
	.def	loopreturnL		= r6	;FOR-NEXT
	.def	loopreturnH		= r7	;FOR-NEXT
;	.def					= r8	;
	.def	RTCcntr			= r9	;hold RTC interval countdown
	.def	LOOPStkPtr		= r10	;FOR_NEXT loop stack pointer
	.def	GoSubStkPtr		= r11	;GOSUB stack pointer
	.def	DStkPtr			= r12	;Data stack pointer.
	.def	ecode			= r13	;Cast returned here and error code stored here
	.def	accl			= r14	;DDS phase accumulator low-byte
	.def	accm			= r15	;DDS phase accumulator mid-byte
	.def	temp 			= r16	;General purpose variable
	.def	inchar 			= r17	;Char destined to go out the uart
;	.def			 		= r18	;Lower byte for asci-hex conversion
	.def	cmdparms		= r19	;relative number of parameters supplied
	.def	outchar			= r20	;Char coming in from the uart
	.def	U				= r21	;Units 
	.def	T				= r22	;Tens
	.def	H				= r23	;Hundreds
	.def	inbytel 		= r24	;registers used to pass int16_t variables
	.def	inbyteh 		= r25	;  to/from USB library routines

	;XL (r26) as flag register bit useage.
	.equ	VARSTK			= 0		;1 for variables to leave pointers on stack, =0 for values on stack
	.equ	VPTRSTK			= 1		;1 to indicate there is a variable pointer on the dstack
	.equ	SKPNXT			= 2		;1 to skip next line during RUN execution
	.equ	GSJAMR			= 3		;1 to tell runcommand to jamb GOSUB return address
	.equ	LPJAMN			= 4		;1 to tell runcommand to jam in loop return address next time
	.equ	LPCAPA			= 5		;1 to tell runcommand to capture loop return address
	.equ	GSCAPA			= 6		;1 to tell runcommand to capture GOSUB return address
	.equ	GSJAMD			= 7		;1 to tell runcommand to jamb destination address

	;XH (r27) as flag register bit useage.
	.equ	HALT			= 0		;1 to tell runcommand to halt
	.equ	SUSP			= 1		;1 to indicate that suspend had been requested
	.equ	RUNACT			= 2		;1 to indicate that the RUN command is executing
	.equ	GSACT			= 3		;1 to indicate that GOSUB is active
	.equ	FNACT			= 4		;1 to indicate that FOR-NEXT is active
	.equ	FNLEQ			= 5		;1 to switch = to be part of FOR-NEXT loop
	.equ	NOEXEC			= 6		;1 to indicate not to execute the keyword 
	.equ	PRIACT			= 7		;1 to indicated a PRINT command is active
;	.equ	FNLNXT			= 3		;1 to activate Next as part of a loop (not used)

	.def	acch	=r30			;phase accumulator high-byte
;	.def	ZH		=r31			;Z-Reg High Byte

