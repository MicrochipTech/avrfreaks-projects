;---------------------------------------------------------
; glcd command definitions
.equ		GLCD_CMD_MC		=	0x00
.equ		GLCD_CMD_SETP	=	0x01
.equ		GLCD_CMD_SETN	=	0x02
.equ		GLCD_CMD_SETD	=	0x03
.equ		GLCD_CMD_SETCUR	=	0x04
.equ		GLCD_CMD_SETSL	=	0x08
.equ		GLCD_CMD_SETSH	=	0x09
.equ		GLCD_CMD_SETCL	=	0x0A
.equ		GLCD_CMD_SETCH	=	0x0B
.equ		GLCD_CMD_WRDD	=	0x0C
.equ		GLCD_CMD_RDDD	=	0x0D
.equ		GLCD_CMD_SETBIT	=	0x0E
.equ		GLCD_CMD_CLRBIT	=	0x0F
;---------------------------------------------------------
; glcd flags
.equ		GLCD_FLG_MC_DON	=	0b00100000
.equ		GLCD_FLG_MC_MAS	=	0b00010000
.equ		GLCD_FLG_MC_BLK	=	0b00001000
.equ		GLCD_FLG_MC_CUR	=	0b00000100
.equ		GLCD_FLG_MC_GR	=	0b00000010
.equ		GLCD_FLG_MC_EXT	=	0b00000001
;---------------------------------------------------------
; HD61830 reset
HD61830_Reset:
			sbi		GLCD_CONTROL,GLCD_CS
			rcall	HD61830_Delay
			cbi		GLCD_CONTROL,GLCD_RESET
			ldi		r16,200
HD61830_Reset_l1:
			rcall	HD61830_Delay
			dec		r16
			brne	HD61830_Reset_l1
			sbi		GLCD_CONTROL,GLCD_RESET
			ret
;---------------------------------------------------------
; HD61830 IR write
; r16 - command to write
HD61830_IR_Write:
			sbi		GLCD_CONTROL,GLCD_RS						; select IR
			cbi		GLCD_CONTROL,GLCD_RW						; write in progress
			sbi		GLCD_CONTROL,GLCD_EN
			rcall	HD61830_Delay
			out		GLCD_DATA,r16
			rcall	HD61830_Delay
			cbi		GLCD_CONTROL,GLCD_EN
			ret
;---------------------------------------------------------
; HD61830 data write
; r17 - value
HD61830_DIR_Write:
			cbi		GLCD_CONTROL,GLCD_RS					; select DIR
			cbi		GLCD_CONTROL,GLCD_RW					; write in progress
			sbi		GLCD_CONTROL,GLCD_EN
			rcall	HD61830_Delay
			out		GLCD_DATA,r17
			rcall	HD61830_Delay
			cbi		GLCD_CONTROL,GLCD_EN
			ret
;---------------------------------------------------------
; HD61830 busy check
HD61830_BusyCheck:
			push	r16
			sbi		GLCD_CONTROL,GLCD_RW
			rcall	HD61830_Delay
			sbi		GLCD_CONTROL,GLCD_RS
			rcall	HD61830_Delay
			clr		r16
			out		GLCD_DATAD,r16
			out		GLCD_DATA,r16
			clr		r16
			out		GLCD_DATAP,r16
HD61830_BusyCheck_l1:
			sbi		GLCD_CONTROL,GLCD_EN
			rcall	HD61830_Delay
			in		r16,GLCD_DATAP
			cbi		GLCD_CONTROL,GLCD_EN
			andi	r16,0x01
			brne	HD61830_BusyCheck_l1
			ldi		r16,0xff
			out		GLCD_DATAD,r16
			pop		r16
			ret
			
;---------------------------------------------------------
; HD61830 command write
; r16 - command to write
; r17 - value
HD61830_CommandWrite:
			rcall	BitReverse
			cbi		GLCD_CONTROL,GLCD_CS						; select HD61830
			rcall	HD61830_Delay
			rcall	HD61830_BusyCheck
			rcall	HD61830_IR_Write
			rcall	HD61830_DIR_Write
			sbi		GLCD_CONTROL,GLCD_CS
			ret
;---------------------------------------------------------
; delay
HD61830_Delay:
			nop
			ret
;---------------------------------------------------------
; HD61830 text init
HD61830_TextInit:
			push	r16
			push	r17
			ldi		r16,GLCD_CMD_MC
			ldi		r17,GLCD_FLG_MC_DON+GLCD_FLG_MC_MAS+GLCD_FLG_MC_BLK+GLCD_FLG_MC_CUR
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETP
			ldi		r17,0b01110111
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETN
			ldi		r17,GLCD_TEXTWIDTH-1
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETD
			ldi		r17,GLCD_DUTY-1
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETSL
			ldi		r17,low(GLCD_SCANSTART)
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETSH
			ldi		r17,high(GLCD_SCANSTART)
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETCL
			ldi		r17,0
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETCH
			ldi		r17,0
			rcall	HD61830_CommandWrite
			pop		r17
			pop		r16
			ret
