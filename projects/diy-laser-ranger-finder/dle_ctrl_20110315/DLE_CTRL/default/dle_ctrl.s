	.file	"dle_ctrl.c"
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
.global	activate_adc_isr
	.type	activate_adc_isr, @function
activate_adc_isr:
.LFB8:
.LM1:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.LVL0:
.LM2:
	ldi r25,lo8(16)
	sts guc_delay_cnt,r25
.LM3:
	ldi r25,lo8(64)
	sts guc_measure_cnt,r25
.LM4:
	ldi r25,lo8(1)
	sts guc_adc_state,r25
.LM5:
	sts guc_adc_stream_en,r24
.LM6:
	ldi r30,lo8(122)
	ldi r31,hi8(122)
	ld r24,Z
.LVL1:
	ori r24,lo8(-112)
	st Z,r24
.LM7:
	ldi r24,lo8(4)
	out 54-0x20,r24
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function activate_adc_isr size 19 (18) */
.LFE8:
	.size	activate_adc_isr, .-activate_adc_isr
.global	init_apd_bias_pwm
	.type	init_apd_bias_pwm, @function
init_apd_bias_pwm:
.LFB9:
.LM8:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.LM9:
	ldi r24,lo8(-17)
	sts 179,r24
.LM10:
	ldi r24,lo8(2)
	sts 112,r24
.LM11:
	out 55-0x20,r24
.LM12:
	ldi r24,lo8(9)
	sts 176,r24
.LM13:
	sbi 37-0x20,7
.LM14:
	sbi 36-0x20,7
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function init_apd_bias_pwm size 13 (12) */
.LFE9:
	.size	init_apd_bias_pwm, .-init_apd_bias_pwm
.global	init_uart
	.type	init_uart, @function
init_uart:
.LFB10:
.LM15:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.LM16:
	ldi r30,lo8(100)
	ldi r31,hi8(100)
	ld r24,Z
	andi r24,lo8(-3)
	st Z,r24
.LM17:
	sts 197,__zero_reg__
.LM18:
	sts 196,__zero_reg__
.LM19:
	ldi r24,lo8(6)
	sts 194,r24
.LM20:
	ldi r24,lo8(-104)
	sts 193,r24
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function init_uart size 16 (15) */
.LFE10:
	.size	init_uart, .-init_uart
.global	init_lcd
	.type	init_lcd, @function
init_lcd:
.LFB11:
.LM21:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.LM22:
	ldi r30,lo8(228)
	ldi r31,hi8(228)
	st Z,__zero_reg__
.LM23:
	ldi r24,lo8(101)
	sts 230,r24
.LM24:
	ldi r24,lo8(16)
	sts 231,r24
.LM25:
	ldi r24,lo8(55)
	sts 229,r24
.LM26:
	ldi r24,lo8(-128)
	st Z,r24
.LM27:
	sts 238,r24
.LM28:
	ldi r24,lo8(-96)
	sts 241,r24
.LM29:
	ldi r24,lo8(10)
	sts 246,r24
.LM30:
	ldi r24,lo8(2)
	sts 247,r24
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function init_lcd size 26 (25) */
.LFE11:
	.size	init_lcd, .-init_lcd
.global	init_spi
	.type	init_spi, @function
init_spi:
.LFB12:
.LM31:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.LM32:
	in r24,45-0x20
	ori r24,lo8(88)
	out 45-0x20,r24
.LM33:
	cbi 45-0x20,5
.LM34:
	in r24,46-0x20
	ori r24,lo8(40)
	out 46-0x20,r24
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function init_spi size 8 (7) */
.LFE12:
	.size	init_spi, .-init_spi
.global	init_adc
	.type	init_adc, @function
init_adc:
.LFB13:
.LM35:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.LM36:
	ldi r26,lo8(122)
	ldi r27,hi8(122)
	st X,__zero_reg__
.LM37:
	ldi r30,lo8(100)
	ldi r31,hi8(100)
	ld r24,Z
	andi r24,lo8(-2)
	st Z,r24
.LM38:
	ldi r24,lo8(5)
	sts 123,r24
.LM39:
	ldi r24,lo8(15)
	sts 126,r24
.LM40:
	sts 124,__zero_reg__
.LM41:
	ldi r24,lo8(125)
	st X,r24
.LM42:
	sts 128,__zero_reg__
.LM43:
	ldi r24,lo8(9)
	sts 129,r24
.LM44:
	ldi r24,lo8(1)
	sts 137,r24
.LM45:
	ldi r24,lo8(-33)
	sts 136,r24
.LM46:
	sts 139,__zero_reg__
.LM47:
	ldi r24,lo8(64)
	sts 138,r24
.LM48:
	ldi r24,lo8(4)
	sts 111,r24
/* epilogue: frame size=0 */
	ret
/* epilogue end (size=1) */
/* function init_adc size 38 (37) */
.LFE13:
	.size	init_adc, .-init_adc
.global	FATAL_TRAP
	.type	FATAL_TRAP, @function
FATAL_TRAP:
.LFB15:
.LM49:
/* prologue: frame size=0 */
/* prologue end (size=0) */
.LVL2:
.LM50:
	sts (gus_trap_line)+1,r25
	sts gus_trap_line,r24
.L14:
	rjmp .L14
/* epilogue: frame size=0 */
/* epilogue: noreturn */
/* epilogue end (size=0) */
/* function FATAL_TRAP size 5 (5) */
.LFE15:
	.size	FATAL_TRAP, .-FATAL_TRAP
.global	command_lookup
	.type	command_lookup, @function
command_lookup:
.LFB16:
.LM51:
/* prologue: frame size=0 */
	push r16
	push r17
	push r28
	push r29
/* prologue end (size=4) */
.LM52:
	lds r24,guca_in_msg_buff
	cpi r24,lo8(-43)
	breq .L18
.LM53:
	ldi r24,lo8(420)
	ldi r25,hi8(420)
	call FATAL_TRAP
.L18:
.LM54:
	lds r16,guca_in_msg_buff+1
.LVL3:
.LBB2:
.LM55:
	ldi r28,lo8(gta_action_table+5)
	ldi r29,hi8(gta_action_table+5)
	movw r30,r28
.LVL4:
/* #APP */
	lpm r24, Z+
	lpm r25, Z
	
