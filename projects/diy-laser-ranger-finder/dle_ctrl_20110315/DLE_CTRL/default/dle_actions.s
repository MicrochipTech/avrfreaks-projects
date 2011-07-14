	.file	"dle_actions.c"
__SREG__ = 0x3f
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__tmp_reg__ = 0
__zero_reg__ = 1
	.global __do_copy_data
	.global __do_clear_bss
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.text
.Ltext0:
.global	spi_set_byte
	.type	spi_set_byte, @function
spi_set_byte:
.LFB2:
.LM1:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.LVL0:
.LM2:
	cbi 46-0x20,3
.LM3:
	ldi r26,lo8(186)
	ldi r27,hi8(186)
	st X,r24
	ldi r30,lo8(184)
	ldi r31,hi8(184)
	ldi r25,lo8(17)
	st Z,r25
	ldi r24,lo8(19)
.LVL1:
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	ld r18,X
.LVL2:
.LM4:
	st X,r22
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	ld r24,X
.LVL3:
.LM5:
	sbi 46-0x20,3
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function spi_set_byte size 45 (44) */
.LFE2:
	.size	spi_set_byte, .-spi_set_byte
.global	spi_set_2bytes
	.type	spi_set_2bytes, @function
spi_set_2bytes:
.LFB3:
.LM6:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.LVL4:
.LM7:
	cbi 46-0x20,3
.LM8:
	ldi r26,lo8(186)
	ldi r27,hi8(186)
	st X,r24
	ldi r30,lo8(184)
	ldi r31,hi8(184)
	ldi r25,lo8(17)
	st Z,r25
	ldi r24,lo8(19)
.LVL5:
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	ld r18,X
.LVL6:
.LM9:
	st X,r22
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	ld r18,X
.LVL7:
.LM10:
	st X,r20
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	ld r24,X
.LVL8:
.LM11:
	sbi 46-0x20,3
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function spi_set_2bytes size 63 (62) */
.LFE3:
	.size	spi_set_2bytes, .-spi_set_2bytes
.global	spi_set_3bytes
	.type	spi_set_3bytes, @function
spi_set_3bytes:
.LFB4:
.LM12:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.LVL9:
.LM13:
	cbi 46-0x20,3
.LM14:
	ldi r26,lo8(186)
	ldi r27,hi8(186)
	st X,r24
	ldi r30,lo8(184)
	ldi r31,hi8(184)
	ldi r25,lo8(17)
	st Z,r25
	ldi r24,lo8(19)
.LVL10:
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	ld r19,X
.LVL11:
.LM15:
	st X,r22
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	ld r19,X
.LVL12:
.LM16:
	st X,r20
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	ld r19,X
.LVL13:
.LM17:
	st X,r18
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	st Z,r25
	st Z,r24
	ld r24,X
.LVL14:
.LM18:
	sbi 46-0x20,3
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function spi_set_3bytes size 81 (80) */
.LFE4:
	.size	spi_set_3bytes, .-spi_set_3bytes
.global	calc_sincos
	.type	calc_sincos, @function
calc_sincos:
.LFB5:
.LM19:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.LVL15:
	movw r30,r24
.LM20:
	lds r18,gus_acc
	lds r19,(gus_acc)+1
	subi r18,lo8(-(8))
	sbci r19,hi8(-(8))
	swap r19
	swap r18
	andi r18,0x0f
	eor r18,r19
	andi r19,0x0f
	eor r18,r19
	lds r24,gus_acc+4
	lds r25,(gus_acc+4)+1
.LVL16:
	adiw r24,8
	swap r25
	swap r24
	andi r24,0x0f
	eor r24,r25
	andi r25,0x0f
	eor r24,r25
	sub r18,r24
	sbc r19,r25
	std Z+5,r19
	std Z+4,r18
.LM21:
	lds r18,gus_acc+2
	lds r19,(gus_acc+2)+1
	subi r18,lo8(-(8))
	sbci r19,hi8(-(8))
	swap r19
	swap r18
	andi r18,0x0f
	eor r18,r19
	andi r19,0x0f
	eor r18,r19
	lds r24,gus_acc+6
	lds r25,(gus_acc+6)+1
	adiw r24,8
	swap r25
	swap r24
	andi r24,0x0f
	eor r24,r25
	andi r25,0x0f
	eor r24,r25
	sub r18,r24
	sbc r19,r25
	std Z+3,r19
	std Z+2,r18
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function calc_sincos size 56 (55) */
.LFE5:
	.size	calc_sincos, .-calc_sincos
.global	send_out_msg
	.type	send_out_msg, @function
send_out_msg:
.LFB6:
.LM22:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.LVL17:
	mov r18,r24
.LM23:
	tst r24
	breq .L14
.LM24:
	ldi r30,lo8(guca_out_msg_buff)
	ldi r31,hi8(guca_out_msg_buff)
	ldi r25,lo8(0)
.LVL18:
.L21:
.LM25:
	lds r24,192
	sbrs r24,5
	rjmp .L21
.LM26:
	ld r24,Z+
	sts 198,r24
.LM27:
	subi r25,lo8(-(1))
	cp r25,r18
	brne .L21
.LVL19:
.L14:
	ret
/* epilogue: frame size=0 */
/* epilogue: noreturn */
/* epilogue end (size=0) */
/* function send_out_msg size 17 (17) */
.LFE6:
	.size	send_out_msg, .-send_out_msg
.global	spi_read_curr
	.type	spi_read_curr, @function
spi_read_curr:
.LFB7:
.LM28:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.LM29:
	cbi 46-0x20,3
.LM30:
	ldi r24,lo8(-128)
	sts 186,r24
	ldi r25,lo8(17)
	sts 184,r25
	ldi r24,lo8(19)
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	lds r24,186
.LVL20:
	ldi r30,lo8(guca_spi_curr_state)
	ldi r31,hi8(guca_spi_curr_state)
.L24:
.LM31:
	sts 186,__zero_reg__
	ldi r25,lo8(17)
	sts 184,r25
	ldi r24,lo8(19)
.LVL21:
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	lds r24,186
.LVL22:
.LM32:
	st Z+,r24
.LM33:
	ldi r24,hi8(guca_spi_curr_state+14)
	cpi r30,lo8(guca_spi_curr_state+14)
	cpc r31,r24
.LVL23:
	brne .L24
.LM34:
	sbi 46-0x20,3
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function spi_read_curr size 87 (86) */
.LFE7:
	.size	spi_read_curr, .-spi_read_curr
.global	action_adc_mux
	.type	action_adc_mux, @function
action_adc_mux:
.LFB11:
.LM35:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.L31:
.LM36:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(2)
	brlo .L31
.LM37:
	lds r18,guca_in_msg_buff+2
.LVL24:
	andi r18,lo8(3)
.LM38:
	mov r24,r18
	ldi r25,lo8(0)
.LVL25:
	lsl r24
	rol r25
	lsl r24
	rol r25
.LVL26:
	sts guc_adc_mux_hdr,r24
.LM39:
	lds r24,124
.LVL27:
.LM40:
	andi r24,lo8(-32)
.LM41:
	or r24,r18
.LVL28:
.LM42:
	sts 124,r24
.LM43:
	ldi r25,lo8(20)
.LVL29:
	sts guca_out_msg_buff,r25
.LM44:
	sts guca_out_msg_buff+1,r24
.LM45:
	ldi r24,lo8(2)
.LVL30:
	sts guc_out_msg_wr_idx,r24
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function action_adc_mux size 30 (29) */
.LFE11:
	.size	action_adc_mux, .-action_adc_mux
.global	rise_bias_ctrl
	.type	rise_bias_ctrl, @function
rise_bias_ctrl:
.LFB15:
.LM46:
/* prologue: frame size=0 */
	push r16
	push r17
	push r28
	push r29
/* prologue end (size=4) */
.LVL31:
	movw r28,r24
	movw r16,r22
.LM47:
	cp r24,r22
	cpc r25,r23
	breq .L41
.LVL32:
.L42:
.LM48:
	adiw r28,1
.LM49:
	sbrs r29,0
	rjmp .L39
.LVL33:
.LM50:
	andi r29,hi8(-257)
.LM51:
	ori r29,hi8(-28672)
.L39:
.LM52:
	mov r22,r29
	clr r23
.LVL34:
	andi r22,lo8(-112)
	ldi r18,lo8(-10)
	mov r20,r28
	ldi r24,lo8(18)
	call spi_set_3bytes
.LM53:
	cp r28,r16
	cpc r29,r17
	brne .L42
.LVL35:
.L41:
/* epilogue: frame size=0 */
	pop r29
	pop r28
	pop r17
	pop r16
	ret
/* epilogue end (size=5) */
/* function rise_bias_ctrl size 31 (22) */
.LFE15:
	.size	rise_bias_ctrl, .-rise_bias_ctrl
.global	down_bias_ctrl
	.type	down_bias_ctrl, @function
down_bias_ctrl:
.LFB16:
.LM54:
/* prologue: frame size=0 */
	push r16
	push r17
	push r28
	push r29
/* prologue end (size=4) */
.LVL36:
	movw r28,r24
	movw r16,r22
.LM55:
	cp r24,r22
	cpc r25,r23
	brne .L50
	rjmp .L49
.LVL37:
.L47:
.LM56:
	andi r22,lo8(-112)
	ldi r18,lo8(-10)
	mov r20,r28
	ldi r24,lo8(18)
	call spi_set_3bytes
.LM57:
	cp r28,r16
	cpc r29,r17
	breq .L49
.LVL38:
.L50:
.LM58:
	sbiw r28,1
.LM59:
	mov r22,r29
	clr r23
	cpi r22,143
	cpc r23,__zero_reg__
	brne .L47
.LM60:
	andi r29,hi8(255)
.LM61:
	ori r29,hi8(4096)
	mov r22,r29
	clr r23
	rjmp .L47
.LVL39:
.L49:
/* epilogue: frame size=0 */
	pop r29
	pop r28
	pop r17
	pop r16
	ret
/* epilogue end (size=5) */
/* function down_bias_ctrl size 36 (27) */
.LFE16:
	.size	down_bias_ctrl, .-down_bias_ctrl
.global	action_lcd
	.type	action_lcd, @function
action_lcd:
.LFB20:
.LM62:
/* prologue: frame size=0 */
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r17
	push r28
	push r29
/* prologue end (size=11) */
.L54:
.LM63:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(17)
	brlo .L54
.LM64:
	ldi r24,lo8(97)
	sts guca_out_msg_buff,r24
.LM65:
	sts guca_out_msg_buff+1,__zero_reg__
.LM66:
	sts 236,__zero_reg__
	ldi r30,lo8(237)
	ldi r31,hi8(237)
.L56:
	st Z+,__zero_reg__
.LM67:
	cpi r30,255
	cpc r31,__zero_reg__
	brne .L56
	ldi r28,lo8(0)
	ldi r29,hi8(0)
.LM68:
	ldi r19,lo8(gta_digit_set0_map)
	mov r12,r19
	ldi r19,hi8(gta_digit_set0_map)
	mov r13,r19
	ldi r18,lo8(guca_in_msg_buff+3)
	mov r14,r18
	ldi r18,hi8(guca_in_msg_buff+3)
	mov r15,r18
	ldi r25,lo8(gta_digit_set1_map)
	mov r10,r25
	ldi r25,hi8(gta_digit_set1_map)
	mov r11,r25
	ldi r24,lo8(5)
	mov r8,r24
	mov r9,__zero_reg__
	add r8,r14
	adc r9,r15
.LVL40:
.L58:
	tst r28
	brne .+2
	rjmp .L59
	movw r22,r10
	movw r26,r8
.L61:
	ldi r17,lo8(0)
.L62:
.LBB2:
.LBB5:
.LM69:
	ld r30,X+
	ldi r31,lo8(0)
	subi r30,lo8(-(gta_digit_decode))
	sbci r31,hi8(-(gta_digit_decode))
/* #APP */
	lpm r24, Z
	
/* #NOAPP */
.LBE5:
.LM70:
	tst r24
	breq .L63
.LM71:
	movw r20,r22
	subi r20,lo8(-(1))
	sbci r21,hi8(-(1))
	mov r19,r24
	rjmp .L65
.L88:
.LM72:
	subi r20,lo8(-(2))
	sbci r21,hi8(-(2))
.L65:
.LM73:
	sbrs r19,0
	rjmp .L66
.LBB3:
.LBB4:
.LM74:
	movw r30,r20
.LVL41:
/* #APP */
	lpm r24, Z+
	lpm r25, Z
	
.LVL42:
/* #NOAPP */
.LBE4:
.LM75:
	movw r30,r24
.LVL43:
	andi r31,hi8(255)
	ld r18,Z
	mov r24,r25
	clr r25
.LVL44:
	or r18,r24
	st Z,r18
.LVL45:
.L66:
.LBE3:
.LM76:
	lsr r19
.LM77:
	brne .L88
.L63:
.LBE2:
.LM78:
	subi r17,lo8(-(1))
	cpi r17,lo8(5)
	breq .L69
.LBB6:
.LM79:
	subi r22,lo8(-(15))
	sbci r23,hi8(-(15))
	rjmp .L62
.L69:
	adiw r28,1
.LBE6:
.LM80:
	cpi r28,2
	cpc r29,__zero_reg__
	brne .L58
	ldi r26,lo8(guca_in_msg_buff+13)
	ldi r27,hi8(guca_in_msg_buff+13)
