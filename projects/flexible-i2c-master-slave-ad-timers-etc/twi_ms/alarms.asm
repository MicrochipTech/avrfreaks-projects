;
; This is just filler stuff as an example of something executed every time through the main loop.
;	Pretty minimal for now...

;	Deal with the input bits
;
Alarm_Check:
;	Check the alarm input bits, update the GIR register
;		For now, we don't know what these inputs mean, so just pass them along, take no other actions
	lds	T2, I2C_GIR1
	in	Temp, Alarm_in0_port
	sbrs	Temp, Alarm_in0_bit
	sbr	T2, GIR1_Alrm_SIP1_mask
	sbrc	Temp, Alarm_in0_bit
	cbr	T2, GIR1_Alrm_SIP1_mask

	sbrs	Temp, Alarm_in1_bit
	sbr	T2, GIR1_Alrm_SIP2_mask
	sbrc	Temp, Alarm_in1_bit
	cbr	T2, GIR1_Alrm_SIP2_mask

	sts	I2C_GIR1, T2

	ret

Test_Check:
	ret