.LVL5:
/* #NOAPP */
.LBE2:
.LM56:
	subi r24,lo8(-275)
	sbci r25,hi8(-275)
	breq .L20
.LM57:
	ldi r24,lo8(432)
	ldi r25,hi8(432)
.LVL6:
	call FATAL_TRAP
.LVL7:
.L20:
	sbiw r28,5
.LVL8:
	rjmp .L22
.LVL9:
.L23:
.LM58:
	cp r16,r17
	breq .L29
.LVL10:
.L22:
.LM59:
	adiw r28,7
.LBB3:
.LM60:
	adiw r28,4
	movw r30,r28
.LVL11:
/* #APP */
	lpm r17, Z
	
/* #NOAPP */
	sbiw r28,4
.LBE3:
.LM61:
	cpi r17,lo8(-1)
	brne .L23
.LM62:
	ldi r24,lo8(441)
	ldi r25,hi8(441)
.LVL12:
	call FATAL_TRAP
.LM63:
	cp r16,r17
	brne .L22
.LVL13:
.L29:
.LBB4:
.LM64:
	movw r30,r28
.LVL14:
	adiw r30,5
/* #APP */
	lpm r24, Z+
	lpm r25, Z
	
.LVL15:
/* #NOAPP */
.LBE4:
	sts (gpf_action_func)+1,r25
	sts gpf_action_func,r24
.LM65:
	movw r30,r24
	icall
.LVL16:
.LM66:
	sts guc_in_msg_idx,__zero_reg__
/* epilogue: frame size=0 */
	pop r29
	pop r28
	pop r17
	pop r16
	ret
/* epilogue end (size=5) */
/* function command_lookup size 72 (63) */
.LFE16:
	.size	command_lookup, .-command_lookup
.global	main
	.type	main, @function
main:
.LFB14:
.LM67:
/* prologue: frame size=0 */
/* prologue end (size=0) */
	ldi r24,lo8(0)
.LVL17:
.L31:
.LBB5:
.LM68:
/* #APP */
	    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

.LM69:
/* #NOAPP */
	subi r24,lo8(-(1))
	cpi r24,lo8(-1)
	brne .L31
.LBE5:
.LM70:
	sbi 46-0x20,2
.LM71:
	cbi 45-0x20,2
.LM72:
	in r24,34-0x20
.LVL18:
	ori r24,lo8(12)
	out 34-0x20,r24
.LM73:
	in r24,33-0x20
	ori r24,lo8(12)
	out 33-0x20,r24
.LM74:
	sts guc_30usec_cnt,__zero_reg__
.LM75:
	sts (gusa_trace)+1,__zero_reg__
	sts gusa_trace,__zero_reg__
.LM76:
	sts (gusa_trace+2)+1,__zero_reg__
	sts gusa_trace+2,__zero_reg__
.LM77:
	sts (gusa_trace+4)+1,__zero_reg__
	sts gusa_trace+4,__zero_reg__
.LM78:
	sts (gusa_trace+6)+1,__zero_reg__
	sts gusa_trace+6,__zero_reg__
.LM79:
	sts (gusa_trace+8)+1,__zero_reg__
	sts gusa_trace+8,__zero_reg__
.LM80:
	sts guc_dbg_buff_idx,__zero_reg__
.LM81:
	call init_apd_bias_pwm
.LM82:
	call init_uart
.LM83:
	call init_spi
.LM84:
	call init_lcd
.LM85:
	call init_adc
.LM86:
	ldi r24,lo8(-125)
	out 67-0x20,r24
.LM87:
	sts 133,__zero_reg__
.LM88:
	sts 132,__zero_reg__
.LM89:
	sts 178,__zero_reg__
.LM90:
	out 67-0x20,__zero_reg__
.LM91:
	sts (gus_trap_line)+1,__zero_reg__
	sts gus_trap_line,__zero_reg__
.LM92:
/* #APP */
	sei
/* #NOAPP */
.L44:
.LM93:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(2)
	brlo .L34
.L45:
.LM94:
	sts guc_out_msg_wr_idx,__zero_reg__
.LM95:
	sts guc_out_msg_rd_idx,__zero_reg__
.LM96:
	call command_lookup
.L34:
.LM97:
	lds r25,guc_out_msg_wr_idx
	lds r24,guc_out_msg_rd_idx
	cp r24,r25
	brsh .L44
.LM98:
	lds r24,192
	sbrs r24,5
	rjmp .L44
.LM99:
	lds r30,guc_out_msg_rd_idx
	ldi r31,lo8(0)
	subi r30,lo8(-(guca_out_msg_buff))
	sbci r31,hi8(-(guca_out_msg_buff))
	ld r24,Z
	sts 198,r24
.LM100:
	lds r24,192
	ori r24,lo8(64)
	sts 192,r24
.LM101:
	lds r24,guc_out_msg_rd_idx
	subi r24,lo8(-(1))
	sts guc_out_msg_rd_idx,r24
.LM102:
	lds r24,guc_in_msg_idx
	cpi r24,lo8(2)
	brsh .L45
	rjmp .L34
/* epilogue: frame size=0 */
/* epilogue: noreturn */
/* epilogue end (size=0) */
/* function main size 138 (138) */
.LFE14:
	.size	main, .-main
.global	__vector_13
	.type	__vector_13, @function
__vector_13:
.LFB17:
.LM103:
/* prologue: frame size=0 */
	push __zero_reg__
	push __tmp_reg__
	in __tmp_reg__,__SREG__
	push __tmp_reg__
	clr __zero_reg__
	push r24
	push r25
	push r30
	push r31
/* prologue end (size=9) */
.LM104:
	lds r24,192
.LVL19:
.LM105:
	lds r25,198
.LVL20:
.LM106:
	andi r24,lo8(24)
	breq .L47
.LM107:
	ldi r24,lo8(470)
	ldi r25,hi8(470)
.LVL21:
	sts (gus_trap_line)+1,r25
	sts gus_trap_line,r24
.L49:
	rjmp .L49
.LVL22:
.L47:
.LM108:
	lds r24,guc_in_msg_idx
.LVL23:
	cpi r24,lo8(-128)
	brne .L50
.LM109:
	ldi r24,lo8(473)
	ldi r25,hi8(473)
	sts (gus_trap_line)+1,r25
	sts gus_trap_line,r24
.L52:
	rjmp .L52