.LVL46:
	ldi r19,lo8(0)
.LVL47:
	ldi r22,lo8(gta_signs_map)
	ldi r23,hi8(gta_signs_map)
.LVL48:
	ldi r20,lo8(0)
	ldi r21,hi8(0)
.LVL49:
	rjmp .L72
.LVL50:
.L89:
.LM81:
	subi r22,lo8(-(2))
	sbci r23,hi8(-(2))
.LVL51:
.L72:
.LM82:
	movw r24,r20
.LVL52:
	andi r24,lo8(7)
	andi r25,hi8(7)
	or r24,r25
	brne .L73
.LM83:
	ld r19,X+
.L73:
.LM84:
	sbrs r19,0
	rjmp .L75
.LBB7:
.LBB8:
.LM85:
	movw r30,r22
.LVL53:
/* #APP */
	lpm r24, Z+
	lpm r25, Z
	
.LVL54:
/* #NOAPP */
.LBE8:
.LM86:
	movw r30,r24
.LVL55:
	andi r31,hi8(255)
	ld r18,Z
	mov r24,r25
	clr r25
.LVL56:
	or r18,r24
	st Z,r18
.LVL57:
.L75:
	subi r20,lo8(-(1))
	sbci r21,hi8(-(1))
.LBE7:
.LM87:
	cpi r20,32
	cpc r21,__zero_reg__
	brne .L89
.LM88:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(17)
	breq .L79
.LM89:
	ldi r24,lo8(1018)
	ldi r25,hi8(1018)
	call FATAL_TRAP
.LVL58:
	rjmp .L79
.LVL59:
.L59:
.LM90:
	movw r22,r12
	movw r26,r14
	rjmp .L61
.LVL60:
.L79:
.LM91:
	ldi r24,lo8(2)
	sts guc_out_msg_wr_idx,r24
/* epilogue: frame size=0 */
	pop r29
	pop r28
	pop r17
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	ret
/* epilogue end (size=12) */
/* function action_lcd size 165 (142) */
.LFE20:
	.size	action_lcd, .-action_lcd
.global	action_apd_off
	.type	action_apd_off, @function
action_apd_off:
.LFB18:
.LM92:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.L92:
.LM93:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(3)
	brlo .L92
.LM94:
	ldi r24,lo8(34)
	sts guca_out_msg_buff,r24
.LM95:
	sts guca_out_msg_buff+1,__zero_reg__
.LM96:
	call spi_read_curr
.LM97:
	ldi r22,lo8(118)
	ldi r24,lo8(22)
	call spi_set_byte
.LM98:
	lds r24,guca_spi_curr_state+9
	cpi r24,lo8(16)
	brne .L94
	lds r24,guca_spi_curr_state+10
	tst r24
	brne .L94
.L96:
.LM99:
	lds r22,guca_spi_curr_state+3
	mov r24,r22
	andi r24,lo8(-64)
	cpi r24,lo8(-64)
	breq .L108
.L97:
.LM100:
	lds r24,guca_spi_curr_state+8
	tst r24
	brne .L109
.L99:
.LM101:
	lds r22,guca_spi_curr_state+3
	sbrc r22,5
	rjmp .L110
.L101:
.LM102:
	ldi r22,lo8(-10)
	ldi r24,lo8(22)
	call spi_set_byte
.LM103:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(3)
	breq .L103
.LM104:
	ldi r24,lo8(756)
	ldi r25,hi8(756)
	call FATAL_TRAP
	rjmp .L103
.L94:
.LM105:
	ldi r20,lo8(0)
	ldi r22,lo8(16)
	ldi r24,lo8(18)
	call spi_set_2bytes
	rjmp .L96
.L109:
.LM106:
	ldi r22,lo8(0)
	ldi r24,lo8(16)
	call spi_set_byte
.LM107:
	lds r22,guca_spi_curr_state+3
	andi r22,lo8(-17)
	sts guca_spi_curr_state+3,r22
.LM108:
	ldi r24,lo8(6)
	call spi_set_byte
	rjmp .L99
.L110:
.LM109:
	andi r22,lo8(-33)
	ldi r24,lo8(6)
	call spi_set_byte
	rjmp .L101
.L108:
.LM110:
	andi r22,lo8(63)
	sts guca_spi_curr_state+3,r22
.LM111:
	ldi r24,lo8(6)
	call spi_set_byte
	rjmp .L97
.L103:
.LM112:
	ldi r24,lo8(2)
	sts guc_out_msg_wr_idx,r24
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function action_apd_off size 85 (84) */
.LFE18:
	.size	action_apd_off, .-action_apd_off
.global	action_laser_off
	.type	action_laser_off, @function
action_laser_off:
.LFB14:
.LM113:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.L113:
.LM114:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(3)
	brlo .L113
.LM115:
	ldi r24,lo8(51)
	sts guca_out_msg_buff,r24
.LM116:
	sts guca_out_msg_buff+1,__zero_reg__
.LM117:
	call spi_read_curr
.LM118:
	lds r24,guca_spi_curr_state+8
	tst r24
	brne .L126
.L115:
.LM119:
	lds r22,guca_spi_curr_state+3
	mov r18,r22
	mov r24,r22
	andi r24,lo8(-64)
	brne .L127
.L117:
.LM120:
	sbrc r18,4
	rjmp .L128
.L119:
.LM121:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(3)
	breq .L121
.LM122:
	ldi r24,lo8(523)
	ldi r25,hi8(523)
	call FATAL_TRAP
	rjmp .L121
.L126:
.LM123:
	ldi r22,lo8(118)
	ldi r24,lo8(22)
	call spi_set_byte
.LM124:
	ldi r22,lo8(0)
	ldi r24,lo8(16)
	call spi_set_byte
.LM125:
	ldi r22,lo8(-10)
	ldi r24,lo8(22)
	call spi_set_byte
	rjmp .L115
.L128:
.LM126:
	andi r22,lo8(-17)
	ldi r24,lo8(6)
	call spi_set_byte
	rjmp .L119
.L127:
.LM127:
	andi r22,lo8(63)
	ldi r24,lo8(6)
	call spi_set_byte
	lds r22,guca_spi_curr_state+3
	mov r18,r22
	rjmp .L117
.L121:
.LM128:
	ldi r24,lo8(2)
	sts guc_out_msg_wr_idx,r24
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function action_laser_off size 63 (62) */
.LFE14:
	.size	action_laser_off, .-action_laser_off
.global	action_laser_aim
	.type	action_laser_aim, @function
action_laser_aim:
.LFB13:
.LM129:
/* prologue: frame size=0 */
	push r17
/* prologue end (size=1) */
.L131:
.LM130:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(4)
	brlo .L131
.LM131:
	lds r17,guca_in_msg_buff+3
.LVL61:
.LM132:
	ldi r24,lo8(50)
	sts guca_out_msg_buff,r24
.LM133:
	ldi r24,lo8(1)
	sts guca_out_msg_buff+1,r24
.LM134:
	call spi_read_curr
.LM135:
	cpi r17,lo8(2)
	breq .L153
.LM136:
	sts guca_out_msg_buff+2,r17
.LM137:
	cpi r17,lo8(1)
	breq .L138
.LM138:
	lds r24,guca_spi_curr_state+8
	tst r24
	breq .L143
.L144:
.LM139:
	ldi r22,lo8(118)
	ldi r24,lo8(22)
	call spi_set_byte
.LM140:
	ldi r22,lo8(0)
	ldi r24,lo8(16)
	call spi_set_byte
.LM141:
	ldi r22,lo8(-10)
	ldi r24,lo8(22)
	call spi_set_byte
.L143:
.LM142:
	lds r22,guca_spi_curr_state+3
	sbrc r22,4
	rjmp .L154
.L141:
.LM143:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(4)
	breq .L146
.LM144:
	ldi r24,lo8(469)
	ldi r25,hi8(469)
	call FATAL_TRAP
	rjmp .L146
.L154:
.LM145:
	andi r22,lo8(-17)
	ldi r24,lo8(6)
	call spi_set_byte
	rjmp .L141
.L153:
.LM146:
	lds r24,guca_spi_curr_state+8
	cpi r24,lo8(-85)
	breq .L155
.LM147:
	ldi r24,lo8(1)
	sts guca_out_msg_buff+2,r24
.L138:
.LM148:
	lds r22,guca_spi_curr_state+3
	sbrs r22,4
	rjmp .L156
.L139:
.LM149:
	lds r24,guca_spi_curr_state+8
	cpi r24,lo8(-85)
	breq .L141
.LM150:
	ldi r22,lo8(118)
	ldi r24,lo8(22)
	call spi_set_byte
.LM151:
	ldi r22,lo8(-85)
	ldi r24,lo8(16)
	call spi_set_byte
.LM152:
	ldi r22,lo8(-10)
	ldi r24,lo8(22)
	call spi_set_byte
	rjmp .L141
.L156:
.LM153:
	ori r22,lo8(16)
	ldi r24,lo8(6)
	call spi_set_byte
	rjmp .L139
.L155:
.LM154:
	sts guca_out_msg_buff+2,__zero_reg__
	rjmp .L144
.L146:
.LM155:
	ldi r24,lo8(3)
	sts guc_out_msg_wr_idx,r24
/* epilogue: frame size=0 */
	pop r17
	ret
/* epilogue end (size=2) */
/* function action_laser_aim size 96 (93) */
.LFE13:
	.size	action_laser_aim, .-action_laser_aim
.global	action_curtain
	.type	action_curtain, @function
action_curtain:
.LFB12:
.LM156:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.L159:
.LM157:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(4)
	brlo .L159
.LM158:
	lds r25,guca_in_msg_buff+3
.LVL62:
.LM159:
	cpi r25,lo8(2)
	brne .+2
	rjmp .L161
.LM160:
	in r24,49-0x20
	ori r24,lo8(48)
	out 49-0x20,r24
.LM161:
	in r24,48-0x20
	ori r24,lo8(48)
	out 48-0x20,r24
.LM162:
	in r24,85-0x20
.LVL63:
	ori r24,lo8(-128)
.LM163:
	out 85-0x20,r24
.LM164:
	out 85-0x20,r24
.LM165:
/* #APP */
	    nop
    nop
    nop
    nop

.LM166:
	    nop
    nop
    nop
    nop

.LM167:
/* #NOAPP */
	tst r25
	brne .L176
.LM168:
	cbi 49-0x20,5
.L165:
.LM169:
	ldi r24,lo8(-48)
.LVL64:
	sts guc_240usec_timer,r24
.L166:
.LM170:
	lds r24,guc_240usec_timer
	tst r24
	brne .L166
.LM171:
	in r24,49-0x20
	ori r24,lo8(48)
	out 49-0x20,r24
.LVL65:
.L168:
.LM172:
	ldi r24,lo8(65)
.LVL66:
	sts guca_out_msg_buff,r24
.LM173:
	ldi r24,lo8(1)
	sts guca_out_msg_buff+1,r24
.LM174:
	sts guca_out_msg_buff+2,r25
.LM175:
	ldi r24,lo8(3)
	sts guc_out_msg_wr_idx,r24
.LM176:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(4)
	breq .L171
.LM177:
	ldi r24,lo8(384)
	ldi r25,hi8(384)
	call FATAL_TRAP
.LVL67:
.LVL68:
.L171:
	ret
.LVL69:
.L176:
.LM178:
	cbi 49-0x20,4
	rjmp .L165
.LVL70:
.L161:
.LM179:
	in r24,85-0x20
.LVL71:
	andi r24,lo8(127)
.LM180:
	out 85-0x20,r24
.LM181:
	out 85-0x20,r24
	rjmp .L168
/* epilogue: frame size=0 */
/* epilogue: noreturn */
/* epilogue end (size=0) */
/* function action_curtain size 79 (79) */
.LFE12:
	.size	action_curtain, .-action_curtain
.global	action_signature
	.type	action_signature, @function
action_signature:
.LFB10:
.LM182:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.LM183:
	lds r24,gusa_trace+2
	lds r25,(gusa_trace+2)+1
	adiw r24,1
	sts (gusa_trace+2)+1,r25
	sts gusa_trace+2,r24
.LM184:
	ldi r24,lo8(17)
	sts guca_out_msg_buff,r24
.LM185:
	ldi r24,lo8(18)
	sts guca_out_msg_buff+1,r24
.LM186:
	lds r24,gt_hw_info
	sts guca_out_msg_buff+2,r24
.LM187:
	lds r24,gt_hw_info+1
	sts guca_out_msg_buff+3,r24
	ldi r30,lo8(gt_hw_info+2)
	ldi r31,hi8(gt_hw_info+2)
	ldi r26,lo8(guca_out_msg_buff+4)
	ldi r27,hi8(guca_out_msg_buff+4)
.L178:
.LM188:
	ld r24,Z+
	st X+,r24
.LM189:
	ldi r24,hi8(gt_hw_info+18)
	cpi r30,lo8(gt_hw_info+18)
	cpc r31,r24
	brne .L178
.LM190:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(2)
	breq .L180
.LM191:
	ldi r24,lo8(272)
	ldi r25,hi8(272)
	call FATAL_TRAP
.L180:
.LM192:
	ldi r24,lo8(20)
	sts guc_out_msg_wr_idx,r24
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function action_signature size 45 (44) */
.LFE10:
	.size	action_signature, .-action_signature
