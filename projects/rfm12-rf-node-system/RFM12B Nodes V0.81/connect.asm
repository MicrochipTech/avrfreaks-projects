


; the remote-node-connect functionality is pretty iffy. It was an afterthought and crudely
; implimented.
; At the start, the basic command interpreter was just to help with diagnostics and enable easy configuration
; of the 'local' node. I did not have the hardware around to connect both nodes serially at the same time so
; I just hacked it to enable me to do it over the link. It turned out to be useful and I can cope with the
; quirks so I have lived with it so far. Needs a rewrite and to be implimented differently
; When using a terminal to config a 'local' node, the characters are echoed back to you. This does not
; happen when configuring a remote node so you are typing blind (or you can enable local-echo in the terminal)
; but then that gives you double chars with the local one. I suppose disabling the local echo is the simple
; thing to do but I think I would rather do the rewrite sometime.

connect_othernode:
	stopRF				; set to idle mode

	ldi	temp1,2			; see if a value passed (2nd item)
	rcall	get_param		; sets pointer to item 2
	brcs	getnodenum		; if somthing exists, jmp
	rjmp	gn1			; malformed number

; Ploc now points at 2nd item in linebuf
getnodenum:
	rcall	read_number		; try and get the number
	brcs	gn2			; if good number, jmp
	rjmp	gn3			; bad number message

; now we have an address in temp1 to try and connect to
gn2:
	sts	TX_destnode,temp1	; fill in destination node address
	mov	destaddr,temp1		; save copy of the dest addr

	lds	temp1,config_node_address ; local address
	sts	TX_sourcenode,temp1	; fill in local addr
	ldi	datacount,2		; only sending source & dest addr
	ldi	headerbyte,connectpack	; set header byte (connection request)
	ldX	ram,TX_base		; point at base of TX block

	call	prepare_TX		; builds TX packet and starts TX
; transmission is now in progress. 

gn3:	jne	state,state11,gn3	; wait till TX finished
	stopRF				; idle mode
	rcall	wait_for_ack		; CS=good, CC=bad
	ret

gn1:	clc
	ret



; *****************************************************************************
; DESCR: waits for 20ms for an ACK from the node we have just sent to
; ENTRY: packet has just been sent
; EXIT: CS=got relevant ACK. CC=no relevant ACK received or error during RX
; CHANGES: 
; USES: 
; *****************************************************************************
wait_for_ack:
	call	begin_RX				; now enable receive

; set up 20ms loop to check for incoming ACK packet
;------------------------------------------------------------------------------
	msloop_for 20				; wait for 20ms
top:		jne	state,state1,noe	;
		call	begin_RX		; if an error, restart RX seeking
noe:		cpi	state,state6		; if complete then breakout early
		breq	RXgood
	msloop_to top
;------------------------------------------------------------------------------

; if falls through then timed-out. Make sure RX all off
RXer1:
	stopRF				; idle mode, int off
	clc
	ret

; received a 'structuraly correct' packet, but have to test if it is the right packet
RXgood:		; a valid RX packet has allready set RF to idle

	rcall	test_ack
	ret