.L50:
.LM110:
	lds r24,guc_in_msg_idx
	mov r30,r24
	ldi r31,lo8(0)
	subi r30,lo8(-(guca_in_msg_buff))
	sbci r31,hi8(-(guca_in_msg_buff))
	st Z,r25
	subi r24,lo8(-(1))
	sts guc_in_msg_idx,r24
/* epilogue: frame size=0 */
	pop r31
	pop r30
	pop r25
	pop r24
	pop __tmp_reg__
	out __SREG__,__tmp_reg__
	pop __tmp_reg__
	pop __zero_reg__
	reti
/* epilogue end (size=9) */
/* function __vector_13 size 53 (35) */
.LFE17:
	.size	__vector_13, .-__vector_13
.global	__vector_4
	.type	__vector_4, @function
__vector_4:
.LFB18:
.LM111:
/* prologue: frame size=0 */
	push __zero_reg__
	push __tmp_reg__
	in __tmp_reg__,__SREG__
	push __tmp_reg__
	clr __zero_reg__
	push r24
	push r25
/* prologue end (size=7) */
.LM112:
	cbi 37-0x20,7
.LM113:
/* #APP */
	    nop
    nop
    nop
    nop

.LM114:
	    nop
    nop
    nop

.LM115:
/* #NOAPP */
	sbi 37-0x20,7
.LM116:
	lds r24,guc_30usec_cnt
	subi r24,lo8(-(1))
	sts guc_30usec_cnt,r24
.LM117:
	ldi r25,lo8(0)
	andi r24,lo8(7)
	andi r25,hi8(7)
	or r24,r25
	brne .L57
.LM118:
	lds r24,guc_240usec_timer
	subi r24,lo8(-(-1))
	sts guc_240usec_timer,r24
.L57:
/* epilogue: frame size=0 */
	pop r25
	pop r24
	pop __tmp_reg__
	out __SREG__,__tmp_reg__
	pop __tmp_reg__
	pop __zero_reg__
	reti
/* epilogue end (size=7) */
/* function __vector_4 size 49 (35) */
.LFE18:
	.size	__vector_4, .-__vector_4
.global	__vector_19
	.type	__vector_19, @function
__vector_19:
.LFB19:
.LM119:
/* prologue: frame size=0 */
	push __zero_reg__
	push __tmp_reg__
	in __tmp_reg__,__SREG__
	push __tmp_reg__
	clr __zero_reg__
	push r18
	push r19
	push r20
	push r21
	push r24
	push r25
	push r30
	push r31
/* prologue end (size=13) */
.LM120:
	lds r20,120
.LVL24:
.LM121:
	lds r21,121
.LVL25:
.LM122:
	ldi r24,lo8(4)
	out 54-0x20,r24
.LM123:
	lds r24,guc_adc_state
	cpi r24,lo8(1)
	breq .L76
.LM124:
	lds r24,guc_adc_state
	cpi r24,lo8(3)
	breq .L77
.LM125:
	lds r24,guc_adc_state
	cpi r24,lo8(2)
	breq .+2
	rjmp .L75
.LM126:
	lds r24,guc_measure_cnt
	tst r24
	brne .+2
	rjmp .L78
.LM127:
	lds r24,guc_delay_cnt
	tst r24
	brne .+2
	rjmp .L67
.LM128:
	lds r24,guc_delay_cnt
	subi r24,lo8(-(-1))
	sts guc_delay_cnt,r24
	rjmp .L75
.L76:
.LM129:
	ldi r24,lo8(3)
	sts guc_adc_state,r24
	rjmp .L75
.L77:
.LM130:
	ldi r24,lo8(2)
	sts guc_adc_state,r24
.LM131:
	cbi 46-0x20,3
.LM132:
	ldi r24,lo8(6)
	sts 186,r24
	ldi r18,lo8(17)
	sts 184,r18
	ldi r24,lo8(19)
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	lds r25,186
.LVL26:
.LM133:
	ldi r25,lo8(-11)
.LVL27:
	sts 186,r25
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	lds r24,186
.LVL28:
.LM134:
	sbi 46-0x20,3
.LM135:
/* #APP */
	    nop
    nop
    nop
    nop

.LM136:
	    nop
    nop
    nop
    nop

.LM137:
	    nop
    nop
    nop
    nop

.LM138:
	    nop
    nop
    nop
    nop

.LM139:
	    nop
    nop
    nop
    nop

.LM140:
	    nop
    nop
    nop
    nop

.LM141:
	    nop
    nop
    nop
    nop

.LM142:
	    nop
    nop
    nop
    nop

.LM143:
/* #NOAPP */
	cbi 46-0x20,3
.LM144:
	ldi r24,lo8(6)
.LVL29:
	sts 186,r24
	sts 184,r18
	ldi r24,lo8(19)
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	lds r25,186
.LVL30:
.LM145:
	ldi r25,lo8(-12)
.LVL31:
	sts 186,r25
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	sts 184,r18
	sts 184,r24
	lds r24,186
.LVL32:
.LM146:
	sbi 46-0x20,3
.LM147:
	sts (gus_acc)+1,__zero_reg__
	sts gus_acc,__zero_reg__
.LM148:
	sts (gus_acc+2)+1,__zero_reg__
	sts gus_acc+2,__zero_reg__
.LM149:
	sts (gus_acc+4)+1,__zero_reg__
	sts gus_acc+4,__zero_reg__
.LM150:
	sts (gus_acc+6)+1,__zero_reg__
	sts gus_acc+6,__zero_reg__
.LM151:
	lds r24,gusa_trace+4
	lds r25,(gusa_trace+4)+1
.LVL33:
	adiw r24,1
	sts (gusa_trace+4)+1,r25
	sts gusa_trace+4,r24
	rjmp .L75
.LVL34:
.L78:
.LM152:
	lds r24,122
	andi r24,lo8(127)
	sts 122,r24
	rjmp .L75
.L67:
.LM153:
	lds r24,guc_measure_cnt
	ldi r30,lo8(4)
	ldi r31,hi8(4)
	sub r30,r24
	sbc r31,__zero_reg__
	andi r30,lo8(3)
	andi r31,hi8(3)
	lsl r30
	rol r31
	subi r30,lo8(-(gus_acc))
	sbci r31,hi8(-(gus_acc))
	mov r24,r21
	ldi r25,lo8(0)
.LVL35:
	mov r25,r24
	clr r24
	mov r18,r20
	ldi r19,lo8(0)