.global	action_spi_wr
	.type	action_spi_wr, @function
action_spi_wr:
.LFB9:
.LM193:
/* prologue: frame size=0 */
	push r17
/* prologue end (size=1) */
.L187:
.LM194:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(3)
	brlo .L187
.LM195:
	lds r19,guca_in_msg_buff+2
.LVL72:
	mov r20,r19
	subi r20,lo8(-(3))
.L189:
.LM196:
	lds r24,guc_in_msg_idx
	cp r24,r20
	brlo .L189
.LM197:
	ldi r24,lo8(21)
	sts guca_out_msg_buff,r24
.LM198:
	sts guca_out_msg_buff+1,r19
.LM199:
	cbi 46-0x20,3
.LM200:
	tst r19
	breq .L205
	ldi r26,lo8(guca_in_msg_buff+3)
	ldi r27,hi8(guca_in_msg_buff+3)
	ldi r30,lo8(guca_out_msg_buff+2)
	ldi r31,hi8(guca_out_msg_buff+2)
	ldi r18,lo8(0)
.LVL73:
.L194:
.LM201:
	ld r24,X+
	sts 186,r24
	ldi r25,lo8(17)
	sts 184,r25
	ldi r24,lo8(19)
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	lds r24,186
.LVL74:
.LM202:
	st Z+,r24
.LM203:
	subi r18,lo8(-(1))
	cp r18,r19
	brne .L194
	mov r17,r19
	subi r17,lo8(-(2))
.L193:
.LM204:
	sbi 46-0x20,3
.LM205:
	lds r24,guc_in_msg_idx
.LVL75:
	cp r24,r20
	breq .L196
.LM206:
	ldi r24,lo8(238)
	ldi r25,hi8(238)
	call FATAL_TRAP
.LVL76:
	rjmp .L196
.LVL77:
.L205:
.LM207:
	ldi r17,lo8(2)
	rjmp .L193
.LVL78:
.L196:
.LM208:
	sts guc_out_msg_wr_idx,r17
/* epilogue: frame size=0 */
	pop r17
	ret
/* epilogue end (size=2) */
/* function action_spi_wr size 87 (84) */
.LFE9:
	.size	action_spi_wr, .-action_spi_wr
.global	action_spi_rd
	.type	action_spi_rd, @function
action_spi_rd:
.LFB8:
.LM209:
/* prologue: frame size=0 */
	push r17
/* prologue end (size=1) */
.L208:
.LM210:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(4)
	brlo .L208
.LM211:
	lds r19,guca_in_msg_buff+3
.LVL79:
.LM212:
	ldi r24,lo8(22)
	sts guca_out_msg_buff,r24
.LM213:
	subi r19,lo8(-(1))
	sts guca_out_msg_buff+1,r19
	subi r19,lo8(-(-1))
.LM214:
	cbi 46-0x20,3
.LM215:
	ldi r24,lo8(-128)
	sts 186,r24
	ldi r25,lo8(17)
	sts 184,r25
	ldi r24,lo8(19)
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	lds r24,186
.LVL80:
.LM216:
	sts guca_out_msg_buff+2,r24
.LM217:
	tst r19
	breq .L222
	ldi r30,lo8(guca_out_msg_buff+3)
	ldi r31,hi8(guca_out_msg_buff+3)
	ldi r18,lo8(0)
.LVL81:
.L213:
.LM218:
	sts 186,__zero_reg__
	ldi r25,lo8(17)
	sts 184,r25
	ldi r24,lo8(19)
.LVL82:
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	sts 184,r25
	sts 184,r24
	lds r24,186
.LVL83:
.LM219:
	st Z+,r24
.LM220:
	subi r18,lo8(-(1))
	cp r18,r19
	brne .L213
	mov r17,r19
	subi r17,lo8(-(3))
.L212:
.LM221:
	sbi 46-0x20,3
.LM222:
	lds r24,guc_in_msg_idx
.LVL84:
	cpi r24,lo8(4)
	breq .L215
.LM223:
	ldi r24,lo8(189)
	ldi r25,hi8(189)
	call FATAL_TRAP
.LVL85:
	rjmp .L215
.LVL86:
.L222:
.LM224:
	ldi r17,lo8(3)
	rjmp .L212
.LVL87:
.L215:
.LM225:
	sts guc_out_msg_wr_idx,r17
/* epilogue: frame size=0 */
	pop r17
	ret
/* epilogue end (size=2) */
/* function action_spi_rd size 121 (118) */
.LFE8:
	.size	action_spi_rd, .-action_spi_rd
.global	action_measure
	.type	action_measure, @function
action_measure:
.LFB19:
.LM226:
/* prologue: frame size=0 */
	push r28
	push r29
/* prologue end (size=2) */
.L225:
.LM227:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(4)
	brlo .L225
.LM228:
	ldi r24,lo8(81)
	sts guca_out_msg_buff,r24
.LM229:
	ldi r24,lo8(6)
	sts guca_out_msg_buff+1,r24
.LM230:
	sts guca_out_msg_buff+2,__zero_reg__
.LM231:
	lds r24,gta_freq_table
	lds r25,(gta_freq_table)+1
	or r24,r25
	breq .L227
	ldi r30,lo8(gta_freq_table)
	ldi r31,hi8(gta_freq_table)
.LVL88:
	ldi r18,lo8(0)
.L229:
.LM232:
	adiw r30,8
	subi r18,lo8(-(1))
.LM233:
	ld r24,Z
	ldd r25,Z+1
	or r24,r25
	brne .L229
	sts guca_out_msg_buff+2,r18
.L227:
.LM234:
	ldi r24,lo8(64)
	sts guca_out_msg_buff+3,r24
.LM235:
	ldi r24,lo8(16)
	sts guca_out_msg_buff+4,r24
.LM236:
	lds r24,guca_in_msg_buff+3
	sts guca_out_msg_buff+5,r24
.LM237:
	lds r24,guca_out_msg_buff+1
	call send_out_msg
.LVL89:
.LM238:
	call spi_read_curr
.LM239:
	lds r22,guca_spi_curr_state+3
	sbrs r22,4
	rjmp .L266
.L231:
.LM240:
	ldi r24,lo8(41)
	sts guc_240usec_timer,r24
.L233:
.LM241:
	lds r24,guc_240usec_timer
	tst r24
	brne .L233
.LM242:
	lds r24,guca_spi_curr_state+8
	cpi r24,lo8(102)
	breq .L235
.LM243:
	ldi r22,lo8(118)
	ldi r24,lo8(22)
	call spi_set_byte
.LM244:
	ldi r22,lo8(102)
	ldi r24,lo8(16)
	call spi_set_byte
.LM245:
	ldi r22,lo8(-10)
	ldi r24,lo8(22)
	call spi_set_byte
.L235:
.LM246:
	ldi r24,lo8(41)
	sts guc_240usec_timer,r24
.L237:
.LM247:
	lds r24,guc_240usec_timer
	tst r24
	brne .L237
.LM248:
	lds r22,guca_spi_curr_state+3
	mov r24,r22
	andi r24,lo8(-64)
	cpi r24,lo8(-64)
	breq .L239
.LM249:
	ori r22,lo8(-64)
	sts guca_spi_curr_state+3,r22
.LM250:
	ldi r24,lo8(6)
	call spi_set_byte
.L239:
.LM251:
	ldi r24,lo8(41)
	sts guc_240usec_timer,r24
.L241:
.LM252:
	lds r24,guc_240usec_timer
	tst r24
	brne .L241
.LM253:
	cbi 34-0x20,2
.LM254:
	lds r20,gta_freq_table
	lds r21,(gta_freq_table)+1
	cp r20,__zero_reg__
	cpc r21,__zero_reg__
	breq .L243
	ldi r28,lo8(gta_freq_table)
	ldi r29,hi8(gta_freq_table)
.LVL90:
.L245:
.LM255:
	mov r22,r21
	clr r23
	ldi r24,lo8(2)
	call spi_set_2bytes
.LM256:
	lds r24,guca_in_msg_buff+3
	andi r24,lo8(1)
	call activate_adc_isr
.L246:
.LM257:
	lds r24,122
	sbrc r24,7
	rjmp .L246
.LM258:
	lds r24,guca_in_msg_buff+3
	sbrc r24,1
	rjmp .L267
.L248:
.LM259:
	adiw r28,8
.LM260:
	ld r20,Y
	ldd r21,Y+1
	cp r20,__zero_reg__
	cpc r21,__zero_reg__
	brne .L245
.L243:
.LM261:
	sbi 34-0x20,2
.LM262:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(4)
	breq .L252
.LM263:
	ldi r24,lo8(899)
	ldi r25,hi8(899)
	call FATAL_TRAP
	rjmp .L252
.L267:
.LM264:
	movw r24,r28
	call calc_sincos
.LM265:
	ldd r24,Y+4
	ldd r25,Y+5
	sts guca_out_msg_buff,r24
.LM266:
	mov r24,r25
	lsl r25
	sbc r25,r25
	sts guca_out_msg_buff+1,r24
.LM267:
	ldd r24,Y+2
	ldd r25,Y+3
	sts guca_out_msg_buff+2,r24
.LM268:
	mov r24,r25
	lsl r25
	sbc r25,r25
	sts guca_out_msg_buff+3,r24
.LM269:
	ldi r24,lo8(4)
	call send_out_msg
	rjmp .L248
.LVL91:
.L266:
.LM270:
	ori r22,lo8(16)
	sts guca_spi_curr_state+3,r22
.LM271:
	ldi r24,lo8(6)
	call spi_set_byte
	rjmp .L231
.LVL92:
.L252:
/* epilogue: frame size=0 */
	pop r29
	pop r28
	ret
/* epilogue end (size=3) */
/* function action_measure size 179 (174) */
.LFE19:
	.size	action_measure, .-action_measure
.global	action_apd_on
	.type	action_apd_on, @function
action_apd_on:
.LFB17:
.LM272:
/* prologue: frame size=8 */
	push r28
	push r29
	in r28,__SP_L__
	in r29,__SP_H__
	sbiw r28,8
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
/* prologue end (size=10) */
.L270:
.LM273:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(5)
	brlo .L270
.LM274:
	ldi r24,lo8(33)
	sts guca_out_msg_buff,r24
.LM275:
	ldi r24,lo8(6)
	sts guca_out_msg_buff+1,r24
.LM276:
	call spi_read_curr
.LM277:
	lds r22,guca_spi_curr_state+3
	mov r24,r22
	sbrs r22,5
	rjmp .L305
.L272:
.LM278:
	sbrs r24,4
	rjmp .L306
.L276:
.LM279:
	lds r24,guca_spi_curr_state+8
	cpi r24,lo8(102)
	breq .L279
.LM280:
	ldi r22,lo8(118)
	ldi r24,lo8(22)
	call spi_set_byte
.LM281:
	ldi r22,lo8(102)
	ldi r24,lo8(16)
	call spi_set_byte
.LM282:
	ldi r22,lo8(-10)
	ldi r24,lo8(22)
	call spi_set_byte
.L279:
.LM283:
	lds r22,guca_spi_curr_state+3
	mov r24,r22
	andi r24,lo8(-64)
	cpi r24,lo8(-64)
	breq .L281
.LM284:
	ori r22,lo8(-64)
	sts guca_spi_curr_state+3,r22
.LM285:
	ldi r24,lo8(6)
	call spi_set_byte
.L281:
.LM286:
	lds r24,guca_in_msg_buff+4
	ldi r25,lo8(0)
	mov r23,r24
	clr r22
	lds r24,guca_in_msg_buff+3
	ldi r25,lo8(0)
	or r22,r24
	or r23,r25
.LM287:
	cp r22,__zero_reg__
	cpc r23,__zero_reg__
	breq .L283
.LM288:
	sbrs r23,4
	rjmp .L283
	movw r24,r22
	andi r24,lo8(28416)
	andi r25,hi8(28416)
	or r24,r25
	brne .L283
.LM289:
	lds r24,guca_spi_curr_state+9
	ldi r25,lo8(0)
	mov r19,r24
	clr r18
.LVL93:
	lds r24,guca_spi_curr_state+10
	ldi r25,lo8(0)
	or r18,r24
	or r19,r25
.LM290:
	cp r18,r22
	cpc r19,r23
	brlo .+2
	rjmp .L287
.LM291:
	movw r24,r18
	call rise_bias_ctrl
.LVL94:
.L283:
.LM292:
	call spi_read_curr
.LM293:
	lds r24,guca_spi_curr_state+10
	sts guca_out_msg_buff+2,r24
.LM294:
	lds r24,guca_spi_curr_state+9
	sts guca_out_msg_buff+3,r24
.LM295:
	ldi r24,lo8(83)
	sts guc_240usec_timer,r24
.L289:
.LM296:
	lds r24,guc_240usec_timer
	tst r24
	brne .L289
.LM297:
	cbi 34-0x20,2
.LM298:
	call activate_adc_isr
.L291:
.LM299:
	lds r24,122
	sbrc r24,7
	rjmp .L291
.LM300:
	sbi 34-0x20,2
.LM301:
	movw r24,r28
	adiw r24,1
	call calc_sincos
.LM302:
	ldd r24,Y+5
	ldd r25,Y+6
	sts guca_out_msg_buff+4,r24
.LM303:
	mov r24,r25
	lsl r25
	sbc r25,r25
	sts guca_out_msg_buff+5,r24
