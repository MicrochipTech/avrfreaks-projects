;
;	These are the memory locations that hold the "registers" accessable from the I2C slave space
;
;	The bit definitions are mostly just for example, however the read auto-increment bit in CTRL1
;	 is implemented in the ISR.  Note that offsets 0-6 are read only (write data cycles will not be ACKed)
;	 and there are 16 locations at the end that behave as normal memory that can be used for testing.
;	The WD Reset bit of GIR1 is implemented in this code.
;
.dseg
I2C_Slave_Base:
I2C_GIR0:		.db	0		;offset 0
I2C_GIR1:		.db	0		;offset 1
I2C_AD0:		.db	0		;offset 2
I2C_AD1:		.db	0		;offset 3
I2C_AD2:		.db	0		;offset 4
I2C_AD3:		.db	0		;offset 5
I2C_SWVer:		.db 0		;offset	6
I2C_CTRL0:		.db 0		;offset 7
I2C_CTRL1:		.db	0		;offset 8
I2C_RW:	.db	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15		;offsets 9 - 25

CReg_Status:	.db 0		;set to indicate which of the control regs have been changed
Prev_CTRL0:		.db 0		;storage for previous values
Prev_CTRL1:		.db 0

Boot_MCSR:		.db 0		;storage for MSCR read at boot time

.cseg

.equ	CTRL0_Changed_bit = 0
.equ	CTRL0_Changed_mask = 0x01
.equ	CTRL1_Changed_bit = 1
.equ	CTRL1_Changed_mask = 0x02

.equ	I2C_NumRegs = CReg_Status - I2C_Slave_Base
.equ	I2C_RW_Offset = I2C_CTRL0 - I2C_Slave_Base	;writes below this offset are not valid
.equ	CTRL0_offset = I2C_CTRL0 - I2C_Slave_Base
.equ	CTRL1_offset = I2C_CTRL1 - I2C_Slave_Base

.equ CTRL0_Alrm_Make_mask = 0x01
.equ CTRL0_Alrm_Make_bit = 0
.equ CTRL0_Alrm_Brk_mask = 0x02
.equ CTRL0_Alrm_Brk_bit = 1
.equ CTRL0_SCP_PwrClk_Pri_mask = 0x04
.equ CTRL0_SCP_PwrClk_Pri_bit = 2
.equ CTRL0_SCP_PwrClk_Sec_mask = 0x08
.equ CTRL0_SCP_PwrClk_Sec_bit = 3
.equ CTRL0_SDB_nc1_mask = 0x10
.equ CTRL0_SDB_nc1_bit = 4
.equ CTRL0_SDB_nc2_mask = 0x20
.equ CTRL0_SDB_nc2_bit = 5
.equ CTRL0_SCP_Cyc_Pri_mask = 0x40
.equ CTRL0_SCP_Cyc_Pri_bit = 6
.equ CTRL0_SCP_Cyc_Sec_mask = 0x80
.equ CTRL0_SCP_Cyc_Sec_bit = 7

.equ CTRL0_Init = CTRL0_SCP_PwrClk_Pri_mask | CTRL0_Alrm_Make_mask

.equ CTRL1_PS_Inh_Pri_mask = 0x01
.equ CTRL1_PS_Inh_Pri_bit = 0
.equ CTRL1_PS_Inh_Sec_mask = 0x02
.equ CTRL1_PS_Inh_Sec_bit = 1
.equ CTRL1_Fan_Full_In_mask = 0x04
.equ CTRL1_Fan_Full_In_bit = 2
.equ CTRL1_Fan_Full_Ex_Mask = 0x08
.equ CTRL1_Fan_Full_Ex_bit = 3
.equ CTRL1_Sys_Reset_mask = 0x10
.equ CTRL1_Sys_Reset_bit = 4
.equ CTRL1_Adr_AutoInc_mask = 0x20
.equ CTRL1_Adr_AutoInc_bit = 5
.equ CTRL1_ReProg_Mode_mask = 0x40
.equ CTRL1_ReProg_Mode_bit = 6
.equ CTRL1_ReProg_Actv_mask = 0x80
.equ CTRL1_ReProg_Actv_bit = 7

.equ CTRL1_Init = 0x00

.equ GIR0_Pwr_Good_Pri_mask = 0x01
.equ GIR0_Pwr_Good_Pri_bit = 0
.equ GIR0_Pwr_Good_Sec_mask = 0x02
.equ GIR0_Pwr_Good_Sec_bit = 1
.equ GIR0_Fan_Good_In_mask = 0x04
.equ GIR0_Fan_Good_In_bit = 2
.equ GIR0_Fan_Good_Ex_mask = 0x08
.equ GIR0_Fan_Good_Ex_bit =3
.equ GIR0_PS_Present_Pri_mask = 0x10
.equ GIR0_PS_Present_Pri_bit = 4
.equ GIR0_PS_Present_Sec_mask = 0x20
.equ GIR0_PS_Present_Sec_bit = 5
.equ GIR0_SCP_Present_Pri_mask = 0x40
.equ GIR0_SCP_Present_Pri_bit = 6
.equ GIR0_SCP_Present_Sec_mask = 0x80
.equ GIR0_SCP_Present_Sec_bit = 7

.equ GIR1_Alrm_SIP1_mask = 0x01
.equ GIR1_Alrm_SIP1_bit = 0
.equ GIR1_Alrm_SIP2_mask = 0x02
.equ GIR1_Alrm_SIP2_bit = 1
.equ GIR1_PEM_Present_Other_mask = 0x04
.equ GIR1_PEM_Present_Other_bit = 2
.equ GIR1_Temp_Alrm_Rear_mask = 0x08
.equ GIR1_Temp_Alrm_Rear_bit = 3
.equ GIR1_Temp_Alrm_Front_mask = 0x10
.equ GIR1_Temp_Alrm_Front_bit = 4
.equ GIR1_PS_Degraded_Pri_mask = 0x20
.equ GIR1_PS_Degraded_Pri_bit = 5
.equ GIR1_PS_Fail_Pri_mask = 0x40
.equ GIR1_PS_Fail_Pri_bit = 6
.equ GIR1_WatchDog_mask = 0x80
.equ GIR1_WatchDog_bit = 7

I2C_Regs_Init:
;
;	just zero all of them except the control reg, where you enable the primary SCP and clocks,
;	and the GIR where you deal with the watchdog bit
;
	ldi	T2, I2C_NumRegs
	ldi	XL, low(I2C_Slave_Base)
	ldi	XH, high(I2C_Slave_Base)
	clr Temp
I2C_Regs_Init_lp:
	st	X+, Temp
	dec	T2
	cpse	T2, Temp
	rjmp	I2C_Regs_Init_lp

	in T2, MCUCSR
	sbrc	T2, WDRF
	sbr	Temp, GIR1_WatchDog_mask

	sts	I2C_GIR1, Temp

	ldi	Temp, CTRL0_Init
	sts	I2C_CTRL0, Temp
	sts	Prev_CTRL0, Temp
	ldi	Temp, CTRL1_Init
	sts I2C_CTRL1, Temp
	sts	Prev_CTRL1, Temp

	clr	Temp
	sts CReg_Status, Temp

	ldi	Temp, SW_Version
	sts	I2C_SWVer, Temp

	ret