.LVL36:
	or r24,r18
	or r25,r19
	ld r18,Z
	ldd r19,Z+1
	add r24,r18
	adc r25,r19
	std Z+1,r25
	st Z,r24
.LM154:
	lds r24,guc_adc_stream_en
	tst r24
	breq .L69
.LM155:
	lds r24,192
	sbrs r24,6
	rjmp .L71
.LM156:
	sts 198,r20
.LM157:
	sts 198,r21
.LM158:
	lds r24,192
	ori r24,lo8(64)
	sts 192,r24
.L69:
.LM159:
	lds r24,guc_measure_cnt
	ldi r25,lo8(0)
.LVL37:
	andi r24,lo8(3)
	andi r25,hi8(3)
	or r24,r25
	brne .L73
.LM160:
	in r24,34-0x20
	ldi r25,lo8(8)
	eor r24,r25
	out 34-0x20,r24
.L73:
.LM161:
	lds r24,guc_measure_cnt
	subi r24,lo8(-(-1))
	sts guc_measure_cnt,r24
	rjmp .L75
.LVL38:
.L71:
.LM162:
	lds r24,gusa_trace+2
	lds r25,(gusa_trace+2)+1
	adiw r24,1
	sts (gusa_trace+2)+1,r25
	sts gusa_trace+2,r24
	rjmp .L69
.LVL39:
.L75:
/* epilogue: frame size=0 */
	pop r31
	pop r30
	pop r25
	pop r24
	pop r21
	pop r20
	pop r19
	pop r18
	pop __tmp_reg__
	out __SREG__,__tmp_reg__
	pop __tmp_reg__
	pop __zero_reg__
	reti
/* epilogue end (size=13) */
/* function __vector_19 size 405 (379) */
.LFE19:
	.size	__vector_19, .-__vector_19
.global	__vector_8
	.type	__vector_8, @function
__vector_8:
.LFB20:
.LM163:
/* prologue: frame size=0 */
	push __zero_reg__
	push __tmp_reg__
	in __tmp_reg__,__SREG__
	push __tmp_reg__
	clr __zero_reg__
/* prologue end (size=5) */
/* epilogue: frame size=0 */
	pop __tmp_reg__
	out __SREG__,__tmp_reg__
	pop __tmp_reg__
	pop __zero_reg__
	reti
/* epilogue end (size=5) */
/* function __vector_8 size 10 (0) */
.LFE20:
	.size	__vector_8, .-__vector_8
.global	gt_hw_info
	.data
	.type	gt_hw_info, @object
	.size	gt_hw_info, 102
gt_hw_info:
	.byte	0
	.byte	1
	.string	"AV DLE Control"
	.skip 1,0
	.skip 84,0
	.comm guc_measure_cnt,1,1
	.comm guc_adc_state,1,1
	.comm guc_adc_stream_en,1,1
	.comm guc_30usec_cnt,1,1
	.comm guc_240usec_timer,1,1
	.comm gus_acc,8,1
	.comm guca_dbg_buff,256,1
	.comm guc_dbg_buff_idx,1,1
	.comm gus_trap_line,2,1
	.comm gusa_trace,32,1
	.comm gpf_action_func,2,1
	.comm guc_delay_cnt,1,1
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
	.long	.LFB8
	.long	.LFE8-.LFB8
	.p2align	2
.LEFDE0:
.LSFDE2:
	.long	.LEFDE2-.LASFDE2
.LASFDE2:
	.long	.Lframe0
	.long	.LFB9
	.long	.LFE9-.LFB9
	.p2align	2
.LEFDE2:
.LSFDE4:
	.long	.LEFDE4-.LASFDE4
.LASFDE4:
	.long	.Lframe0
	.long	.LFB10
	.long	.LFE10-.LFB10
	.p2align	2
.LEFDE4:
.LSFDE6:
	.long	.LEFDE6-.LASFDE6
.LASFDE6:
	.long	.Lframe0
	.long	.LFB11
	.long	.LFE11-.LFB11
	.p2align	2
.LEFDE6:
.LSFDE8:
	.long	.LEFDE8-.LASFDE8
.LASFDE8:
	.long	.Lframe0
	.long	.LFB12
	.long	.LFE12-.LFB12
	.p2align	2
.LEFDE8:
.LSFDE10:
	.long	.LEFDE10-.LASFDE10
.LASFDE10:
	.long	.Lframe0
	.long	.LFB13
	.long	.LFE13-.LFB13
	.p2align	2
.LEFDE10:
.LSFDE12:
	.long	.LEFDE12-.LASFDE12
.LASFDE12:
	.long	.Lframe0
	.long	.LFB15
	.long	.LFE15-.LFB15
	.p2align	2
.LEFDE12:
.LSFDE14:
	.long	.LEFDE14-.LASFDE14
.LASFDE14:
	.long	.Lframe0
	.long	.LFB16
	.long	.LFE16-.LFB16
	.p2align	2
.LEFDE14:
.LSFDE16:
	.long	.LEFDE16-.LASFDE16
.LASFDE16:
	.long	.Lframe0
	.long	.LFB14
	.long	.LFE14-.LFB14
	.p2align	2
.LEFDE16:
.LSFDE18:
	.long	.LEFDE18-.LASFDE18
.LASFDE18:
	.long	.Lframe0
	.long	.LFB17
	.long	.LFE17-.LFB17
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
	.long	.LFB19
	.long	.LFE19-.LFB19
	.p2align	2
.LEFDE22:
.LSFDE24:
	.long	.LEFDE24-.LASFDE24
.LASFDE24:
	.long	.Lframe0
	.long	.LFB20
	.long	.LFE20-.LFB20
	.p2align	2