.LM304:
	ldd r24,Y+3
	ldd r25,Y+4
	sts guca_out_msg_buff+6,r24
.LM305:
	mov r24,r25
	lsl r25
	sbc r25,r25
	sts guca_out_msg_buff+7,r24
.LM306:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(5)
	breq .L293
.LM307:
	ldi r24,lo8(690)
	ldi r25,hi8(690)
	call FATAL_TRAP
	rjmp .L293
.L306:
.LM308:
	ori r22,lo8(16)
	sts guca_spi_curr_state+3,r22
.LM309:
	ldi r24,lo8(6)
	call spi_set_byte
.LM310:
	ldi r24,lo8(83)
	sts guc_240usec_timer,r24
.L278:
.LM311:
	lds r24,guc_240usec_timer
	tst r24
	brne .L278
	rjmp .L276
.L305:
.LM312:
	ori r22,lo8(32)
	sts guca_spi_curr_state+3,r22
.LM313:
	ldi r24,lo8(6)
	call spi_set_byte
.LM314:
	ldi r24,lo8(-48)
	sts guc_240usec_timer,r24
.L274:
.LM315:
	lds r24,guc_240usec_timer
	tst r24
	brne .L274
	lds r22,guca_spi_curr_state+3
	mov r24,r22
	rjmp .L272
.LVL95:
.L287:
.LM316:
	movw r24,r18
	call down_bias_ctrl
.LVL96:
	rjmp .L283
.L293:
.LM317:
	ldi r24,lo8(8)
	sts guc_out_msg_wr_idx,r24
/* epilogue: frame size=8 */
	adiw r28,8
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
	pop r29
	pop r28
	ret
/* epilogue end (size=9) */
/* function action_apd_on size 197 (178) */
.LFE17:
	.size	action_apd_on, .-action_apd_on
.global	gta_freq_table
	.data
	.type	gta_freq_table, @object
	.size	gta_freq_table, 128
gta_freq_table:
	.word	14308
	.word	0
	.word	0
	.word	0
	.word	11236
	.word	0
	.word	0
	.word	0
	.word	7908
	.word	0
	.word	0
	.word	0
	.word	-13867
	.word	0
	.word	0
	.word	0
	.word	-16939
	.word	0
	.word	0
	.word	0
	.word	-18987
	.word	0
	.word	0
	.word	0
	.word	-22059
	.word	0
	.word	0
	.word	0
	.word	-11335
	.word	0
	.word	0
	.word	0
	.word	-14407
	.word	0
	.word	0
	.word	0
	.word	-19271
	.word	0
	.word	0
	.word	0
	.word	-22343
	.word	0
	.word	0
	.word	0
	.word	-8532
	.word	0
	.word	0
	.word	0
	.word	-11604
	.word	0
	.word	0
	.word	0
	.word	-21587
	.word	0
	.word	0
	.word	0
	.word	-24659
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.skip 2,0
.global	gta_action_table
	.section	.text,"aw",@progbits
	.type	gta_action_table, @object
	.size	gta_action_table, 448
gta_action_table:
	.ascii	"mark"
	.byte	0
	.word	-275
	.ascii	"sign"
	.byte	17
	.word	gs(action_signature)
	.ascii	"amux"
	.byte	20
	.word	gs(action_adc_mux)
	.ascii	"spiw"
	.byte	21
	.word	gs(action_spi_wr)
	.ascii	"spir"
	.byte	22
	.word	gs(action_spi_rd)
	.ascii	"curt"
	.byte	65
	.word	gs(action_curtain)
	.ascii	"laim"
	.byte	50
	.word	gs(action_laser_aim)
	.ascii	"loff"
	.byte	51
	.word	gs(action_laser_off)
	.ascii	"apde"
	.byte	33
	.word	gs(action_apd_on)
	.ascii	"apdd"
	.byte	34
	.word	gs(action_apd_off)
	.ascii	"meas"
	.byte	81
	.word	gs(action_measure)
	.ascii	"lcd "
	.byte	97
	.word	gs(action_lcd)
	.string	""
	.skip 3,0
	.byte	-1
	.word	0
	.skip 357,0
.global	gta_digit_decode
	.type	gta_digit_decode, @object
	.size	gta_digit_decode, 32
gta_digit_decode:
	.byte	63
	.byte	6
	.byte	91
	.byte	79
	.byte	102
	.byte	109
	.byte	125
	.byte	7
	.byte	127
	.byte	111
	.byte	119
	.byte	124
	.byte	57
	.byte	94
	.byte	121
	.byte	113
	.byte	0
	.byte	115
	.byte	63
	.byte	118
	.byte	56
	.byte	6
	.byte	62
	.byte	84
	.byte	124
	.byte	94
	.byte	80
	.byte	92
	.byte	4
	.byte	116
	.byte	64
	.byte	8
.global	gta_digit_set0_map
	.type	gta_digit_set0_map, @object
	.size	gta_digit_set0_map, 75
gta_digit_set0_map:
	.byte	4
	.byte	-19
	.byte	4
	.byte	-14
	.byte	2
	.byte	-4
	.byte	2
	.byte	-4
	.byte	4
	.byte	-9
	.byte	4
	.byte	-14
	.byte	4
	.byte	-9
	.byte	2
	.byte	3
	.byte	-19
	.byte	1
	.byte	-20
	.byte	-128
	.byte	-10
	.byte	-128
	.byte	-4
	.byte	1
	.byte	-9
	.byte	1
	.byte	-14
	.byte	1
	.byte	-15
	.byte	-128
	.byte	2
	.byte	-20
	.byte	64
	.byte	-20
	.byte	32
	.byte	-10
	.byte	32
	.byte	-5
	.byte	64
	.byte	-10
	.byte	64
	.byte	-15
	.byte	64
	.byte	-15
	.byte	32
	.byte	1
	.byte	-20
	.byte	16
	.byte	-15
	.byte	8
	.byte	-5
	.byte	8
	.byte	-5
	.byte	16
	.byte	-10
	.byte	16
	.byte	-15
	.byte	16
	.byte	-10
	.byte	8
	.byte	0
	.byte	-20
	.byte	4
	.byte	-15
	.byte	2
	.byte	-5
	.byte	2
	.byte	-5
	.byte	4
	.byte	-10
	.byte	4
	.byte	-15
	.byte	4
	.byte	-10
	.byte	2
.global	gta_digit_set1_map
	.type	gta_digit_set1_map, @object
	.size	gta_digit_set1_map, 75
gta_digit_set1_map:
	.byte	4
	.byte	-4
	.byte	32
	.byte	-4
	.byte	64
	.byte	-14
	.byte	64
	.byte	-19
	.byte	32
	.byte	-14
	.byte	32
	.byte	-9
	.byte	32
	.byte	-9
	.byte	64
	.byte	3
	.byte	-4
	.byte	-128
	.byte	-3
	.byte	1
	.byte	-13
	.byte	1
	.byte	-19
	.byte	-128
	.byte	-14
	.byte	-128
	.byte	-9
	.byte	-128
	.byte	-8
	.byte	1
	.byte	2
	.byte	-3
	.byte	2
	.byte	-3
	.byte	4
	.byte	-13
	.byte	4
	.byte	-18
	.byte	2
	.byte	-13
	.byte	2
	.byte	-8
	.byte	2
	.byte	-8
	.byte	4
	.byte	1
	.byte	-3
	.byte	8
	.byte	-3
	.byte	16
	.byte	-13
	.byte	16
	.byte	-18
	.byte	8
	.byte	-13
	.byte	8
	.byte	-8
	.byte	8
	.byte	-8
	.byte	16
	.byte	0
	.byte	-3
	.byte	32
	.byte	-3
	.byte	64
	.byte	-13
	.byte	64
	.byte	-18
	.byte	32
	.byte	-13
	.byte	32
	.byte	-8
	.byte	32
	.byte	-8
	.byte	64
.global	gta_signs_map
	.type	gta_signs_map, @object
	.size	gta_signs_map, 64
gta_signs_map:
	.byte	-5
	.byte	32
	.byte	-5
	.byte	-128
	.byte	-5
	.byte	1
	.byte	-10
	.byte	1
	.byte	-18
	.byte	16
	.byte	-18
	.byte	4
	.byte	-18
	.byte	1
	.byte	-8
	.byte	-128
	.byte	-7
	.byte	1
	.byte	-15
	.byte	1
	.byte	-20
	.byte	1
	.byte	-17
	.byte	1
	.byte	-12
	.byte	1
	.byte	-18
	.byte	-128
	.byte	-13
	.byte	-128
	.byte	-18
	.byte	64
	.byte	-20
	.byte	2
	.byte	-20
	.byte	8
	.byte	-19
	.byte	2
	.byte	-19
	.byte	16
	.byte	-19
	.byte	8
	.byte	-14
	.byte	8
	.byte	-19
	.byte	64
	.byte	-14
	.byte	16
	.byte	-4
	.byte	8
	.byte	-4
	.byte	16
	.byte	-9
	.byte	8
	.byte	-9
	.byte	16
	.byte	-3
	.byte	-128
	.byte	-2
	.byte	1
	.byte	-2
	.byte	1
	.byte	-2
	.byte	1
	.comm guc_in_msg_idx,1,1
	.comm guca_in_msg_buff,128,1
	.comm guc_out_msg_wr_idx,1,1
	.comm guca_out_msg_buff,128,1
	.comm guc_adc_mux_hdr,1,1
	.comm guc_out_msg_rd_idx,1,1
	.comm guca_spi_curr_state,26,1
	.section	.debug_frame,"",@progbits
.Lframe0:
	.long	.LECIE0-.LSCIE0
.LSCIE0:
	.long	0xffffffff
	.byte	0x1
	.string	""
	.uleb128 0x1
	.sleb128 -1
	.byte	0x24
	.byte	0xc
	.uleb128 0x20
	.uleb128 0x0
	.p2align	2
.LECIE0:
.LSFDE0:
	.long	.LEFDE0-.LASFDE0
.LASFDE0:
	.long	.Lframe0
	.long	.LFB2
	.long	.LFE2-.LFB2
	.p2align	2
.LEFDE0:
.LSFDE2:
	.long	.LEFDE2-.LASFDE2
.LASFDE2:
	.long	.Lframe0
	.long	.LFB3
	.long	.LFE3-.LFB3
	.p2align	2
.LEFDE2:
.LSFDE4:
	.long	.LEFDE4-.LASFDE4
.LASFDE4:
	.long	.Lframe0
	.long	.LFB4
	.long	.LFE4-.LFB4
	.p2align	2
.LEFDE4:
.LSFDE6:
	.long	.LEFDE6-.LASFDE6
.LASFDE6:
	.long	.Lframe0
	.long	.LFB5
	.long	.LFE5-.LFB5
	.p2align	2
.LEFDE6:
.LSFDE8:
	.long	.LEFDE8-.LASFDE8
.LASFDE8:
	.long	.Lframe0
	.long	.LFB6
	.long	.LFE6-.LFB6
	.p2align	2
.LEFDE8:
.LSFDE10:
	.long	.LEFDE10-.LASFDE10
.LASFDE10:
	.long	.Lframe0
	.long	.LFB7
	.long	.LFE7-.LFB7
	.p2align	2
.LEFDE10:
.LSFDE12:
	.long	.LEFDE12-.LASFDE12
.LASFDE12:
	.long	.Lframe0
	.long	.LFB11
	.long	.LFE11-.LFB11
	.p2align	2
.LEFDE12:
.LSFDE14:
	.long	.LEFDE14-.LASFDE14
.LASFDE14:
	.long	.Lframe0
	.long	.LFB15
	.long	.LFE15-.LFB15
	.p2align	2
.LEFDE14:
.LSFDE16:
	.long	.LEFDE16-.LASFDE16
.LASFDE16:
	.long	.Lframe0
	.long	.LFB16
	.long	.LFE16-.LFB16
	.p2align	2
.LEFDE16:
.LSFDE18:
	.long	.LEFDE18-.LASFDE18
.LASFDE18:
	.long	.Lframe0
	.long	.LFB20
	.long	.LFE20-.LFB20
	.p2align	2
.LEFDE18:
.LSFDE20:
	.long	.LEFDE20-.LASFDE20
.LASFDE20:
	.long	.Lframe0
	.long	.LFB18
	.long	.LFE18-.LFB18
	.p2align	2
.LEFDE20:
.LSFDE22:
	.long	.LEFDE22-.LASFDE22
.LASFDE22:
	.long	.Lframe0
	.long	.LFB14
	.long	.LFE14-.LFB14
	.p2align	2
.LEFDE22:
.LSFDE24:
	.long	.LEFDE24-.LASFDE24
.LASFDE24:
	.long	.Lframe0
	.long	.LFB13
	.long	.LFE13-.LFB13
	.p2align	2
.LEFDE24:
.LSFDE26:
	.long	.LEFDE26-.LASFDE26
.LASFDE26:
	.long	.Lframe0
	.long	.LFB12
	.long	.LFE12-.LFB12
	.p2align	2
.LEFDE26:
.LSFDE28:
	.long	.LEFDE28-.LASFDE28
.LASFDE28:
	.long	.Lframe0
	.long	.LFB10
	.long	.LFE10-.LFB10
	.p2align	2
.LEFDE28:
.LSFDE30:
	.long	.LEFDE30-.LASFDE30
