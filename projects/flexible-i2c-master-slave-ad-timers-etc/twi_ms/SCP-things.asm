;
;	This is just a filler checking routine called from the main loop.  All it does now is look
;	 at an input pin and change the GIR when it is seen to go low.
;


SCP_Check:
	rcall	SCP_Pri_Check
	rcall	SCP_Sec_Check
	ret


SCP_Pri_Check:
	in	Temp, SCP_Pri_Present_port
	sbrc	Temp, SCP_Pri_Present_bit	;if the scp is there, check some things
	ret
;update the GIR
	lds	Temp, I2C_GIR0
	sbr	Temp, GIR0_SCP_Present_Pri_mask
	sts	I2C_GIR0, Temp

	ret									;if the enable bit is not set, no need to do more



SCP_Sec_Check:			;do all of the same things, but for the secondary

	in	Temp, SCP_Sec_Present_port
	sbrc	Temp, SCP_Sec_Present_bit	;if the scp is there, check some things
	ret
;update the GIR
	lds	Temp, I2C_GIR0
	sbr	Temp, GIR0_SCP_Present_Sec_mask
	sts	I2C_GIR0, Temp
	
	ret