.LEFDE24:
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
.LLST9:
	.long	.LVL4-.Ltext0
	.long	.LVL7-.Ltext0
	.word	0x6
	.byte	0x6e
	.byte	0x93
	.uleb128 0x1
	.byte	0x6f
	.byte	0x93
	.uleb128 0x1
	.long	.LVL7-.Ltext0
	.long	.LVL9-.Ltext0
	.word	0x6
	.byte	0x6e
	.byte	0x93
	.uleb128 0x1
	.byte	0x6f
	.byte	0x93
	.uleb128 0x1
	.long	.LVL10-.Ltext0
	.long	.LVL11-.Ltext0
	.word	0x6
	.byte	0x6e
	.byte	0x93
	.uleb128 0x1
	.byte	0x6f
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
.LLST10:
	.long	.LVL5-.Ltext0
	.long	.LVL6-.Ltext0
	.word	0x6
	.byte	0x68
	.byte	0x93
	.uleb128 0x1
	.byte	0x69
	.byte	0x93
	.uleb128 0x1
	.long	.LVL7-.Ltext0
	.long	.LVL12-.Ltext0
	.word	0x6
	.byte	0x68
	.byte	0x93
	.uleb128 0x1
	.byte	0x69
	.byte	0x93
	.uleb128 0x1
	.long	.LVL13-.Ltext0
	.long	.LVL15-.Ltext0
	.word	0x6
	.byte	0x68
	.byte	0x93
	.uleb128 0x1
	.byte	0x69
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
.LLST11:
	.long	.LVL14-.Ltext0
	.long	.LVL16-.Ltext0
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
	.long	.LVL15-.Ltext0
	.long	.LVL16-.Ltext0
	.word	0x6
	.byte	0x68
	.byte	0x93
	.uleb128 0x1
	.byte	0x69
	.byte	0x93
	.uleb128 0x1
	.long	0x0
	.long	0x0
.LLST14:
	.long	.LVL17-.Ltext0
	.long	.LVL18-.Ltext0
	.word	0x1
	.byte	0x68
	.long	0x0
	.long	0x0
.LLST16:
	.long	.LVL19-.Ltext0
	.long	.LVL21-.Ltext0
	.word	0x1
	.byte	0x68
	.long	.LVL22-.Ltext0
	.long	.LVL23-.Ltext0
	.word	0x1
	.byte	0x68
	.long	0x0
	.long	0x0
.LLST19:
	.long	.LVL25-.Ltext0
	.long	.LVL35-.Ltext0
	.word	0x1
	.byte	0x65
	.long	.LVL35-.Ltext0
	.long	.LVL37-.Ltext0
	.word	0x6
	.byte	0x68
	.byte	0x93
	.uleb128 0x1
	.byte	0x69
	.byte	0x93
	.uleb128 0x1
	.long	.LVL37-.Ltext0
	.long	.LVL38-.Ltext0
	.word	0x1
	.byte	0x65
	.long	.LVL38-.Ltext0
	.long	.LVL39-.Ltext0
	.word	0x6
	.byte	0x65
	.byte	0x93
	.uleb128 0x1
	.byte	0x69
	.byte	0x93
	.uleb128 0x1
	.long	.LVL39-.Ltext0
	.long	.LFE19-.Ltext0
	.word	0x1
	.byte	0x65
	.long	0x0
	.long	0x0
