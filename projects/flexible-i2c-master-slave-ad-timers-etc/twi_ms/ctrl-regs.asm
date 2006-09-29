;	These routines are called when some other I2C master has written one or the other control register bytes,
;	 so this is where the bits are acted upon.
;
;	This code does not do anything in this example, but there is some commented out stuff that gives an idea
;	 of dealing with some cases that take various actions based on the setting or clearing of control register bits.
;	 This is obviously based on a real system monitor application, but the specifics of that system are not included here.
;
CTRL0_Change:
	lds	Temp, I2C_CTRL0
	mov	T3, Temp
	lds	T2, Prev_CTRL0
	eor Temp, T2
	andi	Temp, (CTRL0_Alrm_Make_mask | CTRL0_Alrm_Brk_mask )
; The SCP power and power cycle bits are handled by the SCP handling code, so mask them off
	cpi	Temp, 0
	breq C0C_4

; Handle all of the changed bits in one pass, so fall through after each case making sure to not mess up regs
;	Temp has the change mask, T3 has the new value
C0C_0:
	sbrs	Temp, CTRL0_Alrm_Make_bit
	rjmp	C0C_1

	;fill in the code that handles this bit here...

;	st	-Y, Temp						;save Temp since these routines mess it up
;	sbrc	T3, CTRL0_Alrm_Make_bit
;	call	Alrm_Set_Make
;	sbrs	T3, CTRL0_Alrm_Make_bit
;	call	Alrm_Clr_Make
;	ld	Temp, Y+						;restore Temp

C0C_1:
	sbrs	Temp, CTRL0_Alrm_Brk_bit
	rjmp	C0C_4

	;fill in the code that handles this bit here...

;	sbrc	T3, CTRL0_Alrm_Brk_bit
;	call	Alrm_Set_Break
;	sbrs	T3, CTRL0_Alrm_Brk_bit
;	call	Alrm_Clr_Break
;
C0C_4:
	lds	Temp, CReg_Status				;clear the changed flag
	cbr	Temp, CTRL0_Changed_mask
	sts	CReg_Status, Temp

	lds	Temp, I2C_CTRL0					;make a new previous
	sts	Prev_CTRL0, Temp
		
	ret


CTRL1_Change:
	lds	Temp, I2C_CTRL1
	mov	T3, Temp

	lds	T2, Prev_CTRL1
	eor Temp, T2
	andi	Temp, (CTRL1_PS_Inh_Pri_mask | CTRL1_PS_Inh_Sec_mask | CTRL1_Sys_Reset_mask | CTRL1_Fan_Full_In_mask | CTRL1_Fan_Full_Ex_mask | CTRL1_ReProg_Mode_mask)
; The auto-inc bits are handled otherplaces
	cpi	Temp, 0
	brne C1C_0
	jmp	C1C_5

; Handle all of the changed bits in one pass, so fall through after each case making sure to not mess up regs
;	Temp has the change mask, T3 has the new value
C1C_0:
	sbrs	Temp, CTRL1_PS_Inh_Pri_bit
	rjmp	C1C_1

	;fill in the code that handles this bit here...

;	st	-Y, Temp
;	sbrs	T3, CTRL1_PS_Inh_Pri_bit
;	rjmp	C1C_0_0

;	in	Temp, PSPri_Inhibit_port
;	cbr	Temp, PSPri_Inhibit_mask
;	out PSPri_Inhibit_port, Temp
;	rjmp	C1C_0_1
;C1C_0_0:
;	in	Temp, PSPri_Inhibit_port
;	sbr	Temp, PSPri_Inhibit_mask
;	out PSPri_Inhibit_port, Temp

;C1C_0_1:
;	ld	Temp, Y+						;restore Temp

C1C_1:
	sbrs	Temp, CTRL1_PS_Inh_Sec_bit
	rjmp	C1C_2

	;fill in the code that handles this bit here...

;	st	-Y, Temp						;save Temp since these routines mess it up
;	sbrs	T3, CTRL1_PS_Inh_Sec_bit
;	rjmp	C1C_1_0

;	in	Temp, PSSec_Inhibit_port
;	cbr	Temp, PSSec_Inhibit_mask
;	out PSSec_Inhibit_port, Temp
;	rjmp	C1C_1_1
;C1C_1_0:
;	in	Temp, PSSec_Inhibit_port
;	sbr	Temp, PSSec_Inhibit_mask
;	out PSSec_Inhibit_port, Temp

;;C1C_1_1:
	ld	Temp, Y+						;restore Temp

C1C_2:
	sbrc	Temp, CTRL1_Sys_Reset_bit
	rjmp	C1C_3

	;fill in the code that handles this bit here...

;	sbrs	T3, CTRL1_Sys_Reset_bit
;	rjmp	C1C_3						;nothing to do if it's written to 0, and it shouldn't happen

;	st	-Y, Temp
;	in	Temp, Sys_Reset_port
;	sbr	Temp, Sys_Reset_mask
;	out Sys_Reset_port, Temp
;	in	Temp, PORTA						;this is necessary in order to ensure a pullup
;	cbr	Temp, Sys_Reset_mask
;	out	PORTA, Temp

;	ldi	Temp, 50							;reset it for 500ms
;	rcall	Wait_x10ms
	
;	in	Temp, PORTA
;	sbr	Temp, Sys_Reset_mask
;	out	PORTA, Temp						;this drives high then goes into pullup mode
;	in	Temp, Sys_Reset_port
;	cbr	Temp, Sys_Reset_mask
;	out Sys_Reset_port, Temp

;	cbr	T3, CTRL1_Sys_Reset_mask		;clear the register bit
;	sts	I2C_CTRL1, T3					;and update the memory image

;	ld	Temp, Y+

C1C_3:
	sbrs	Temp, CTRL1_ReProg_Mode_bit
	rjmp	C1C_5

	;fill in the code that handles this bit here...

;	st	-Y, Temp
;	ldi	Temp, ReProg_Timeout
;	sts	ReProg_Timer, Temp
;	clr	Temp
;	sts	Page_cntr, Temp
;	sts Flash_Copy_flg, Temp
;	ld	Temp, Y+

C1C_4:
	andi	Temp, CTRL1_Fan_Full_In_mask | CTRL1_Fan_Full_Ex_mask
	cpi	Temp, 0
	breq	C1C_5
	
	;fill in the code that handles this bit here...

;	rcall	CR_Fan_Full

		
C1C_5:
	lds	Temp, CReg_Status				;clear the changed mask
	cbr	Temp, CTRL1_Changed_mask
	sts	CReg_Status, Temp

	lds	Temp, I2C_CTRL1					;make a new previous
	sts	Prev_CTRL1, Temp
		
	ret

	

