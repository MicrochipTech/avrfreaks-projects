

.dseg
command_index:		.byte	1	; which item in the table was found


.cseg
; =============================================================================
; list of possible commands
cmd_base:
	string	'?'		; prints command list
	string	"help"		; prints command list
	string	"reboot"	; soft reboot
	string	"channel"	; sets RF channel
	string	"connect"	; connect in command mode to another node
	string	"save"		; not done yet
	string	"group"		; sets group code
	string	"id"		; sets node address
	string	"default"	; reset to default values (address/group/channel/datarate)
	string	"rate"		; sets over the air dta rate

	.db	0xFF		; table terminator

; =============================================================================
; to get this far, a line has been entered and at least 1 parameter
; exists, so a search for parameter 1 should (if its in the table of course..)
; produce a result.
docom:	lds	temp1,paramcount	;
	tst	temp1
	brne	issome			; 

; empty line or no parameter. Perhaps just an empty line entered.
	newline				; cr/lf to o/p stream
donode:	rcall	shownodeID		; displays RFM12B info (channel, data-rate etc...)
	newline
	newline
	rjmp	command_exit

; 1st item in the linebuf is Parameter 1
; 2nd could be a value for configuration purposes
issome:	ldi	temp1,1			; should always be at least 1
	rcall	get_param		; sets pointer to item 1
	rcall	find_command		; gets command index
	brcs	yup			; CS=found it in table

; item not in table so show what is not found end exit
	newline
	send	'"'
	rcall	show_param
	send	'"'
	stringout " Not found!"
	newline

command_exit:
	ret			; rets to wherever 'check_for_input" was being
				; called from

; so the entered item has been found in the table
yup:	lds	temp1,command_index	; get table index

	jeq	temp1,0,help		; shows command list
	jeq	temp1,1,help		; shows command list
	jeq	temp1,2,reboot		; stops ints and jumps to zero
	jeq	temp1,3,channel		; set RF channel. 1..19 @0.5Mhz intervals
	jeq	temp1,4,connect		; connects to another node by RF in command mode
	jeq	temp1,5,save		; nothing yet!
	jeq	temp1,6,group_		; sets the nodes group address
	jeq	temp1,7,ID		; sets the nodes address
	jeq	temp1,8,default		; sets the node back to default settings
	jeq	temp1,9,rate		; sets the over-the-air data rate

; should never fall through to here
zxc:	stringout	".index error!"
	newline
	rjmp	command_exit


; +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
help:
	newline
	newline
	stringout	"command list is...."
	newline
	send	' '
	send	' '

;	rjmp	donode

	ldZ	rom,cmd_base

gtab:	lpm	temp1,z+
	tst	temp1
	breq	dozed		; jmp if Z
	rcall	stream_out
	rjmp	gtab

dozed:	lpm	temp1,z+
	tst	temp1
	breq	dozed
	cpi	temp1,0xff	; table end
	breq	last
	newline
	push	temp1
	send	' '
	send	' '
	pop	temp1
	rcall	stream_out
	rjmp	gtab

last:	newline
	newline

;	rjmp	command_exit

	rjmp	donode

; +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
reboot:
	newline
	stringout	"....soft reboot...  hit 'enter' to continue"
	newline
	cli
	nop
	nop
	jmp	0



; +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

channel:
	newline
	ldi	temp1,2			; see if a value passed
	rcall	get_param		; sets pointer to item 2
	brcs	chan5			; somthing exists
	rjmp	chan1			; no number error
; Ploc now points at item in linebuf
chan5:	rcall	read_number		; try and extract the number
	brcs	chan4
	rjmp	chan2

; we have a valid number so try and use it to set the channel
chan4:	rcall	set_channel		; in rfm12b.asm
	brcc	chan3			; bad or unable to set channel
	
	ldX	ram,node_channel	; where to store in eeprom
	storeX	EEadr			; set pointer
	rcall	EEPROM_write		; update node address byte
	rcall	update_CRC		; update eeprom CRC
	rcall	set_config		; make it active
	stringout	"Channel updated"	;
	newline

	rjmp	last

chan3:	stringout	"Unable to change channel"
	newline
	stringout	"Invalid channel number!"
	newline
	rjmp	last

chan2:	stringout	"bad number format"
	rjmp	last
chan1:	stringout	"no value!"
	rjmp	last
; +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

connect:
	newline
	call	connect_othernode
	brcs	gotcon
	stringout	"Failed to connect to node "
	rjmp	sha

gotcon:
	stringout	"connected to node "
sha:	mov	temp1,destaddr
	call	hexout

	ldi	temp1,connect_timeout
	mov	seconds,temp1	; prepare timeout for connect

	call	begin_RX
	newline

	ser	temp1
	sts	outgoing_connect,temp1	; outgoing connection now active
	rjmp	last

; +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

save:
	stringout	"no save yet"
	rjmp	last

; +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

group_:
	newline
	ldi	temp1,2			; see if a value passed (2nd item)
	rcall	get_param		; sets pointer to item 2
	brcs	grp1			; if somthing exists, jmp
	rjmp	chan1			; go to message