.LLST20:
	.long	.LVL24-.Ltext0
	.long	.LVL26-.Ltext0
	.word	0x1
	.byte	0x64
	.long	.LVL26-.Ltext0
	.long	.LVL27-.Ltext0
	.word	0x1
	.byte	0x69
	.long	.LVL28-.Ltext0
	.long	.LVL29-.Ltext0
	.word	0x1
	.byte	0x68
	.long	.LVL30-.Ltext0
	.long	.LVL31-.Ltext0
	.word	0x1
	.byte	0x69
	.long	.LVL32-.Ltext0
	.long	.LVL33-.Ltext0
	.word	0x1
	.byte	0x68
	.long	.LVL34-.Ltext0
	.long	.LVL36-.Ltext0
	.word	0x1
	.byte	0x64
	.long	.LVL36-.Ltext0
	.long	.LFE19-.Ltext0
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
	.long	0x558
	.word	0x2
	.long	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.long	.LASF65
	.byte	0x1
	.long	.LASF66
	.long	.LASF67
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
	.uleb128 0x4
	.byte	0x2
	.byte	0x5
	.string	"int"
	.uleb128 0x3
	.long	.LASF3
	.byte	0x2
	.byte	0x7c
	.long	0x50
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.long	.LASF4
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.long	.LASF5
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF6
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.long	.LASF7
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.long	.LASF8
	.uleb128 0x5
	.byte	0x1
	.byte	0x8
	.uleb128 0x3
	.long	.LASF9
	.byte	0x3
	.byte	0x19
	.long	0x2c
	.uleb128 0x3
	.long	.LASF10
	.byte	0x3
	.byte	0x1b
	.long	0x45
	.uleb128 0x3
	.long	.LASF11
	.byte	0x3
	.byte	0x21
	.long	0x97
	.uleb128 0x6
	.byte	0x2
	.long	0x9d
	.uleb128 0x7
	.long	0xa4
	.uleb128 0x8
	.byte	0x0
	.uleb128 0x9
	.byte	0x66
	.byte	0x3
	.byte	0x29
	.long	0xe5
	.uleb128 0xa
	.long	.LASF12
	.byte	0x3
	.byte	0x2a
	.long	0x76
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xa
	.long	.LASF13
	.byte	0x3
	.byte	0x2b
	.long	0x76
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0xa
	.long	.LASF14
	.byte	0x3
	.byte	0x2c
	.long	0xe5
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.uleb128 0xa
	.long	.LASF15
	.byte	0x3
	.byte	0x2d
	.long	0xff
	.byte	0x2
	.byte	0x23
	.uleb128 0x12
	.byte	0x0
	.uleb128 0xb
	.long	0xf8
	.long	0xf5
	.uleb128 0xc
	.long	0xf5
	.byte	0xf
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.byte	0x7
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.long	.LASF16
	.uleb128 0xb
	.long	0x76
	.long	0x10f
	.uleb128 0xc
	.long	0xf5
	.byte	0x53
	.byte	0x0
	.uleb128 0x3
	.long	.LASF17
	.byte	0x3
	.byte	0x2e
	.long	0xa4
	.uleb128 0xd
	.long	.LASF68
	.byte	0x7
	.byte	0x3
	.byte	0x3d
	.long	0x151
	.uleb128 0xa
	.long	.LASF18
	.byte	0x3
	.byte	0x3e
	.long	0x151
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xa
	.long	.LASF19
	.byte	0x3
	.byte	0x3f
	.long	0x76
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0xa
	.long	.LASF20
	.byte	0x3
	.byte	0x40
	.long	0x163
	.byte	0x2
	.byte	0x23
	.uleb128 0x5
	.byte	0x0
	.uleb128 0xb
	.long	0xf8
	.long	0x161
	.uleb128 0xc
	.long	0xf5
	.byte	0x3
	.byte	0x0
	.uleb128 0xe
	.byte	0x1
	.uleb128 0x6
	.byte	0x2
	.long	0x161
	.uleb128 0x3
	.long	.LASF21
	.byte	0x3
	.byte	0x41
	.long	0x11a
	.uleb128 0xf
	.byte	0x1
	.long	.LASF27
	.byte	0x1
	.byte	0x62
	.byte	0x1
	.long	.LFB8
	.long	.LFE8
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x19c
	.uleb128 0x10
	.long	.LASF29
	.byte	0x1
	.byte	0x62
	.long	0x76
	.long	.LLST1
	.byte	0x0
	.uleb128 0x11
	.byte	0x1
	.long	.LASF22
	.byte	0x1
	.byte	0x72
	.long	.LFB9
	.long	.LFE9
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.uleb128 0x11
	.byte	0x1
	.long	.LASF23
	.byte	0x1
	.byte	0x88
	.long	.LFB10
	.long	.LFE10
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.uleb128 0x11
	.byte	0x1
	.long	.LASF24
	.byte	0x1
	.byte	0xa3
	.long	.LFB11
	.long	.LFE11
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.uleb128 0x11
	.byte	0x1
	.long	.LASF25
	.byte	0x1
	.byte	0xc7
	.long	.LFB12
	.long	.LFE12
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.uleb128 0x11
	.byte	0x1
	.long	.LASF26
	.byte	0x1
	.byte	0xdc
	.long	.LFB13
	.long	.LFE13
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.uleb128 0x12
	.byte	0x1
	.long	.LASF28
	.byte	0x1
	.word	0x18e
	.byte	0x1
	.long	.LFB15
	.long	.LFE15
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x228
	.uleb128 0x13
	.long	.LASF30
	.byte	0x1
	.word	0x18e
	.long	0x81
	.byte	0x6
	.byte	0x68
	.byte	0x93
	.uleb128 0x1
	.byte	0x69
	.byte	0x93
	.uleb128 0x1
	.byte	0x0
	.uleb128 0x14
	.byte	0x1
	.long	.LASF69
	.byte	0x1
	.word	0x199
	.long	.LFB16
	.long	.LFE16
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x2fa
	.uleb128 0x15
	.long	.LASF32
	.byte	0x1
	.word	0x19b
	.long	0x2fa
	.byte	0x6
	.byte	0x6c
	.byte	0x93
	.uleb128 0x1
	.byte	0x6d
	.byte	0x93
	.uleb128 0x1
	.uleb128 0x16
	.long	.LASF20
	.byte	0x1
	.word	0x19c
	.long	0x8c
	.uleb128 0x16
	.long	.LASF31
	.byte	0x1
	.word	0x19e
	.long	0x76
	.uleb128 0x15
	.long	.LASF33
	.byte	0x1
	.word	0x19f
	.long	0x76
	.byte	0x1
	.byte	0x60
	.uleb128 0x17
	.long	.LBB2
	.long	.LBE2
	.long	0x2a7
	.uleb128 0x18
	.long	.LASF34
	.byte	0x1
	.word	0x1ae
	.long	0x45
	.long	.LLST9
	.uleb128 0x18
	.long	.LASF35
	.byte	0x1
	.word	0x1ae
	.long	0x45
	.long	.LLST10
	.byte	0x0
	.uleb128 0x17
	.long	.LBB3
	.long	.LBE3
	.long	0x2cf
	.uleb128 0x16
	.long	.LASF34
	.byte	0x1
	.word	0x1b5
	.long	0x45
	.uleb128 0x15
	.long	.LASF35
	.byte	0x1
	.word	0x1b5
	.long	0x2c
	.byte	0x1
	.byte	0x61
	.byte	0x0
	.uleb128 0x19
	.long	.LBB4
	.long	.LBE4
	.uleb128 0x18
	.long	.LASF34
	.byte	0x1
	.word	0x1bd
	.long	0x45
	.long	.LLST11
	.uleb128 0x18
	.long	.LASF35
	.byte	0x1
	.word	0x1bd
	.long	0x45
	.long	.LLST12
	.byte	0x0
	.byte	0x0
	.uleb128 0x6
	.byte	0x2
	.long	0x169
	.uleb128 0x1a
	.byte	0x1
	.long	.LASF36
	.byte	0x1
	.word	0x11e
	.long	0x3e
	.long	.LFB14
	.long	.LFE14
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x337
	.uleb128 0x19
	.long	.LBB5
	.long	.LBE5
	.uleb128 0x18
	.long	.LASF37
	.byte	0x1
	.word	0x123
	.long	0x76
	.long	.LLST14
	.byte	0x0
	.byte	0x0
	.uleb128 0x12
	.byte	0x1
	.long	.LASF38
	.byte	0x1
	.word	0x1cb
	.byte	0x1
	.long	.LFB17
	.long	.LFE17
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x36f
	.uleb128 0x18
	.long	.LASF39
	.byte	0x1
	.word	0x1cd
	.long	0x76
	.long	.LLST16
	.uleb128 0x15
	.long	.LASF40
	.byte	0x1
	.word	0x1ce
	.long	0x76
	.byte	0x1
	.byte	0x69
	.byte	0x0
	.uleb128 0x1b
	.byte	0x1
	.long	.LASF44
	.byte	0x1
	.word	0x1df
	.byte	0x1
	.long	.LFB18
	.long	.LFE18
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.uleb128 0x12
	.byte	0x1
	.long	.LASF41
	.byte	0x1
	.word	0x1f1
	.byte	0x1
	.long	.LFB19
	.long	.LFE19
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.long	0x3be
	.uleb128 0x18
	.long	.LASF42
	.byte	0x1
	.word	0x1f4
	.long	0x76
	.long	.LLST19
	.uleb128 0x18
	.long	.LASF43
	.byte	0x1
	.word	0x1f4
	.long	0x76
	.long	.LLST20
	.byte	0x0
	.uleb128 0x1b
	.byte	0x1
	.long	.LASF45
	.byte	0x1
	.word	0x258
	.byte	0x1
	.long	.LFB20
	.long	.LFE20
	.byte	0x2
	.byte	0x90
	.uleb128 0x20
	.uleb128 0x1c
	.long	.LASF46
	.byte	0x1
	.byte	0x40
	.long	0x10f
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	gt_hw_info
	.uleb128 0xb
	.long	0x169
	.long	0x3f0
	.uleb128 0x1d
	.byte	0x0
	.uleb128 0x1e
	.long	.LASF47
	.byte	0x3
	.byte	0x6c
	.long	0x3e5
	.byte	0x1
	.byte	0x1
	.uleb128 0x1e
	.long	.LASF48
	.byte	0x4
	.byte	0x4d
	.long	0x40a
	.byte	0x1
	.byte	0x1
	.uleb128 0x1f
	.long	0x76
	.uleb128 0xb
	.long	0x76
	.long	0x41f
	.uleb128 0xc
	.long	0xf5
	.byte	0x7f
	.byte	0x0
	.uleb128 0x1e
	.long	.LASF49
	.byte	0x4
	.byte	0x4e
	.long	0x40f
	.byte	0x1
	.byte	0x1
	.uleb128 0x1e
	.long	.LASF50
	.byte	0x4
	.byte	0x50
	.long	0x40a
	.byte	0x1
	.byte	0x1
	.uleb128 0x1e
	.long	.LASF51
	.byte	0x4
	.byte	0x51
	.long	0x40f
	.byte	0x1
	.byte	0x1
	.uleb128 0x1e
	.long	.LASF52
	.byte	0x4
	.byte	0x54
	.long	0x40a
	.byte	0x1
	.byte	0x1
	.uleb128 0x1c
	.long	.LASF53
	.byte	0x1
	.byte	0x5e
	.long	0x40a
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	guc_measure_cnt
	.uleb128 0x1c
	.long	.LASF54
	.byte	0x1
	.byte	0x5f
	.long	0x40a
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	guc_adc_state
	.uleb128 0x1c
	.long	.LASF55
	.byte	0x1
	.byte	0x60
	.long	0x40a
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	guc_adc_stream_en
	.uleb128 0x1c
	.long	.LASF56
	.byte	0x1
	.byte	0x4a
	.long	0x76
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	guc_30usec_cnt
	.uleb128 0x1c
	.long	.LASF57
	.byte	0x1
	.byte	0x4b
	.long	0x40a
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	guc_240usec_timer
	.uleb128 0xb
	.long	0x81
	.long	0x4bd
	.uleb128 0xc
	.long	0xf5
	.byte	0x3
	.byte	0x0
	.uleb128 0x1c
	.long	.LASF58
	.byte	0x1
	.byte	0x4d
	.long	0x4ad
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	gus_acc
	.uleb128 0xb
	.long	0x76
	.long	0x4df
	.uleb128 0xc
	.long	0xf5
	.byte	0xff
	.byte	0x0
	.uleb128 0x1c
	.long	.LASF59
	.byte	0x1
	.byte	0x47
	.long	0x4cf
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	guca_dbg_buff
	.uleb128 0x1c
	.long	.LASF60
	.byte	0x1
	.byte	0x48
	.long	0x76
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	guc_dbg_buff_idx
	.uleb128 0x1c
	.long	.LASF61
	.byte	0x1
	.byte	0x42
	.long	0x81
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	gus_trap_line
	.uleb128 0xb
	.long	0x81
	.long	0x525
	.uleb128 0xc
	.long	0xf5
	.byte	0xf
	.byte	0x0
	.uleb128 0x1c
	.long	.LASF62
	.byte	0x1
	.byte	0x45
	.long	0x515
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	gusa_trace
	.uleb128 0x1c
	.long	.LASF63
	.byte	0x1
	.byte	0x4f
	.long	0x8c
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	gpf_action_func
	.uleb128 0x1c
	.long	.LASF64
	.byte	0x1
	.byte	0x5d
	.long	0x40a
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.long	guc_delay_cnt
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
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x7
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x8
	.uleb128 0x18
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.uleb128 0x9
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
	.uleb128 0xa
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
	.uleb128 0xb
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xc
	.uleb128 0x21
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0xd
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
	.uleb128 0xe
	.uleb128 0x15
	.byte	0x0
	.uleb128 0x27
	.uleb128 0xc
	.byte	0x0
	.byte	0x0
	.uleb128 0xf
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
	.uleb128 0x10
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
	.uleb128 0x11
	.uleb128 0x2e
	.byte	0x0
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
	.byte	0x0
	.byte	0x0
	.uleb128 0x12
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
	.uleb128 0x13
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
	.uleb128 0xa
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
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x16
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
	.uleb128 0x17
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
	.uleb128 0x2
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x19
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.byte	0x0
	.byte	0x0
	.uleb128 0x1a
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
	.uleb128 0x49
	.uleb128 0x13
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
	.uleb128 0x1b
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
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x1c
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
	.uleb128 0x1d
	.uleb128 0x21
	.byte	0x0
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
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0x0
	.byte	0x0
	.uleb128 0x1f
	.uleb128 0x35
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,"",@progbits
	.long	0x1c5
	.word	0x2
	.long	.Ldebug_info0
	.long	0x55c
	.long	0x174
	.string	"activate_adc_isr"
	.long	0x19c
	.string	"init_apd_bias_pwm"
	.long	0x1af
	.string	"init_uart"
	.long	0x1c2
	.string	"init_lcd"
	.long	0x1d5
	.string	"init_spi"
	.long	0x1e8
	.string	"init_adc"
	.long	0x1fb
	.string	"FATAL_TRAP"
	.long	0x228
	.string	"command_lookup"
	.long	0x300
	.string	"main"
	.long	0x337
	.string	"__vector_13"
	.long	0x36f
	.string	"__vector_4"
	.long	0x384
	.string	"__vector_19"
	.long	0x3be
	.string	"__vector_8"
	.long	0x3d3
	.string	"gt_hw_info"
	.long	0x453
	.string	"guc_measure_cnt"
	.long	0x465
	.string	"guc_adc_state"
	.long	0x477
	.string	"guc_adc_stream_en"
	.long	0x489
	.string	"guc_30usec_cnt"
	.long	0x49b
	.string	"guc_240usec_timer"
	.long	0x4bd
	.string	"gus_acc"
	.long	0x4df
	.string	"guca_dbg_buff"
	.long	0x4f1
	.string	"guc_dbg_buff_idx"
	.long	0x503
	.string	"gus_trap_line"
	.long	0x525
	.string	"gusa_trace"
	.long	0x537
	.string	"gpf_action_func"
	.long	0x549
	.string	"guc_delay_cnt"
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
	.string	"dle_ctrl.c"
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
	.byte	0x75
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
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM7
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM8
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM9
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM10
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM11
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM12
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM13
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM14
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM15
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM16
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM17
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM18
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM19
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM20
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM21
	.byte	0x1c
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM22
	.byte	0x1c
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM23
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM24
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM25
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM26
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM27
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM28
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM29
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM30
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM31
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM32
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM33
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM34
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM35
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM36
	.byte	0x1d
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM37
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM38
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM39
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM40
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM41
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM42
	.byte	0x24
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM43
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM44
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM45
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM46
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM47
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM48
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM49
	.byte	0x89
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM50
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM51
	.byte	0x1d
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM52
	.byte	0x1e
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM53
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM54
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM55
	.byte	0x1c
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM56
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM57
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM58
	.byte	0x1f
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM59
	.byte	0xd
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM60
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM61
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM62
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM63
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM64
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM65
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM66
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM67
	.byte	0x3
	.sleb128 -167
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM68
	.byte	0x1d
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM69
	.byte	0x12
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM70
	.byte	0x24
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM71
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM72
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM73
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM74
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM75
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM76
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM77
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM78
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM79
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM80
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM81
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM82
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM83
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM84
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM85
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM86
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM87
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM88
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM89
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM90
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM91
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM92
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM93
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM94
	.byte	0x17
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
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM98
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM99
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM100
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM101
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM102
	.byte	0x3
	.sleb128 -19
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM103
	.byte	0x77
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM104
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM105
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM106
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM107
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM108
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM109
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM110
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM111
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM112
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM113
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM114
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM115
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM116
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM117
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM118
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM119
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM120
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM121
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM122
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM123
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM124
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM125
	.byte	0x38
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM126
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM127
	.byte	0x1b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM128
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM129
	.byte	0x3
	.sleb128 -52
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM130
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM131
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM132
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM133
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM134
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM135
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM136
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM137
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM138
	.byte	0x15
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
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM143
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM144
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM145
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM146
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM147
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM148
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM149
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM150
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM151
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM152
	.byte	0x20
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM153
	.byte	0x1f
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM154
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM155
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM156
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM157
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM158
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM159
	.byte	0x1d
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM160
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM161
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM162
	.byte	0xc
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.LM163
	.byte	0x26
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.long	.Letext0
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
.LELT0:
	.section	.debug_str,"MS",@progbits,1