.LASFDE30:
	.long	.Lframe0
	.long	.LFB9
	.long	.LFE9-.LFB9
	.p2align	2
.LEFDE30:
.LSFDE32:
	.long	.LEFDE32-.LASFDE32
.LASFDE32:
	.long	.Lframe0
	.long	.LFB8
	.long	.LFE8-.LFB8
	.p2align	2
.LEFDE32:
.LSFDE34:
	.long	.LEFDE34-.LASFDE34
.LASFDE34:
	.long	.Lframe0
	.long	.LFB19
	.long	.LFE19-.LFB19
	.p2align	2
.LEFDE34:
.LSFDE36:
	.long	.LEFDE36-.LASFDE36
.LASFDE36:
	.long	.Lframe0
	.long	.LFB17
	.long	.LFE17-.LFB17
	.p2align	2
.LEFDE36:
	.text
.Letext0:
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
.LLST1:
	.long	.LVL0-.Ltext0
	.long	.LVL1-.Ltext0
	.word	0x1
	.byte	0x68
	.long	0x0
	.long	0x0
.LLST2:
	.long	.LVL2-.Ltext0
	.long	.LVL3-.Ltext0
	.word	0x1
	.byte	0x62
	.long	.LVL3-.Ltext0
	.long	.LFE2-.Ltext0
	.word	0x1
	.byte	0x68
	.long	0x0
	.long	0x0
.LLST4:
	.long	.LVL4-.Ltext0
	.long	.LVL5-.Ltext0
	.word	0x1
	.byte	0x68
	.long	0x0
	.long	0x0
.LLST5:
	.long	.LVL6-.Ltext0
	.long	.LVL8-.Ltext0
	.word	0x1
	.byte	0x62
	.long	.LVL8-.Ltext0
	.long	.LFE3-.Ltext0
	.word	0x1
	.byte	0x68
	.long	0x0
	.long	0x0
.LLST7:
	.long	.LVL9-.Ltext0
	.long	.LVL10-.Ltext0
	.word	0x1
	.byte	0x68
	.long	0x0
	.long	0x0
.LLST8:
	.long	.LVL11-.Ltext0
	.long	.LVL14-.Ltext0
	.word	0x1
	.byte	0x63
	.long	.LVL14-.Ltext0
	.long	.LFE4-.Ltext0
	.word	0x1
	.byte	0x68
	.long	0x0
	.long	0x0
.LLST10:
	.long	.LVL15-.Ltext0
	.long	.LVL16-.Ltext0
	.word	0x6
	.byte	0x68
	.byte	0x93
	.uleb128 0x1
	.byte	0x69
	.byte	0x93
	.uleb128 0x1
	.long	.LVL16-.Ltext0
	.long	.LFE5-.Ltext0
	.word	0x6
	.byte	0x6e
	.byte	0x93
	.uleb128 0x1
	.byte	0x6f
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
.LLST12:
	.long	.LVL17-.Ltext0
	.long	.LVL18-.Ltext0
	.word	0x1
	.byte	0x68
	.long	.LVL18-.Ltext0
	.long	.LFE6-.Ltext0
	.word	0x1
	.byte	0x62
	.long	0x0
	.long	0x0
.LLST14:
	.long	.LVL20-.Ltext0
	.long	.LVL21-.Ltext0
	.word	0x1
	.byte	0x68
	.long	.LVL22-.Ltext0
	.long	.LVL23-.Ltext0
	.word	0x1
	.byte	0x68
	.long	0x0
	.long	0x0
.LLST16:
	.long	.LVL24-.Ltext0
	.long	.LVL25-.Ltext0
	.word	0x1
	.byte	0x62
	.long	.LVL25-.Ltext0
	.long	.LVL27-.Ltext0
	.word	0x6
	.byte	0x68
	.byte	0x93
	.uleb128 0x1
	.byte	0x69
	.byte	0x93
	.uleb128 0x1
	.long	.LVL28-.Ltext0
	.long	.LVL29-.Ltext0
	.word	0x6
	.byte	0x62
	.byte	0x93
	.uleb128 0x1
	.byte	0x69
	.byte	0x93
	.uleb128 0x1
	.long	.LVL29-.Ltext0
	.long	.LFE11-.Ltext0
	.word	0x1
	.byte	0x62
	.long	0x0
	.long	0x0
.LLST17:
	.long	.LVL27-.Ltext0
	.long	.LVL30-.Ltext0
	.word	0x1
	.byte	0x68
	.long	0x0
	.long	0x0
.LLST19:
	.long	.LVL31-.Ltext0
	.long	.LVL32-.Ltext0
	.word	0x6
	.byte	0x68
	.byte	0x93
	.uleb128 0x1
	.byte	0x69
	.byte	0x93
	.uleb128 0x1
	.long	.LVL32-.Ltext0
	.long	.LFE15-.Ltext0
	.word	0x6
	.byte	0x6c
	.byte	0x93
	.uleb128 0x1
	.byte	0x6d
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
.LLST20:
	.long	.LVL31-.Ltext0
	.long	.LVL32-.Ltext0
	.word	0x6
	.byte	0x66
	.byte	0x93
	.uleb128 0x1
	.byte	0x67
	.byte	0x93
	.uleb128 0x1
	.long	.LVL32-.Ltext0
	.long	.LVL33-.Ltext0
	.word	0x6
	.byte	0x60
	.byte	0x93
	.uleb128 0x1
	.byte	0x61
	.byte	0x93
	.uleb128 0x1
	.long	.LVL33-.Ltext0
	.long	.LVL34-.Ltext0
	.word	0x6
	.byte	0x66
	.byte	0x93
	.uleb128 0x1
	.byte	0x67
	.byte	0x93
	.uleb128 0x1
	.long	.LVL34-.Ltext0
	.long	.LFE15-.Ltext0
	.word	0x6
	.byte	0x60
	.byte	0x93
	.uleb128 0x1
	.byte	0x61
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
.LLST22:
	.long	.LVL36-.Ltext0
	.long	.LVL37-.Ltext0
	.word	0x6
	.byte	0x68
	.byte	0x93
	.uleb128 0x1
	.byte	0x69
	.byte	0x93
	.uleb128 0x1
	.long	.LVL37-.Ltext0
	.long	.LFE16-.Ltext0
	.word	0x6
	.byte	0x6c
	.byte	0x93
	.uleb128 0x1
	.byte	0x6d
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
.LLST23:
	.long	.LVL36-.Ltext0
	.long	.LVL37-.Ltext0
	.word	0x6
	.byte	0x66
	.byte	0x93
	.uleb128 0x1
	.byte	0x67
	.byte	0x93
	.uleb128 0x1
	.long	.LVL37-.Ltext0
	.long	.LFE16-.Ltext0
	.word	0x6
	.byte	0x60
	.byte	0x93
	.uleb128 0x1
	.byte	0x61
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
.LLST25:
	.long	.LVL40-.Ltext0
	.long	.LVL47-.Ltext0
	.word	0x1
	.byte	0x63
	.long	.LVL59-.Ltext0
	.long	.LVL60-.Ltext0
	.word	0x1
	.byte	0x63
	.long	0x0
	.long	0x0
.LLST26:
	.long	.LVL40-.Ltext0
	.long	.LVL56-.Ltext0
	.word	0x1
	.byte	0x61
	.long	.LVL57-.Ltext0
	.long	.LFE20-.Ltext0
	.word	0x1
	.byte	0x61
	.long	0x0
	.long	0x0
.LLST27:
	.long	.LVL40-.Ltext0
	.long	.LVL48-.Ltext0
	.word	0x6
	.byte	0x66
	.byte	0x93
	.uleb128 0x1
	.byte	0x67
	.byte	0x93
	.uleb128 0x1
	.long	.LVL59-.Ltext0
	.long	.LVL60-.Ltext0
	.word	0x6
	.byte	0x66
	.byte	0x93
	.uleb128 0x1
	.byte	0x67
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
.LLST28:
	.long	.LVL40-.Ltext0
	.long	.LVL58-.Ltext0
	.word	0x6
	.byte	0x6a
	.byte	0x93
	.uleb128 0x1
	.byte	0x6b
	.byte	0x93
	.uleb128 0x1
	.long	.LVL59-.Ltext0
	.long	.LFE20-.Ltext0
	.word	0x6
	.byte	0x6a
	.byte	0x93
	.uleb128 0x1
	.byte	0x6b
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
.LLST29:
	.long	.LVL47-.Ltext0
	.long	.LVL58-.Ltext0
	.word	0x1
	.byte	0x63
	.long	.LVL60-.Ltext0
	.long	.LFE20-.Ltext0
	.word	0x1
	.byte	0x63
	.long	0x0
	.long	0x0
.LLST30:
	.long	.LVL48-.Ltext0
	.long	.LVL58-.Ltext0
	.word	0x6
	.byte	0x66
	.byte	0x93
	.uleb128 0x1
	.byte	0x67
	.byte	0x93
	.uleb128 0x1
	.long	.LVL60-.Ltext0
	.long	.LFE20-.Ltext0
	.word	0x6
	.byte	0x66
	.byte	0x93
	.uleb128 0x1
	.byte	0x67
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
.LLST31:
	.long	.LVL40-.Ltext0
	.long	.LVL49-.Ltext0
	.word	0x6
	.byte	0x64
	.byte	0x93
	.uleb128 0x1
	.byte	0x65
	.byte	0x93
	.uleb128 0x1
	.long	.LVL59-.Ltext0
	.long	.LVL60-.Ltext0
	.word	0x6
	.byte	0x64
	.byte	0x93
	.uleb128 0x1
	.byte	0x65
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
.LLST32:
	.long	.LVL41-.Ltext0
	.long	.LVL43-.Ltext0
	.word	0x6
	.byte	0x6e
	.byte	0x93
	.uleb128 0x1
	.byte	0x6f
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
.LLST33:
	.long	.LVL42-.Ltext0
	.long	.LVL44-.Ltext0
	.word	0x6
	.byte	0x68
	.byte	0x93
	.uleb128 0x1
	.byte	0x69
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
.LLST34:
	.long	.LVL40-.Ltext0
	.long	.LVL42-.Ltext0
	.word	0x1
	.byte	0x68
	.long	.LVL45-.Ltext0
	.long	.LVL50-.Ltext0
	.word	0x1
	.byte	0x68
	.long	.LVL51-.Ltext0
	.long	.LVL52-.Ltext0
	.word	0x1
	.byte	0x68
	.long	.LVL59-.Ltext0
	.long	.LVL60-.Ltext0
	.word	0x1
	.byte	0x68
	.long	0x0
	.long	0x0
.LLST35:
	.long	.LVL53-.Ltext0
	.long	.LVL55-.Ltext0
	.word	0x6
	.byte	0x6e
	.byte	0x93
	.uleb128 0x1
	.byte	0x6f
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
.LLST36:
	.long	.LVL54-.Ltext0
	.long	.LVL56-.Ltext0
	.word	0x6
	.byte	0x68
	.byte	0x93
	.uleb128 0x1
	.byte	0x69
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
.LLST41:
	.long	.LVL62-.Ltext0
	.long	.LVL67-.Ltext0
	.word	0x1
	.byte	0x69
	.long	.LVL68-.Ltext0
	.long	.LFE12-.Ltext0
	.word	0x1
	.byte	0x69
	.long	0x0
	.long	0x0
.LLST42:
	.long	.LVL63-.Ltext0
	.long	.LVL64-.Ltext0
	.word	0x1
	.byte	0x68
	.long	.LVL65-.Ltext0
	.long	.LVL66-.Ltext0
	.word	0x1
	.byte	0x68
	.long	.LVL69-.Ltext0
	.long	.LVL70-.Ltext0
	.word	0x1
	.byte	0x68
	.long	.LVL71-.Ltext0
	.long	.LFE12-.Ltext0
	.word	0x1
	.byte	0x68
	.long	0x0
	.long	0x0
.LLST45:
	.long	.LVL72-.Ltext0
	.long	.LVL76-.Ltext0
	.word	0x1
	.byte	0x63
	.long	.LVL77-.Ltext0
	.long	.LFE9-.Ltext0
	.word	0x1
	.byte	0x63
	.long	0x0
	.long	0x0
.LLST46:
	.long	.LVL73-.Ltext0
	.long	.LVL76-.Ltext0
	.word	0x1
	.byte	0x62
	.long	.LVL78-.Ltext0
	.long	.LFE9-.Ltext0
	.word	0x1
	.byte	0x62
	.long	0x0
	.long	0x0
.LLST47:
	.long	.LVL74-.Ltext0
	.long	.LVL75-.Ltext0
	.word	0x1
	.byte	0x68
	.long	0x0
	.long	0x0
.LLST49:
	.long	.LVL79-.Ltext0
	.long	.LVL85-.Ltext0
	.word	0x1
	.byte	0x63
	.long	.LVL86-.Ltext0
	.long	.LFE8-.Ltext0
	.word	0x1
	.byte	0x63
	.long	0x0
	.long	0x0
.LLST50:
	.long	.LVL81-.Ltext0
	.long	.LVL85-.Ltext0
	.word	0x1
	.byte	0x62
	.long	.LVL87-.Ltext0
	.long	.LFE8-.Ltext0
	.word	0x1
	.byte	0x62
	.long	0x0
	.long	0x0