; *****************************************************************************
; DESCR: tests RX buffer for a valid ACK from the expected source to THIS NODE
; ENTRY: a valid packet has been received
; EXIT: CS=correct ack. CC=not the expected packet (the packet IS 'structuraly correct'
; its just not what we are waiting for
; CHANGES: 
; USES: 
; *****************************************************************************
test_ack:
;	error	20			; error code info for diagnostics
	lds	temp1,RX_header_code	; read the received header byte
	cpi	temp1,ackpack		; should be an ACK
	brne	ack_fail		; if not, exit

;	error	21
	lds	temp1,RX_datasize	; data size is only 2 bytes
	cpi	temp1,2			; only source & dest address
	brne	ack_fail		; if not, exit

;	error	22
	lds	temp1,RX_sourcenode	; this is where data came FROM
	lds	temp2,TX_destnode	; so thats where we sent it TO
	cp	temp1,temp2		; these should be the same
	brne	ack_fail		; if not, failure

;	error	23
	lds	temp1,RX_destnode	; has to be addressed to THIS node
	lds	temp2,config_node_address ; get local address
	cp	temp1,temp2		; do they match?
	brne	ack_fail
; if falls through then it is a valid ACK packet FROM the correct node (where
; we just sent the data TO)
;	error	0			; dont leave it with an errorcode
	sec
	ret

; ack failed in some way. See the errorcode if using it
ack_fail:
	clc
	ret


; to gete here, the received packet must be structuraly correct ie  
; fifo fill chars, data count and of course CRC.
; Check if the packet is addressed to HERE and if so, extract the header byte
; and use it to decide next step.
; It could be a packet that has no dest address ie a beacon, time,
; *****************************************************************************
; DESCR: acts on a received packet
; ENTRY: packet in RX buffer. CALLED from main
; EXIT: 
; CHANGES: 
; USES: 
; *****************************************************************************
process_packet:
	lds	temp1,RX_header_code		; get the header code (packet type)
	jeq	temp1,beaconpack,add_nodetable	; an 'i'm here' packet
	jeq	temp1,timepack,set_localtime	; a time sync packet
	jeq	temp1,datapack,justdata		; no source/dest

; everything else should have a source/dest addr
	lds	temp1,RX_destnode		; address of target node
	lds	temp2,config_node_address	; our local address
	cp	temp1,temp2			; test for match
	brne	cleanexit			; if not for HERE then exit

; so we are the destination for this packet. Jump to relevant action/processing
	lds	temp1,RX_header_code			; get the header code (packet type)
	jeq	temp1,connectpack,prepare_connect	; if a connect request, send an ACK	
	jeq	temp1,temppack,display_temp		; jump to sensor processing
	jeq	temp1,cmdpack+0x80,command_response	; A remote command response

cleanexit:
	ret

; no actions defined yet for these
add_nodetable:
set_localtime:
justdata:
	rjmp	cleanexit			; to re-enable rx


; *****************************************************************************
; DESCR: prepare for incoming connection
; ENTRY: connect packet arrived
; EXIT: ready for command data
; CHANGES: 
; USES: 
; *****************************************************************************
prepare_connect:
	rcall	sendack		; let source know that its target exists so connect can continue
	ser	temp1
	sts	Incoming_connect,temp1	; set to true
	ldi	temp1,connect_timeout
	mov	seconds,temp1	; prepare a timeout if no connect data

	pop	temp1		; remove return address from stack
	pop	temp1		; which is in 'main' loop
	jmp	command_mode	; so exit from command mode is just a jump to main

; *****************************************************************************
; DESCR: stops connect/command mode
; ENTRY: 
; EXIT: 
; CHANGES: 
; USES: 
; *****************************************************************************
stop_connect:
	stream	serial_stream			; back to default serial
	clr	seconds				; clear downcount
	sts	incoming_connect,seconds	; set to false
	ret






; *****************************************************************************
; DESCR: send an ACK packet to where we just received from
; ENTRY: RX packet in buffer contains addr of source node (to return ACK to)
; EXIT: ack completely sent
; CHANGES: 
; USES: 
; *****************************************************************************
sendack:
	lds	temp1,RX_sourcenode	; where to send the ack (who just sent to us)
	sts	TX_destnode,temp1	; fill in destination node address

	lds	temp1,config_node_address ; local address
	sts	TX_sourcenode,temp1	; fill in local addr

	ldi	datacount,2		; only sending source & dest addr
	ldi	headerbyte,ackpack	; set header byte (ack)
	ldX	ram,TX_base		; point at base of TX block
	call	prepare_TX		; builds TX packet and starts TX

; transmit now in progress. Wait till sent
sendpaws:
	jne	state,state11,sendpaws	; wait till TX finished
	ret





; *****************************************************************************
; DESCR: copies the line of text entered in the linebuf to the TX buffer &
; loads the datacount for the TX functionality
; ENTRY: data in linebuf & charcount valid, X points at TX buffer data area
; EXIT: data copied and datacount updated
; CHANGES: TX buffer
; USES: -
; *****************************************************************************
linebuf_to_txbuf:
	lds	loopcounter,charcount	; get count of linebuf data
	push	loopcounter		; save count
	ldZ	ram,linebuf
	ldX	ram,TX_data_buffer+2

fer:	ld	temp1,z+
	st	x+,temp1
	dec	loopcounter
	brne	fer			; loop till copied

	sbiw	xh:xl,1			; point back at the last char which
	ldi	temp1,cr		; should be a CR but was replaced by a
	st	x,temp1			; terminating zero
	pop	datacount		; retrieve count
	inc	datacount
	inc	datacount		; add 2 for source & dest addresses
; the linebuffer string is now in the TX buffer and the datacount is updated
	clr	temp1
	sts	charcount,temp1		; reset linebuf count
	ret


; *****************************************************************************
; DESCR: copies the line of text (asciiZ)in the RX buffer to the linebuffer
; loads
; ENTRY:
; EXIT: data copied and charcount updated
; CHANGES: linebuffer
; USES: -
; *****************************************************************************
rxbuf_to_linebuf:
	lds	loopcounter,RX_datasize	; get received datacount (includes source/dest addr)
	dec	loopcounter		; adjust to just extract string length
	dec	loopcounter
	sts	charcount,loopcounter	; save count
	ldZ	ram,RX_data_buffer+2	; skip the source-dest addresses
	ldX	ram,linebuf			; point at linebuffer

Rfer:	ld	temp1,z+
	st	x+,temp1
	dec	loopcounter
	brne	Rfer			; loop till copied

	sbiw	xh:xl,1			; point back at the last char which
	clr	temp1			; should be a CR but was replaced by a
	st	x,temp1			; terminating zero

; the RX'ed asciiZ string is now in the linebuffer
	ret






; sends the received data to the serial port
display_temp:
	lds	loopcounter,RX_datasize
	cpi	loopcounter,0
	breq	nodat
	dec	loopcounter
	dec	loopcounter


; extract the source node address so it can be shown
	ldx	ram,RX_data_buffer	; also source addr

	send	'S'			; 'S' for soutce addr
	ld	temp1,x+
	call	hexout			; show the address
	send	' '
	ld	temp1,x+		; discard

showloop:
	ld	temp1,x+
	call	stream_out
	dec	loopcounter
	brne	showloop
	send	' '
	call	showtime
	newline
indicate:
; pulse a LED on receiver each time
;	sbi	portd,3		; light led
;	msdelay	200		; wait
;	cbi	portd,3		; led off
	ret


; left-over from early diag. Shows local CRC
	send	' '
nodat:	mov	temp1,crc_high
	call	hexout
	mov	temp1,crc_low
	call	hexout
	newline
	ret





