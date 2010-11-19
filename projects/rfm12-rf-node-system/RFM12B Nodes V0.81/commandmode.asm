


; no return address on stack when entering command_mode
; so can just jump back to main when exiting

; this is the command DESTINATION node ie the remote node that is receiving
; commands from a source node
; The only ways out of this mode are:
; 1- Timeout (set to 30-sec) which sends us back to the main loop
; 2- A reboot command from the source node to this remote node

command_mode:
	call	begin_RX		; start listening for packets

wait_commandpacket:
	jeq	state,state1,command_mode	; if err, restart RX
	jeq	state,state6,gotpack	; If RX complete, exit RX loop and process
	tst	seconds			; get timeout timer
	brne	wait_commandpacket	; if not expired, loop again

; timer expired so stop command mode, stop RX and exit back to main loop
	stopRF				; idle mode
	call	stop_connect
	jmp	main			; back to main loop


; a structuraly correct packet has been received, so figure out if we want it
gotpack:
	lds	temp1,RX_header_code	; get packet type
	jeq	temp1,cmdpack,cmdpk	; if a command packet then continue	

; anything else is discarded, go back to waiting for a command packet
	rjmp	command_mode


; so it is a command packet, so copy the received text to the local linebuffer
; before invoking the local command processor
cmdpk:	call	rxbuf_to_linebuf	; copy the asciiZ string to the linebuf

; now change the output stream to memory so thet the command processor response
; gets sent back to the source node
	stream	memory_stream			; set o/p stream

; set memory 'print address' to TX data area just after the source/dest address bytes
	ldx	ram,TX_data_buffer+2		; address to 'print' to ram
	storeX	Ppointer			; save print pointer
	ldi	temp1,2				; for 1st 2 bytes (source/dest)
	sts	Pcount,temp1			; set count start value

; now we are set up for the command response to 'print' to the TX buffer, call
; the local command processor, which acts on the linebuf as if it was
; entered localy

	call	local		; local processing of the string from remote node

; at the exit from 'local' the command processor response has been 'printed' to
; the 'memory stream'. Dont leave the o/p stream as ram.

	stream	serial_stream			; set o/p stream back to serial

; now fill in the source-dest addresses in the TX buffer
	lds	temp1,config_node_address	; THIS nodes address
	sts	TX_sourcenode,temp1		; is source address

	lds	temp1,RX_sourcenode		; where the string came from
	sts	TX_destnode,temp1		; so where to send back to

	ldi	headerbyte,cmdpack+0x80		; to identify a command response

; get the data count of the local command-processor string response
	lds	datacount,Pcount	; save the count for TX'ing
	ldx	ram,TX_base		; start of ram block for TX function

	call	prepare_tx		; build packet in memory then send
; now the local command processor response is being transmitted back to the source node

	call	wait_TXend		; loops till finished sending TX buffer
	rjmp	command_mode			; back to top of command loop








; this is where we end up to deal with the response from a remote node while in
; command mode. In this case, its to just send it to the output stream which
; at this point is the serial port.
; first check if in command-mode, ie connected

command_response:
	lds	temp1,outgoing_connect	; get the current state
	cpi	temp1,true		; can we accept a response from a remote node?
	breq	rmok			; if Y, jmp
; if falls through, wrong mode so abandon and return to main loop
	ret

; now we need to get the returned data from the RX buffer and send it to our
; local output stream
rmok:	lds	loopcounter,RX_datasize	; this is never zero
	dec	loopcounter
	dec	loopcounter		; compensate for the addresses
	ldX	ram,rx_data_buffer+2	; skip the addresses

	stream	serial_stream			; set o/p stream to serial

tostrm:	ld	temp1,x+
	call	stream_out
	dec	loopcounter
	brne	tostrm
	ret




send_linebuffer:
	call	linebuf_to_txbuf	; copy the just entered local text to TX buffer

	mov	temp1,destaddr		; where we are connected to
	sts	TX_destnode,temp1	; set as destination

	lds	temp1,config_node_address
	sts	TX_sourcenode,temp1	; and local addr is source

	ldi	headerbyte,cmdpack

	ldX	ram,TX_base		; point at base of TX block
	call	prepare_TX		; send linebuffer

	call	wait_TXend		; wait till TX complete
	stopRF
	call	begin_RX		; start listening for packets
	ret