;---------------------------------------------------------
; HD61830 text init
HD61830_GrInit:
			push	r16
			push	r17
			ldi		r16,GLCD_CMD_MC
			ldi		r17,GLCD_FLG_MC_DON+GLCD_FLG_MC_MAS+GLCD_FLG_MC_GR
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETP
			ldi		r17,7
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETN
			ldi		r17,GLCD_GRAPHWIDTH-1
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETD
			ldi		r17,GLCD_DUTY-1
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETCUR
			ldi		r17,7
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETSL
			ldi		r17,low(GLCD_SCANSTART)
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETSH
			ldi		r17,high(GLCD_SCANSTART)
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETCL
			ldi		r17,0
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETCH
			ldi		r17,0
			rcall	HD61830_CommandWrite
			pop		r17
			pop		r16
			ret
;---------------------------------------------------------
; HD61830 clear text display
HD61830_Cls:
			push	r16
			push	r17
			push	r18
			ldi		r16,GLCD_CMD_SETCL
			ldi		r17,0
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETCH
			ldi		r17,0
			rcall	HD61830_CommandWrite
			cbi		GLCD_CONTROL,GLCD_CS						; select HD61830
			ldi		r16,GLCD_CMD_WRDD
			rcall	BitReverse
			rcall	HD61830_IR_Write
			ldi		r18,128
			ldi		r17,0
HD61830_Cls_l1:
			push	r18
			ldi		r18,30
HD61830_Cls_l2:
			rcall	HD61830_BusyCheck
			rcall	HD61830_DIR_Write
			dec		r18
			brne	HD61830_Cls_l2
			pop		r18
			dec		r18
			brne	HD61830_Cls_l1
			pop		r18
			pop		r17
			pop		r16
			sbi		GLCD_CONTROL,GLCD_CS						; unselect HD61830
			ret
;---------------------------------------------------------
; HD61830 text out
; z - pointer to zero terminated string
HD61830_TextOut:
			cbi		GLCD_CONTROL,GLCD_CS						; select HD61830
			push	r16
			push	r17
			rcall	HD61830_BusyCheck
			ldi		r16,GLCD_CMD_WRDD
			rcall	BitReverse
			rcall	HD61830_IR_Write
HD61830_TextOut_l1:
			lpm		r17,z
			adiw	zl,1
			cpi		r17,0
			breq	HD61830_TextOut_end
			rcall	HD61830_BusyCheck
			rcall	BitReverse
			rcall	HD61830_DIR_Write
			rjmp	HD61830_TextOut_l1
HD61830_TextOut_end:
			sbi		GLCD_CONTROL,GLCD_CS
			pop		r16
			ret

;---------------------------------------------------------
; HD61830_Line_Out
; r16 - line number
; x - line data pointer
HD61830_LineOut:
			push	r0
			push	r1
			push	r17
			push	r18
			ldi		r17,GLCD_GRAPHWIDTH
			mul		r16,r17
			ldi		r16,GLCD_CMD_SETCL
			mov		r17,r0
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETCH
			mov		r17,r1
			rcall	HD61830_CommandWrite
			cbi		GLCD_CONTROL,GLCD_CS						; select HD61830
			rcall	HD61830_BusyCheck
			ldi		r16,GLCD_CMD_WRDD
			rcall	BitReverse
			rcall	HD61830_IR_Write
			ldi		r18,GLCD_GRAPHWIDTH
HD61830_LineOut_l1:
			ld		r17,x+
			rcall	BitReverse
			rcall	HD61830_BusyCheck
			rcall	HD61830_DIR_Write
			dec		r18
			brne	HD61830_LineOut_l1
			sbi		GLCD_CONTROL,GLCD_CS
			pop		r18
			pop		r17
			pop		r1
			pop		r0
			ret
;---------------------------------------------------------
; HD61830_CLL
; r16 - line number
; r17 - line count
HD61830_CLL:
			push	r0
			push	r1
			push	r17
			push	r18
			push	r19
			mov		r19,r17
			ldi		r17,GLCD_GRAPHWIDTH
			mul		r16,r17
			ldi		r16,GLCD_CMD_SETCL
			mov		r17,r0
			rcall	HD61830_CommandWrite
			ldi		r16,GLCD_CMD_SETCH
			mov		r17,r1
			rcall	HD61830_CommandWrite
			cbi		GLCD_CONTROL,GLCD_CS						; select HD61830
			rcall	HD61830_BusyCheck
			ldi		r16,GLCD_CMD_WRDD
			rcall	BitReverse
			rcall	HD61830_IR_Write
			clr		r17
HD61830_CLL_l1:
			ldi		r18,GLCD_GRAPHWIDTH
HD61830_CLL_l2:
			rcall	HD61830_BusyCheck
			rcall	HD61830_DIR_Write
			dec		r18
			brne	HD61830_CLL_l2
			dec		r19
			brne	HD61830_CLL_l1
			sbi		GLCD_CONTROL,GLCD_CS
			pop		r19
			pop		r18
			pop		r17
			pop		r1
			pop		r0
			ret
;---------------------------------------------------------
; Bit reverse
BitReverse:	push	r18
			push	r15
			push	r14
			ldi		r18,8
			clc
BitReverse_l1:
			ror		r17
			rol		r15
			ror		r16
			rol		r14
			dec		r18
			brne	BitReverse_l1
			mov		r17,r15
			mov		r16,r14
			pop		r14
			pop		r15
			pop		r18
			ret
;---------------------------------------------------------