.LLST51:
	.long	.LVL80-.Ltext0
	.long	.LVL82-.Ltext0
	.word	0x1
	.byte	0x68
	.long	.LVL83-.Ltext0
	.long	.LVL84-.Ltext0
	.word	0x1
	.byte	0x68
	.long	.LVL86-.Ltext0
	.long	.LVL87-.Ltext0
	.word	0x1
	.byte	0x68
	.long	0x0
	.long	0x0
.LLST53:
	.long	.LVL88-.Ltext0
	.long	.LVL89-.Ltext0
	.word	0x6
	.byte	0x6e
	.byte	0x93
	.uleb128 0x1
	.byte	0x6f
	.byte	0x93
	.uleb128 0x1
	.long	.LVL90-.Ltext0
	.long	.LVL91-.Ltext0
	.word	0x6
	.byte	0x6c
	.byte	0x93
	.uleb128 0x1
	.byte	0x6d
	.byte	0x93
	.uleb128 0x1
	.long	.LVL92-.Ltext0
	.long	.LFE19-.Ltext0
	.word	0x6
	.byte	0x6c
	.byte	0x93
	.uleb128 0x1
	.byte	0x6d
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
.LLST55:
	.long	.LVL93-.Ltext0
	.long	.LVL94-.Ltext0
	.word	0x6
	.byte	0x62
	.byte	0x93
	.uleb128 0x1
	.byte	0x63
	.byte	0x93
	.uleb128 0x1
	.long	.LVL95-.Ltext0
	.long	.LVL96-.Ltext0
	.word	0x6
	.byte	0x62
	.byte	0x93
	.uleb128 0x1
	.byte	0x63
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
	.section	.debug_info
	.long	0x961
	.word	0x2
	.long	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.long	.LASF107
	.byte	0x1
	.long	.LASF108
	.long	.LASF109
	.long	.Ltext0
	.long	.Letext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.long	.LASF0
	.uleb128 0x3
	.long	.LASF2
	.byte	0x2
	.byte	0x7a
	.long	0x37
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.long	.LASF1
	.uleb128 0x3
	.long	.LASF3
	.byte	0x2
	.byte	0x7b
	.long	0x49
	.uleb128 0x4
	.byte	0x2
	.byte	0x5
	.string	"int"
	.uleb128 0x3
	.long	.LASF4
	.byte	0x2
	.byte	0x7c
	.long	0x5b
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.long	.LASF5
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.long	.LASF6
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF7
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.long	.LASF8
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.long	.LASF9
	.uleb128 0x5
	.byte	0x1
	.byte	0x8
	.uleb128 0x3
	.long	.LASF10
	.byte	0x3
	.byte	0x19
	.long	0x2c
	.uleb128 0x3
	.long	.LASF11
	.byte	0x3
	.byte	0x1a
	.long	0x3e
	.uleb128 0x3
	.long	.LASF12
	.byte	0x3
	.byte	0x1b
	.long	0x50
	.uleb128 0x6
	.byte	0x66
	.byte	0x3
	.byte	0x29
	.long	0xe3
	.uleb128 0x7
	.long	.LASF13
	.byte	0x3
	.byte	0x2a
	.long	0x81
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x7
	.long	.LASF14
	.byte	0x3
	.byte	0x2b
	.long	0x81
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0x7
	.long	.LASF15
	.byte	0x3
	.byte	0x2c
	.long	0xe3
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.uleb128 0x7
	.long	.LASF16
	.byte	0x3
	.byte	0x2d
	.long	0xfd
	.byte	0x2
	.byte	0x23
	.uleb128 0x12
	.byte	0x0
	.uleb128 0x8
	.long	0xf6
	.long	0xf3
	.uleb128 0x9
	.long	0xf3
	.byte	0xf
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.byte	0x7
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.long	.LASF17
	.uleb128 0x8
	.long	0x81
	.long	0x10d
	.uleb128 0x9
	.long	0xf3
	.byte	0x53
	.byte	0x0
	.uleb128 0x3
	.long	.LASF18
	.byte	0x3
	.byte	0x2e
	.long	0xa2
	.uleb128 0xa
	.long	.LASF23
	.byte	0x7
	.byte	0x3
	.byte	0x3d
	.long	0x14f
	.uleb128 0x7
	.long	.LASF19
	.byte	0x3
	.byte	0x3e
	.long	0x14f
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x7
	.long	.LASF20
	.byte	0x3
	.byte	0x3f
	.long	0x81
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x7
	.long	.LASF21
	.byte	0x3
	.byte	0x40
	.long	0x161
	.byte	0x2
	.byte	0x23
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x8
	.long	0xf6
	.long	0x15f
	.uleb128 0x9
	.long	0xf3
	.byte	0x3
	.byte	0x0
	.uleb128 0xb
	.byte	0x1
	.uleb128 0xc
	.byte	0x2
	.long	0x15f
	.uleb128 0x3
	.long	.LASF22
	.byte	0x3
	.byte	0x41
	.long	0x118
	.uleb128 0xa
	.long	.LASF24
	.byte	0x2
	.byte	0x3
	.byte	0x4a
	.long	0x19b
	.uleb128 0x7
	.long	.LASF25
	.byte	0x3
	.byte	0x4b
	.long	0x81
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x7
	.long	.LASF26
	.byte	0x3
	.byte	0x4c
	.long	0x81
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.byte	0x0
	.uleb128 0x3
	.long	.LASF27
	.byte	0x3
	.byte	0x4d
	.long	0x172
	.uleb128 0xa
	.long	.LASF28
	.byte	0xf
	.byte	0x3
	.byte	0x56
	.long	0x1cf
	.uleb128 0x7
	.long	.LASF29
	.byte	0x3
	.byte	0x57
	.long	0x81
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x7
	.long	.LASF30
	.byte	0x3
	.byte	0x58
	.long	0x1cf
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.byte	0x0
	.uleb128 0x8
	.long	0x19b
	.long	0x1df
	.uleb128 0x9
	.long	0xf3
	.byte	0x6
	.byte	0x0
	.uleb128 0x3
	.long	.LASF31
	.byte	0x3
	.byte	0x59
	.long	0x1a6
	.uleb128 0x6
	.byte	0x8
	.byte	0x3
	.byte	0x5b
	.long	0x22b
	.uleb128 0x7
	.long	.LASF32
	.byte	0x3
	.byte	0x5c
	.long	0x97
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x7
	.long	.LASF33
	.byte	0x3
	.byte	0x5e
	.long	0x8c
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.uleb128 0x7
	.long	.LASF34
	.byte	0x3
	.byte	0x5f
	.long	0x8c
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x7
	.long	.LASF35
	.byte	0x3
	.byte	0x61
	.long	0x97
	.byte	0x2
	.byte	0x23
	.uleb128 0x6
	.byte	0x0
	.uleb128 0x3
	.long	.LASF36
	.byte	0x3
	.byte	0x65
	.long	0x1ea
	.uleb128 0xd
	.byte	0x1
	.long	.LASF38
	.byte	0x1
	.byte	0x46
	.byte	0x1
	.long	.LFB2
	.long	.LFE2
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x27a
	.uleb128 0xe
	.long	.LASF25
	.byte	0x1
	.byte	0x46
	.long	0x81
	.long	.LLST1
	.uleb128 0xf
	.long	.LASF37
	.byte	0x1
	.byte	0x46
	.long	0x81
	.byte	0x1
	.byte	0x66
	.uleb128 0x10
	.long	.LASF42
	.byte	0x1
	.byte	0x47
	.long	0x81
	.long	.LLST2
	.byte	0x0
	.uleb128 0xd
	.byte	0x1
	.long	.LASF39
	.byte	0x1
	.byte	0x50
	.byte	0x1
	.long	.LFB3
	.long	.LFE3
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x2cb
	.uleb128 0xe
	.long	.LASF25
	.byte	0x1
	.byte	0x50
	.long	0x81
	.long	.LLST4
	.uleb128 0xf
	.long	.LASF40
	.byte	0x1
	.byte	0x50
	.long	0x81
	.byte	0x1
	.byte	0x66
	.uleb128 0xf
	.long	.LASF41
	.byte	0x1
	.byte	0x50
	.long	0x81
	.byte	0x1
	.byte	0x64
	.uleb128 0x10
	.long	.LASF42
	.byte	0x1
	.byte	0x51
	.long	0x81
	.long	.LLST5
	.byte	0x0
	.uleb128 0xd
	.byte	0x1
	.long	.LASF43
	.byte	0x1
	.byte	0x5b
	.byte	0x1
	.long	.LFB4
	.long	.LFE4
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x329
	.uleb128 0xe
	.long	.LASF25
	.byte	0x1
	.byte	0x5b
	.long	0x81
	.long	.LLST7
	.uleb128 0xf
	.long	.LASF40
	.byte	0x1
	.byte	0x5b
	.long	0x81
	.byte	0x1
	.byte	0x66
	.uleb128 0xf
	.long	.LASF41
	.byte	0x1
	.byte	0x5b
	.long	0x81
	.byte	0x1
	.byte	0x64
	.uleb128 0xf
	.long	.LASF44
	.byte	0x1
	.byte	0x5b
	.long	0x81
	.byte	0x1
	.byte	0x62
	.uleb128 0x10
	.long	.LASF42
	.byte	0x1
	.byte	0x5c
	.long	0x81
	.long	.LLST8
	.byte	0x0
	.uleb128 0xd
	.byte	0x1
	.long	.LASF45
	.byte	0x1
	.byte	0x67
	.byte	0x1
	.long	.LFB5
	.long	.LFE5
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x37d
	.uleb128 0xe
	.long	.LASF46
	.byte	0x1
	.byte	0x67
	.long	0x37d
	.long	.LLST10
	.uleb128 0x11
	.long	.LASF47
	.byte	0x1
	.byte	0x69
	.long	0x97
	.uleb128 0x11
	.long	.LASF48
	.byte	0x1
	.byte	0x69
	.long	0x97
	.uleb128 0x11
	.long	.LASF49
	.byte	0x1
	.byte	0x6a
	.long	0x81
	.uleb128 0x11
	.long	.LASF50
	.byte	0x1
	.byte	0x6a
	.long	0x81
	.byte	0x0
	.uleb128 0xc
	.byte	0x2
	.long	0x22b
	.uleb128 0xd
	.byte	0x1
	.long	.LASF51
	.byte	0x1
	.byte	0x79
	.byte	0x1
	.long	.LFB6
	.long	.LFE6
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x3b8
	.uleb128 0xe
	.long	.LASF52
	.byte	0x1
	.byte	0x79
	.long	0x81
	.long	.LLST12
	.uleb128 0x12
	.long	.LASF53
	.byte	0x1
	.byte	0x7b
	.long	0x81
	.byte	0x1
	.byte	0x69
	.byte	0x0
	.uleb128 0x13
	.byte	0x1
	.long	.LASF54
	.byte	0x1
	.byte	0x84
	.long	.LFB7
	.long	.LFE7
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x3ea
	.uleb128 0x10
	.long	.LASF42
	.byte	0x1
	.byte	0x86
	.long	0x81
	.long	.LLST14
	.uleb128 0x11
	.long	.LASF53
	.byte	0x1
	.byte	0x86
	.long	0x81
	.byte	0x0
	.uleb128 0x14
	.byte	0x1
	.long	.LASF55
	.byte	0x1
	.word	0x119
	.long	.LFB11
	.long	.LFE11
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x423
	.uleb128 0x15
	.long	.LASF56
	.byte	0x1
	.word	0x11b
	.long	0x81
	.long	.LLST16
	.uleb128 0x15
	.long	.LASF57
	.byte	0x1
	.word	0x11b
	.long	0x81
	.long	.LLST17
	.byte	0x0
	.uleb128 0x16
	.byte	0x1
	.long	.LASF58
	.byte	0x1
	.word	0x215
	.byte	0x1
	.long	.LFB15
	.long	.LFE15
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x45d
	.uleb128 0x17
	.long	.LASF59
	.byte	0x1
	.word	0x215
	.long	0x97
	.long	.LLST19
	.uleb128 0x17
	.long	.LASF60
	.byte	0x1
	.word	0x215
	.long	0x97
	.long	.LLST20
	.byte	0x0
	.uleb128 0x16
	.byte	0x1
	.long	.LASF61
	.byte	0x1
	.word	0x224
	.byte	0x1
	.long	.LFB16
	.long	.LFE16
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x497
	.uleb128 0x17
	.long	.LASF59
	.byte	0x1
	.word	0x224
	.long	0x97
	.long	.LLST22
	.uleb128 0x17
	.long	.LASF60
	.byte	0x1
	.word	0x224
	.long	0x97
	.long	.LLST23
	.byte	0x0
	.uleb128 0x14
	.byte	0x1
	.long	.LASF62
	.byte	0x1
	.word	0x38c
	.long	.LFB20
	.long	.LFE20
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x5f8
	.uleb128 0x18
	.long	.LASF53
	.byte	0x1
	.word	0x38e
	.long	0x81
	.uleb128 0x15
	.long	.LASF63
	.byte	0x1
	.word	0x38f
	.long	0x81
	.long	.LLST25
	.uleb128 0x15
	.long	.LASF64
	.byte	0x1
	.word	0x390
	.long	0x81
	.long	.LLST26
	.uleb128 0x18
	.long	.LASF65
	.byte	0x1
	.word	0x390
	.long	0x81
	.uleb128 0x15
	.long	.LASF66
	.byte	0x1
	.word	0x3ae
	.long	0x5f8
	.long	.LLST27
	.uleb128 0x15
	.long	.LASF67
	.byte	0x1
	.word	0x3af
	.long	0x5fe
	.long	.LLST28
	.uleb128 0x18
	.long	.LASF68
	.byte	0x1
	.word	0x3e0
	.long	0x81
	.uleb128 0x15
	.long	.LASF26
	.byte	0x1
	.word	0x3e0
	.long	0x81
	.long	.LLST29
	.uleb128 0x15
	.long	.LASF69
	.byte	0x1
	.word	0x3e1
	.long	0x604
	.long	.LLST30
	.uleb128 0x19
	.long	.Ldebug_ranges0+0x0
	.long	0x5b7
	.uleb128 0x15
	.long	.LASF70
	.byte	0x1
	.word	0x3c1
	.long	0x604
	.long	.LLST31
	.uleb128 0x1a
	.long	.LBB3
	.long	.LBE3
	.long	0x590
	.uleb128 0x18
	.long	.LASF71
	.byte	0x1
	.word	0x3d0
	.long	0x97
	.uleb128 0x1b
	.long	.LBB4
	.long	.LBE4
	.uleb128 0x15
	.long	.LASF72
	.byte	0x1
	.word	0x3d1
	.long	0x50
	.long	.LLST32
	.uleb128 0x15
	.long	.LASF73
	.byte	0x1
	.word	0x3d1
	.long	0x50
	.long	.LLST33
	.byte	0x0
	.byte	0x0
	.uleb128 0x1b
	.long	.LBB5
	.long	.LBE5
	.uleb128 0x18
	.long	.LASF72
	.byte	0x1
	.word	0x3c7
	.long	0x50
	.uleb128 0x15
	.long	.LASF73
	.byte	0x1
	.word	0x3c7
	.long	0x2c
	.long	.LLST34
	.byte	0x0
	.byte	0x0
	.uleb128 0x1b
	.long	.LBB7
	.long	.LBE7
	.uleb128 0x18
	.long	.LASF71
	.byte	0x1
	.word	0x3ef
	.long	0x97
	.uleb128 0x1b
	.long	.LBB8
	.long	.LBE8
	.uleb128 0x15
	.long	.LASF72
	.byte	0x1
	.word	0x3f0
	.long	0x50
	.long	.LLST35
	.uleb128 0x15
	.long	.LASF73
	.byte	0x1
	.word	0x3f0
	.long	0x50
	.long	.LLST36
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.uleb128 0xc
	.byte	0x2
	.long	0x1df
	.uleb128 0xc
	.byte	0x2
	.long	0x81
	.uleb128 0xc
	.byte	0x2
	.long	0x19b
	.uleb128 0x1c
	.byte	0x1
	.long	.LASF74
	.byte	0x1
	.word	0x2bb
	.long	.LFB18
	.long	.LFE18
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.uleb128 0x1c
	.byte	0x1
	.long	.LASF75
	.byte	0x1
	.word	0x1df
	.long	.LFB14
	.long	.LFE14
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.uleb128 0x14
	.byte	0x1
	.long	.LASF76
	.byte	0x1
	.word	0x187
	.long	.LFB13
	.long	.LFE13
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x659
	.uleb128 0x1d
	.long	.LASF77
	.byte	0x1
	.word	0x189
	.long	0x81
	.byte	0x1
	.byte	0x61
	.byte	0x0
	.uleb128 0x14
	.byte	0x1
	.long	.LASF78
	.byte	0x1
	.word	0x132
	.long	.LFB12
	.long	.LFE12
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x692
	.uleb128 0x15
	.long	.LASF79
	.byte	0x1
	.word	0x134
	.long	0x81
	.long	.LLST41
	.uleb128 0x15
	.long	.LASF57
	.byte	0x1
	.word	0x135
	.long	0x81
	.long	.LLST42
	.byte	0x0
	.uleb128 0x13
	.byte	0x1
	.long	.LASF80
	.byte	0x1
	.byte	0xf8
	.long	.LFB10
	.long	.LFE10
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x6b5
	.uleb128 0x11
	.long	.LASF53
	.byte	0x1
	.byte	0xfa
	.long	0x81
	.byte	0x0
	.uleb128 0x13
	.byte	0x1
	.long	.LASF81
	.byte	0x1
	.byte	0xc6
	.long	.LFB9
	.long	.LFE9
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x6fa
	.uleb128 0x10
	.long	.LASF82
	.byte	0x1
	.byte	0xc8
	.long	0x81
	.long	.LLST45
	.uleb128 0x10
	.long	.LASF53
	.byte	0x1
	.byte	0xc8
	.long	0x81
	.long	.LLST46
	.uleb128 0x10
	.long	.LASF42
	.byte	0x1
	.byte	0xc9
	.long	0x81
	.long	.LLST47
	.byte	0x0
	.uleb128 0x13
	.byte	0x1
	.long	.LASF83
	.byte	0x1
	.byte	0x96
	.long	.LFB8
	.long	.LFE8
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x73f
	.uleb128 0x10
	.long	.LASF82
	.byte	0x1
	.byte	0x98
	.long	0x81
	.long	.LLST49
	.uleb128 0x10
	.long	.LASF53
	.byte	0x1
	.byte	0x98
	.long	0x81
	.long	.LLST50
	.uleb128 0x10
	.long	.LASF42
	.byte	0x1
	.byte	0x99
	.long	0x81
	.long	.LLST51
	.byte	0x0
	.uleb128 0x14
	.byte	0x1
	.long	.LASF84
	.byte	0x1
	.word	0x300
	.long	.LFB19
	.long	.LFE19
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x768
	.uleb128 0x15
	.long	.LASF85
	.byte	0x1
	.word	0x302
	.long	0x37d
	.long	.LLST53
	.byte	0x0
	.uleb128 0x14
	.byte	0x1
	.long	.LASF86
	.byte	0x1
	.word	0x233
	.long	.LFB17
	.long	.LFE17
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x7ac
	.uleb128 0x1d
	.long	.LASF87
	.byte	0x1
	.word	0x235
	.long	0x22b
	.byte	0x2
	.byte	0x8c
	.sleb128 1
	.uleb128 0x18
	.long	.LASF88
	.byte	0x1
	.word	0x236
	.long	0x97
	.uleb128 0x15
	.long	.LASF89
	.byte	0x1
	.word	0x236
	.long	0x97
	.long	.LLST55
	.byte	0x0
	.uleb128 0x8
	.long	0x22b
	.long	0x7bc
	.uleb128 0x9
	.long	0xf3
	.byte	0xf
	.byte	0x0
	.uleb128 0x1e
	.long	.LASF90
	.byte	0x1
	.byte	0x2c
	.long	0x7ac
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	gta_freq_table
	.uleb128 0x1f
	.long	.LASF91
	.byte	0x3
	.byte	0x6b
	.long	0x10d
	.byte	0x1
	.byte	0x1
	.uleb128 0x8
	.long	0x167
	.long	0x7eb
	.uleb128 0x9
	.long	0xf3
	.byte	0x3f
	.byte	0x0
	.uleb128 0x20
	.long	.LASF92
	.byte	0x1
	.word	0x403
	.long	0x7db
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	gta_action_table
	.uleb128 0x8
	.long	0x81
	.long	0x80e
	.uleb128 0x9
	.long	0xf3
	.byte	0x1f
	.byte	0x0
	.uleb128 0x20
	.long	.LASF93
	.byte	0x1
	.word	0x413
	.long	0x7fe
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	gta_digit_decode
	.uleb128 0x8
	.long	0x1df
	.long	0x831
	.uleb128 0x9
	.long	0xf3
	.byte	0x4
	.byte	0x0
	.uleb128 0x20
	.long	.LASF94
	.byte	0x1
	.word	0x43b
	.long	0x821
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	gta_digit_set0_map
	.uleb128 0x20
	.long	.LASF95
	.byte	0x1
	.word	0x445
	.long	0x821
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	gta_digit_set1_map
	.uleb128 0x8
	.long	0x19b
	.long	0x867
	.uleb128 0x9
	.long	0xf3
	.byte	0x1f
	.byte	0x0
	.uleb128 0x20
	.long	.LASF96
	.byte	0x1
	.word	0x44e
	.long	0x857
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	gta_signs_map
	.uleb128 0x1e
	.long	.LASF97
	.byte	0x1
	.byte	0x1b
	.long	0x88c
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	guc_in_msg_idx
	.uleb128 0x21
	.long	0x81
	.uleb128 0x8
	.long	0x81
	.long	0x8a1
	.uleb128 0x9
	.long	0xf3
	.byte	0x7f
	.byte	0x0
	.uleb128 0x1e
	.long	.LASF98
	.byte	0x1
	.byte	0x1c
	.long	0x891
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	guca_in_msg_buff
	.uleb128 0x1e
	.long	.LASF99
	.byte	0x1
	.byte	0x1e
	.long	0x88c
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	guc_out_msg_wr_idx
	.uleb128 0x1e
	.long	.LASF100
	.byte	0x1
	.byte	0x21
	.long	0x891
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	guca_out_msg_buff
	.uleb128 0x1e
	.long	.LASF101
	.byte	0x1
	.byte	0x28
	.long	0x88c
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	guc_adc_mux_hdr
	.uleb128 0x1e
	.long	.LASF102
	.byte	0x1
	.byte	0x1f
	.long	0x88c
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	guc_out_msg_rd_idx
	.uleb128 0x1f
	.long	.LASF103
	.byte	0x4
	.byte	0x5c
	.long	0x88c
	.byte	0x1
	.byte	0x1
	.uleb128 0x8
	.long	0x97
	.long	0x918
	.uleb128 0x9
	.long	0xf3
	.byte	0x3
	.byte	0x0
	.uleb128 0x1f
	.long	.LASF104
	.byte	0x4
	.byte	0x5e
	.long	0x908
	.byte	0x1
	.byte	0x1
	.uleb128 0x8
	.long	0x81
	.long	0x935
	.uleb128 0x9
	.long	0xf3
	.byte	0x19
	.byte	0x0
	.uleb128 0x1e
	.long	.LASF105
	.byte	0x1
	.byte	0x23
	.long	0x925
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	guca_spi_curr_state
	.uleb128 0x8
	.long	0x97
	.long	0x957
	.uleb128 0x9
	.long	0xf3
	.byte	0xf
	.byte	0x0
	.uleb128 0x1f
	.long	.LASF106
	.byte	0x1
	.byte	0x2a
	.long	0x947
	.byte	0x1
	.byte	0x1
	.byte	0x0
	.section	.debug_abbrev
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0x0
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x16
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0x0
	.byte	0x0
	.uleb128 0x5
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x6
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x7
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x8
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x9
	.uleb128 0x21
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0xa
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xb
	.uleb128 0x15
	.byte	0x0
	.uleb128 0x27
	.uleb128 0xc
	.byte	0x0
	.byte	0x0
	.uleb128 0xc
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xd
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xe
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0xf
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x10
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x11
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x12
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x13
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x14
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x15
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x16
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x17
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x18
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x19
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x55
	.uleb128 0x6
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x1a
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x1b
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.byte	0x0
	.byte	0x0
	.uleb128 0x1c
	.uleb128 0x2e
	.byte	0x0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x1d
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x1e
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x1f
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0x0
	.byte	0x0
	.uleb128 0x20
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x21
	.uleb128 0x35
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,"",@progbits
	.long	0x283
	.word	0x2
	.long	.Ldebug_info0
	.long	0x965
	.long	0x236
	.string	"spi_set_byte"
	.long	0x27a
	.string	"spi_set_2bytes"
	.long	0x2cb
	.string	"spi_set_3bytes"
	.long	0x329
	.string	"calc_sincos"
	.long	0x383
	.string	"send_out_msg"
	.long	0x3b8
	.string	"spi_read_curr"
	.long	0x3ea
	.string	"action_adc_mux"
	.long	0x423
	.string	"rise_bias_ctrl"
	.long	0x45d
	.string	"down_bias_ctrl"
	.long	0x497
	.string	"action_lcd"
	.long	0x60a
	.string	"action_apd_off"
	.long	0x61e
	.string	"action_laser_off"
	.long	0x632
	.string	"action_laser_aim"
	.long	0x659
	.string	"action_curtain"
	.long	0x692
	.string	"action_signature"
	.long	0x6b5
	.string	"action_spi_wr"
	.long	0x6fa
	.string	"action_spi_rd"
	.long	0x73f
	.string	"action_measure"
	.long	0x768
	.string	"action_apd_on"
	.long	0x7bc
	.string	"gta_freq_table"
	.long	0x7eb
	.string	"gta_action_table"
	.long	0x80e
	.string	"gta_digit_decode"
	.long	0x831
	.string	"gta_digit_set0_map"
	.long	0x844
	.string	"gta_digit_set1_map"
	.long	0x867
	.string	"gta_signs_map"
	.long	0x87a
	.string	"guc_in_msg_idx"
	.long	0x8a1
	.string	"guca_in_msg_buff"
	.long	0x8b3
	.string	"guc_out_msg_wr_idx"
	.long	0x8c5
	.string	"guca_out_msg_buff"
	.long	0x8d7
	.string	"guc_adc_mux_hdr"
	.long	0x8e9
	.string	"guc_out_msg_rd_idx"
	.long	0x935
	.string	"guca_spi_curr_state"
	.long	0x0
	.section	.debug_aranges,"",@progbits
	.long	0x1c
	.word	0x2
	.long	.Ldebug_info0
	.byte	0x4
	.byte	0x0
	.word	0x0
	.word	0x0
	.long	.Ltext0
	.long	.Letext0-.Ltext0
	.long	0x0
	.long	0x0
	.section	.debug_ranges,"",@progbits