.LASF27:
	.string	"activate_adc_isr"
.LASF69:
	.string	"command_lookup"
.LASF28:
	.string	"FATAL_TRAP"
.LASF51:
	.string	"guca_out_msg_buff"
.LASF54:
	.string	"guc_adc_state"
.LASF36:
	.string	"main"
.LASF18:
	.string	"ca_name"
.LASF14:
	.string	"ca_signature"
.LASF47:
	.string	"gta_action_table"
.LASF20:
	.string	"pf_func"
.LASF17:
	.string	"HW_INFO"
.LASF65:
	.string	"GNU C 4.2.2 (WinAVR 20071221)"
.LASF57:
	.string	"guc_240usec_timer"
.LASF31:
	.string	"uc_act_cmd"
.LASF10:
	.string	"uint16"
.LASF2:
	.string	"uint8_t"
.LASF55:
	.string	"guc_adc_stream_en"
.LASF9:
	.string	"uint8"
.LASF30:
	.string	"us_line_num"
.LASF34:
	.string	"__addr16"
.LASF44:
	.string	"__vector_4"
.LASF45:
	.string	"__vector_8"
.LASF7:
	.string	"long long int"
.LASF49:
	.string	"guca_in_msg_buff"
.LASF56:
	.string	"guc_30usec_cnt"