; Ploc now points at 2nd item in linebuf
grp1:	rcall	read_number		; try and get the number
	brcs	grp2			; if good number, jmp
	rjmp	chan2			; bad number message

; number is in temp1
grp2:	ldX	ram,node_group
	storeX	EEadr			; set pointer
	rcall	EEPROM_write		; update node address byte
	rcall	update_CRC		; update eeprom CRC
	rcall	set_config		; make it active

	rjmp	last

; +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ID:	newline
	ldi	temp1,2			; see if a value passed (2nd item)
	rcall	get_param		; sets pointer to item 2
	brcs	ID1			; if somthing exists, jmp
	rjmp	chan1			; go to message

; Ploc now points at 2nd item in linebuf
ID1:	rcall	read_number		; try and get the number
	brcs	ID2			; if good number, jmp
	rjmp	chan2			; bad number message

; number is in temp1
ID2:	ldX	ram,node_address
	storeX	EEadr			; set pointer
	rcall	EEPROM_write		; update node address byte
	rcall	update_CRC		; update eeprom CRC
	rcall	set_config		; make it active

	rjmp	last

; +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

default:
;	stringout	"re-initialise and reset to new??.."
;	newline
;	stringout	"are you sure??..  y/n.. "
;	call	flush_rx
;	call	getabyte
;	jeq	temp1,'y',do_init	; clear and init eeprom
;	jeq	temp1,'n',last
;	newline
;	rjmp	default

do_init:
	stringout	"Creating default config!"
	newline
	call	default_config
	call	test_config
	brcc	recon_f
	rjmp	reboot

recon_f:
	newline
	stringout	"default config write failure!"
	newline
	rjmp	last


; +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
rate:
	newline
	ldi	temp1,2			; see if a value passed
	rcall	get_param		; sets pointer to item 2
	brcs	rate5			; somthing exists
	rjmp	rate1			; no number error
; Ploc now points at item in linebuf
rate5:	rcall	read_number		; try and get the number
	brcs	rate4
	rjmp	rate2

; we have a valid number so try and use it to set the channel
rate4:	rcall	set_datarate
	brcc	rate3			; bad or unable to set channel
	
	ldX	ram,node_rate		; where to store in eeprom
	storeX	EEadr			; set pointer
	rcall	EEPROM_write		; update node address byte
	rcall	update_CRC		; update eeprom CRC
	rcall	set_config		; make it active
	stringout	"data rate updated"	;
	newline
	rjmp	last

rate3:	stringout	"Unable to change data rate!"
	newline
	stringout	"Invalid rate!"
	newline
	rjmp	last

rate2:	stringout	"bad number format"
	rjmp	last
rate1:	stringout	"no value!"
	rjmp	last






; *****************************************************************************
; DESCR: compares a Parameter to table entries
; ENTRY: Parameter at Ploc
; EXIT: C set & index in temp2 (1..X). NC=not found
; CHANGES:
; USES:
; *****************************************************************************
find_command:
	push	temp1
	push	proc_count
	push	zl
	push	zh
	push	xl
	push	xh

	ldz	rom,cmd_base	; rom pointer
	clr	proc_count	; index count

; this is the outer loop as it works through the table entries
C0:	loadX	Ploc		; ram pointer

; this is the inner loop as it scans through each entry
C1:	lpm	temp1,z+	; get rom char
	ld	temp2,x+	; ram char
	cp	temp1,temp2	; are they the same?
	brne	diff1		; jump if diff
	tst	temp1		; are they zero (terminator)
	breq	found
; not zeros so keep looking through this entry
	rjmp	C1

; get till z
diff1:	tst	temp1		; check char from rom for zero
	breq	N1		; if a zero, fwd
	lpm	temp1,z+	; not a zero so get next
	rjmp	diff1		; and check it

; get till nz
N1:	lpm	temp1,z+
	tst	temp1
	breq	N1
; see if end of table
	cpi	temp1,0xff
	breq	notfound

; if here, we are past the zero terminator and also possibly padding byte
; and have got the 1st byte of the next entry so back-off the pointer by one.
	sbiw	zh:zl,1
	inc	proc_count	; increment the index counter
	rjmp	C0		; now go back and compare the next entry

notfound:
	clc
	rjmp	comex

found:	sts	command_index,proc_count
	sec
comex:
	pop	xh
	pop	xl
	pop	zh
	pop	zl
	pop	proc_count
	pop	temp1
	ret



; *****************************************************************************
; DESCR: outputs assorted info about the RF configuration
; ENTRY: 
; EXIT: 
; CHANGES: 
; USES: 
; *****************************************************************************
shownodeID:

	stringout	"ID="
	lds	temp1,config_node_address
	call	decout


	stringout	", Group="
	lds	temp1,config_node_group
	call	decout

	stringout	", Channel="
	lds	temp1,config_node_channel
	call	decout
	stringout	" ("
	ldZ	rom,chantext
	lds	temp1,config_node_channel
	dec	temp1
	call	show_text
	stringout	"), "

	stringout	" Rate="
	lds	temp1,config_node_rate
	call	decout
	stringout	" ("
	lds	temp1,config_node_rate
	ldZ	rom,ratetext
	call	show_text
	send	')'

	ret