.Ldebug_ranges0:
	.long	.LBB2-.Ltext0
	.long	.LBE2-.Ltext0
	.long	.LBB6-.Ltext0
	.long	.LBE6-.Ltext0
	.long	0x0
	.long	0x0
	.section	.debug_line
	.long	.LELT0-.LSLT0
.LSLT0:
	.word	0x2
	.long	.LELTP0-.LASLTP0
.LASLTP0:
	.byte	0x1
	.byte	0x1
	.byte	0xf6
	.byte	0xf5
	.byte	0xa
	.byte	0x0
	.byte	0x1
	.byte	0x1
	.byte	0x1
	.byte	0x1
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.byte	0x1
	.ascii	".."
	.byte	0
	.ascii	"c:/program files/atmel/winavr-20071221/bin/../avr/include"
	.byte	0
	.byte	0x0
	.string	"dle_actions.c"
	.uleb128 0x1
	.uleb128 0x0
	.uleb128 0x0
	.string	"stdint.h"
	.uleb128 0x2
	.uleb128 0x0
	.uleb128 0x0
	.string	"dle_actions.h"
	.uleb128 0x1
	.uleb128 0x0
	.uleb128 0x0
	.string	"dle_ctrl.h"
	.uleb128 0x1
	.uleb128 0x0
	.uleb128 0x0
	.byte	0x0