.LASF21:
	.string	"T_ACTION"
.LASF5:
	.string	"long int"
.LASF64:
	.string	"guc_delay_cnt"
.LASF26:
	.string	"init_adc"
.LASF19:
	.string	"uc_cmd"
.LASF68:
	.string	"action_tag"
.LASF59:
	.string	"guca_dbg_buff"
.LASF11:
	.string	"PF_PVOID"
.LASF1:
	.string	"unsigned char"
.LASF46:
	.string	"gt_hw_info"
.LASF66:
	.string	"../dle_ctrl.c"
.LASF0:
	.string	"signed char"
.LASF8:
	.string	"long long unsigned int"
.LASF25:
	.string	"init_spi"
.LASF52:
	.string	"guc_out_msg_rd_idx"
.LASF4:
	.string	"unsigned int"
.LASF33:
	.string	"uc_msg_cmd"
.LASF3:
	.string	"uint16_t"
.LASF12:
	.string	"uc_ver_maj"
.LASF35:
	.string	"__result"
.LASF32:
	.string	"pt_action"
.LASF62:
	.string	"gusa_trace"
.LASF16:
	.string	"char"
.LASF22:
	.string	"init_apd_bias_pwm"
.LASF40:
	.string	"uc_data"
.LASF23:
	.string	"init_uart"
.LASF39:
	.string	"uc_status"
.LASF13:
	.string	"uc_ver_min"
.LASF38:
	.string	"__vector_13"
.LASF6:
	.string	"long unsigned int"
.LASF58:
	.string	"gus_acc"
.LASF29:
	.string	"uc_adc_stream_en"
.LASF41:
	.string	"__vector_19"
.LASF63:
	.string	"gpf_action_func"
.LASF61:
	.string	"gus_trap_line"
.LASF37:
	.string	"uc_i"
.LASF42:
	.string	"uc_data_h"
.LASF43:
	.string	"uc_data_l"
.LASF24:
	.string	"init_lcd"
.LASF53:
	.string	"guc_measure_cnt"
.LASF67:
	.string	"C:\\\\MyWorks\\\\RangeFinder_rvrs\\\\FW\\\\DLE_CTRL\\\\default"
.LASF48:
	.string	"guc_in_msg_idx"
.LASF50:
	.string	"guc_out_msg_wr_idx"
.LASF60:
	.string	"guc_dbg_buff_idx"
.LASF15:
	.string	"reserved"
/* File "../dle_ctrl.c": code  852 = 0x0354 ( 769), prologues  38, epilogues  45 */