.LELTP0:
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM1
	.byte	0x59
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM2
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM3
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM4
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM5
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM6
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM7
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM8
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM9
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM10
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM11
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM12
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM13
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM14
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM15
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM16
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM17
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM18
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM19
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM20
	.byte	0x1f
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM21
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM22
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM23
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM24
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM25
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM26
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM27
	.byte	0x11
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM28
	.byte	0x1b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM29
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM30
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM31
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM32
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM33
	.byte	0x12
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM34
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM35
	.byte	0x9b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM36
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM37
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM38
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM39
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM40
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM41
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM42
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM43
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM44
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM45
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM46
	.byte	0xfb
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM47
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM48
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM49
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM50
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM51
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM52
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM53
	.byte	0xb
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM54
	.byte	0x21
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM55
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM56
	.byte	0x1d
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM57
	.byte	0xb
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM58
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM59
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM60
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM61
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM62
	.byte	0x3
	.sleb128 352
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM63
	.byte	0x25
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM64
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM65
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM66
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM67
	.byte	0x12
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM68
	.byte	0x22
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM69
	.byte	0x27
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM70
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM71
	.byte	0x11
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM72
	.byte	0x20
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM73
	.byte	0xd
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM74
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM75
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM76
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM77
	.byte	0xc
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM78
	.byte	0x3
	.sleb128 -13
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM79
	.byte	0x2c
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM80
	.byte	0x3
	.sleb128 -38
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM81
	.byte	0x56
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM82
	.byte	0xb
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM83
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM84
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM85
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM86
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM87
	.byte	0xb
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM88
	.byte	0x25
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM89
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM90
	.byte	0x3
	.sleb128 -70
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM91
	.byte	0x5d
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM92
	.byte	0x3
	.sleb128 -322
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM93
	.byte	0x1e
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM94
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM95
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM96
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM97
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM98
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM99
	.byte	0x1b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM100
	.byte	0x1b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM101
	.byte	0x1d
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM102
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM103
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM104
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM105
	.byte	0x3
	.sleb128 -30
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM106
	.byte	0x21
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM107
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM108
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM109
	.byte	0x1b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM110
	.byte	0x3
	.sleb128 -16
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM111
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM112
	.byte	0x2e
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM113
	.byte	0x3
	.sleb128 -280
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM114
	.byte	0x1e
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM115
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM116
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM117
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM118
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM119
	.byte	0x1c
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM120
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM121
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM122
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM123
	.byte	0x3
	.sleb128 -19
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM124
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM125
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM126
	.byte	0x20
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM127
	.byte	0xe
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM128
	.byte	0x22
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM129
	.byte	0x3
	.sleb128 -135
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM130
	.byte	0x20
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM131
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM132
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM133
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM134
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM135
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM136
	.byte	0x20
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM137
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM138
	.byte	0x28
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM139
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM140
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM141
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM142
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM143
	.byte	0x1c
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM144
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM145
	.byte	0xd
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM146
	.byte	0x3
	.sleb128 -42
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM147
	.byte	0x1e
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM148
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM149
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM150
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM151
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM152
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM153
	.byte	0xc
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM154
	.byte	0xd
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM155
	.byte	0x3e
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM156
	.byte	0x3
	.sleb128 -166
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM157
	.byte	0x25
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM158
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM159
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM160
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM161
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM162
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM163
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM164
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM165
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM166
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM167
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM168
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM169
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM170
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM171
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM172
	.byte	0x23
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM173
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM174
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM175
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM176
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM177
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM178
	.byte	0x3
	.sleb128 -34
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM179
	.byte	0x29
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM180
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM181
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM182
	.byte	0x3
	.sleb128 -125
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM183
	.byte	0x1b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM184
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM185
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM186
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM187
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM188
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM189
	.byte	0x13
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM190
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM191
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM192
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM193
	.byte	0x3
	.sleb128 -76
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM194
	.byte	0x23
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM195
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM196
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM197
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM198
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM199
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM200
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM201
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM202
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM203
	.byte	0x12
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM204
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM205
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM206
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM207
	.byte	0xb
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM208
	.byte	0x20
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM209
	.byte	0x3
	.sleb128 -91
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM210
	.byte	0x23
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM211
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM212
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM213
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM214
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM215
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM216
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM217
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM218
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM219
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM220
	.byte	0x12
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM221
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM222
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM223
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM224
	.byte	0xb
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM225
	.byte	0x20
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM226
	.byte	0x3
	.sleb128 576
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM227
	.byte	0x23
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM228
	.byte	0x1d
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM229
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM230
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM231
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM232
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM233
	.byte	0x12
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM234
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM235
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM236
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM237
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM238
	.byte	0x1b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM239
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM240
	.byte	0x1b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM241
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM242
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM243
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM244
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM245
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM246
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM247
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM248
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM249
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM250
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM251
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM252
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM253
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM254
	.byte	0x1e
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM255
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM256
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM257
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM258
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM259
	.byte	0x22
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM260
	.byte	0x3
	.sleb128 -26
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM261
	.byte	0x34
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM262
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM263
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM264
	.byte	0x3
	.sleb128 -22
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM265
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM266
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM267
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM268
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM269
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM270
	.byte	0x3
	.sleb128 -64
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM271
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM272
	.byte	0x3
	.sleb128 -259
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM273
	.byte	0x21
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM274
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM275
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM276
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM277
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM278
	.byte	0x20
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM279
	.byte	0x1f
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM280
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM281
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM282
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM283
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM284
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM285
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM286
	.byte	0x1b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM287
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM288
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM289
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM290
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM291
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM292
	.byte	0x22
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM293
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM294
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM295
	.byte	0x1b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM296
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM297
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM298
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM299
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM300
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM301
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM302
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM303
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM304
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM305
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM306
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM307
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM308
	.byte	0x3
	.sleb128 -85
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM309
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM310
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM311
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM312
	.byte	0x3
	.sleb128 -16
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM313
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM314
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM315
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM316
	.byte	0x45
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM317
	.byte	0x42
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.Letext0
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
.LELT0:
	.section	.debug_str,"MS",@progbits,1
.LASF75:
	.string	"action_laser_off"
.LASF79:
	.string	"uc_act"
.LASF103:
	.string	"guc_240usec_timer"
.LASF95:
	.string	"gta_digit_set1_map"
.LASF43:
	.string	"spi_set_3bytes"
.LASF78:
	.string	"action_curtain"
.LASF31:
	.string	"T_DIGIT_MAP"
.LASF53:
	.string	"uc_i"
.LASF59:
	.string	"us_curr_bias"
.LASF61:
	.string	"down_bias_ctrl"
.LASF51:
	.string	"send_out_msg"
.LASF30:
	.string	"ta_seg_map"
.LASF34:
	.string	"s_cos"
.LASF19:
	.string	"ca_name"
.LASF15:
	.string	"ca_signature"
.LASF88:
	.string	"us_bias_ctrl"
.LASF21:
	.string	"pf_func"
.LASF18:
	.string	"HW_INFO"
.LASF107:
	.string	"GNU C 4.2.2 (WinAVR 20071221)"
.LASF68:
	.string	"uc_sign"
.LASF87:
	.string	"t_apd_res"
.LASF12:
	.string	"uint16"
.LASF35:
	.string	"us_pwr"
.LASF2:
	.string	"uint8_t"
.LASF11:
	.string	"int16"
.LASF10:
	.string	"uint8"
.LASF52:
	.string	"uc_len"
.LASF55:
	.string	"action_adc_mux"
.LASF76:
	.string	"action_laser_aim"
.LASF72:
	.string	"__addr16"
.LASF50:
	.string	"uc_rnd_const"
.LASF80:
	.string	"action_signature"
.LASF90:
	.string	"gta_freq_table"
.LASF101:
	.string	"guc_adc_mux_hdr"
.LASF89:
	.string	"us_bias_ctrl_curr"
.LASF8:
	.string	"long long int"
.LASF25:
	.string	"uc_addr"
.LASF98:
	.string	"guca_in_msg_buff"
.LASF22:
	.string	"T_ACTION"
.LASF6:
	.string	"long int"
.LASF36:
	.string	"T_FREQ_DATA"
.LASF20:
	.string	"uc_cmd"
.LASF108:
	.string	"../dle_actions.c"
.LASF47:
	.string	"us_sampl0"
.LASF48:
	.string	"us_sampl1"
.LASF66:
	.string	"pt_digit_map"
.LASF84:
	.string	"action_measure"
.LASF82:
	.string	"uc_spi_len"
.LASF56:
	.string	"uc_mux"
.LASF83:
	.string	"action_spi_rd"
.LASF65:
	.string	"uc_set"
.LASF57:
	.string	"uc_tmp"
.LASF1:
	.string	"unsigned char"
.LASF91:
	.string	"gt_hw_info"
.LASF0:
	.string	"signed char"
.LASF63:
	.string	"uc_digit_mask"
.LASF9:
	.string	"long long unsigned int"
.LASF102:
	.string	"guc_out_msg_rd_idx"
.LASF69:
	.string	"pt_sign_map"
.LASF5:
	.string	"unsigned int"
.LASF105:
	.string	"guca_spi_curr_state"
.LASF4:
	.string	"uint16_t"
.LASF13:
	.string	"uc_ver_maj"
.LASF73:
	.string	"__result"
.LASF38:
	.string	"spi_set_byte"
.LASF106:
	.string	"gusa_trace"
.LASF81:
	.string	"action_spi_wr"
.LASF17:
	.string	"char"
.LASF29:
	.string	"uc_digit_pos"
.LASF93:
	.string	"gta_digit_decode"
.LASF45:
	.string	"calc_sincos"
.LASF33:
	.string	"s_sin"
.LASF37:
	.string	"uc_data"
.LASF32:
	.string	"us_freq"
.LASF3:
	.string	"int16_t"
.LASF58:
	.string	"rise_bias_ctrl"
.LASF71:
	.string	"us_map"
.LASF70:
	.string	"pt_seg_map"
.LASF46:
	.string	"pt_meas"
.LASF14:
	.string	"uc_ver_min"
.LASF74:
	.string	"action_apd_off"
.LASF94:
	.string	"gta_digit_set0_map"
.LASF86:
	.string	"action_apd_on"
.LASF39:
	.string	"spi_set_2bytes"
.LASF7:
	.string	"long unsigned int"
.LASF100:
	.string	"guca_out_msg_buff"
.LASF104:
	.string	"gus_acc"
.LASF26:
	.string	"uc_mask"
.LASF42:
	.string	"uc_feedback"
.LASF64:
	.string	"uc_digit"
.LASF40:
	.string	"uc_data0"
.LASF41:
	.string	"uc_data1"
.LASF44:
	.string	"uc_data2"
.LASF27:
	.string	"T_SEG_MAP"
.LASF62:
	.string	"action_lcd"
.LASF109:
	.string	"C:\\\\MyWorks\\\\RangeFinder_rvrs\\\\FW\\\\DLE_CTRL\\\\default"
.LASF85:
	.string	"pt_freq"
.LASF97:
	.string	"guc_in_msg_idx"
.LASF96:
	.string	"gta_signs_map"
.LASF24:
	.string	"t_seg_map_tag"
.LASF28:
	.string	"lcd_digit_segment_map_tag"
.LASF23:
	.string	"action_tag"
.LASF99:
	.string	"guc_out_msg_wr_idx"
.LASF77:
	.string	"uc_action"
.LASF92:
	.string	"gta_action_table"
.LASF49:
	.string	"uc_scale"
.LASF16:
	.string	"reserved"
.LASF60:
	.string	"us_final_bias"
.LASF54:
	.string	"spi_read_curr"
.LASF67:
	.string	"puc_inp"
/* File "../dle_actions.c": code 1563 = 0x061b (1480), prologues  34, epilogues  49 */
