
_main:
	LDI        27, 255
	OUT        93, 27
	LDI        27, 0
	OUT        94, 27
	IN         28, 93
	IN         29, 94
	SBIW       28, 34
	OUT        93, 28
	OUT        94, 29
	ADIW       28, 1
;wattmeter.mbas,9 :: 		main:
;wattmeter.mbas,11 :: 		UART1_Init(2400)                           ' initialize UART module
	LDI        27, 25
	OUT        41, 27
	LDI        27, 0
	OUT        64, 27
	CALL       _UART1_Init+0
;wattmeter.mbas,12 :: 		Delay_ms(100)
	LDI        17, 130
	LDI        16, 222
	DEC        16
	BRNE       $-1
	DEC        17
	BRNE       $-3
	NOP
;wattmeter.mbas,14 :: 		c1=0
	LDI        27, 0
	STS        _c1+0, 27
	STS        _c1+1, 27
;wattmeter.mbas,15 :: 		c2=0
	LDI        27, 0
	STS        _c2+0, 27
	STS        _c2+1, 27
;wattmeter.mbas,16 :: 		power2=0
	LDI        27, 0
	STS        _power2+0, 27
	STS        _power2+1, 27
;wattmeter.mbas,19 :: 		while TRUE
L__main2:
;wattmeter.mbas,21 :: 		adc_rd1 = ADC_Read(1)    ' get ADC value from 2nd channel
	LDI        27, 1
	PUSH       27
	CALL       _ADC_Read+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 1
	OUT        93, 26
	OUT        94, 27
	STS        _adc_rd1+0, 16
	STS        _adc_rd1+1, 17
;wattmeter.mbas,23 :: 		d3d= adc_rd1*34.233/1024      ' (180/180+847)=.175, 6/.1752=34.23
	LDI        18, 0
	MOV        19, 18
	CALL       _float_ulong2fp+0
	LDI        20, 152
	LDI        21, 238
	LDI        22, 8
	LDI        23, 66
	CALL       _float_fpmul1+0
	STD        Y+30, 16
	STD        Y+31, 17
	STD        Y+32, 18
	STD        Y+33, 19
	LDI        16, 0
	LDI        17, 4
	LDI        18, 0
	LDI        19, 0
	CALL       _float_ulong2fp+0
	STD        Y+26, 16
	STD        Y+27, 17
	STD        Y+28, 18
	STD        Y+29, 19
	LDD        16, Y+30
	LDD        17, Y+31
	LDD        18, Y+32
	LDD        19, Y+33
	PUSH       23
	PUSH       22
	PUSH       21
	PUSH       20
	LDD        20, Y+26
	LDD        21, Y+27
	LDD        22, Y+28
	LDD        23, Y+29
	CALL       _float_fpdiv1+0
	POP        20
	POP        21
	POP        22
	POP        23
	STS        _d3d+0, 16
	STS        _d3d+1, 17
	STS        _d3d+2, 18
	STS        _d3d+3, 19
;wattmeter.mbas,24 :: 		FloatToStr(d3d,d3dr)
	LDI        27, hi_addr(_d3dr+0)
	PUSH       27
	LDI        27, _d3dr+0
	PUSH       27
	PUSH       19
	PUSH       18
	PUSH       17
	PUSH       16
	CALL       _FloatToStr+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 6
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,27 :: 		adc_rd2 = ADC_Read(2)
	LDI        27, 2
	PUSH       27
	CALL       _ADC_Read+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 1
	OUT        93, 26
	OUT        94, 27
	STD        Y+26, 16
	STD        Y+27, 17
	STS        _adc_rd2+0, 16
	STS        _adc_rd2+1, 17
;wattmeter.mbas,28 :: 		e3e= adc_rd2*6/1024
	LDI        16, 6
	LDI        17, 0
	LDI        18, 0
	LDI        19, 0
	CALL       _float_ulong2fp+0
	LDD        20, Y+26
	LDD        21, Y+27
	STD        Y+26, 16
	STD        Y+27, 17
	STD        Y+28, 18
	STD        Y+29, 19
	MOVW       16, 20
	LDI        18, 0
	MOV        19, 18
	CALL       _float_ulong2fp+0
	LDD        20, Y+26
	LDD        21, Y+27
	LDD        22, Y+28
	LDD        23, Y+29
	CALL       _float_fpmul1+0
	STD        Y+30, 16
	STD        Y+31, 17
	STD        Y+32, 18
	STD        Y+33, 19
	LDI        16, 0
	LDI        17, 4
	LDI        18, 0
	LDI        19, 0
	CALL       _float_ulong2fp+0
	STD        Y+26, 16
	STD        Y+27, 17
	STD        Y+28, 18
	STD        Y+29, 19
	LDD        16, Y+30
	LDD        17, Y+31
	LDD        18, Y+32
	LDD        19, Y+33
	PUSH       23
	PUSH       22
	PUSH       21
	PUSH       20
	LDD        20, Y+26
	LDD        21, Y+27
	LDD        22, Y+28
	LDD        23, Y+29
	CALL       _float_fpdiv1+0
	POP        20
	POP        21
	POP        22
	POP        23
	STS        _e3e+0, 16
	STS        _e3e+1, 17
	STS        _e3e+2, 18
	STS        _e3e+3, 19
;wattmeter.mbas,29 :: 		FloatToStr(e3e,e3er)
	LDI        27, hi_addr(_e3er+0)
	PUSH       27
	LDI        27, _e3er+0
	PUSH       27
	PUSH       19
	PUSH       18
	PUSH       17
	PUSH       16
	CALL       _FloatToStr+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 6
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,31 :: 		f3f=e3e*d3d
	LDS        16, _e3e+0
	LDS        17, _e3e+1
	LDS        18, _e3e+2
	LDS        19, _e3e+3
	LDS        20, _d3d+0
	LDS        21, _d3d+1
	LDS        22, _d3d+2
	LDS        23, _d3d+3
	CALL       _float_fpmul1+0
	STS        _f3f+0, 16
	STS        _f3f+1, 17
	STS        _f3f+2, 18
	STS        _f3f+3, 19
;wattmeter.mbas,32 :: 		FloatToStr(f3f,f3fr)
	LDI        27, hi_addr(_f3fr+0)
	PUSH       27
	LDI        27, _f3fr+0
	PUSH       27
	PUSH       19
	PUSH       18
	PUSH       17
	PUSH       16
	CALL       _FloatToStr+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 6
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,34 :: 		if((c1 <> adc_rd1))then
	LDS        18, _c1+0
	LDS        19, _c1+1
	LDS        16, _adc_rd1+0
	LDS        17, _adc_rd1+1
	CP         18, 16
	CPC        19, 17
	BRNE       L__main25
	JMP        L__main7
L__main25:
;wattmeter.mbas,35 :: 		UART1_Write_Text("let the voltage stabilize")
	MOVW       30, 28
	LDI        27, 108
	ST         Z+, 27
	LDI        27, 101
	ST         Z+, 27
	LDI        27, 116
	ST         Z+, 27
	LDI        27, 32
	ST         Z+, 27
	LDI        27, 116
	ST         Z+, 27
	LDI        27, 104
	ST         Z+, 27
	LDI        27, 101
	ST         Z+, 27
	LDI        27, 32
	ST         Z+, 27
	LDI        27, 118
	ST         Z+, 27
	LDI        27, 111
	ST         Z+, 27
	LDI        27, 108
	ST         Z+, 27
	LDI        27, 116
	ST         Z+, 27
	LDI        27, 97
	ST         Z+, 27
	LDI        27, 103
	ST         Z+, 27
	LDI        27, 101
	ST         Z+, 27
	LDI        27, 32
	ST         Z+, 27
	LDI        27, 115
	ST         Z+, 27
	LDI        27, 116
	ST         Z+, 27
	LDI        27, 97
	ST         Z+, 27
	LDI        27, 98
	ST         Z+, 27
	LDI        27, 105
	ST         Z+, 27
	LDI        27, 108
	ST         Z+, 27
	LDI        27, 105
	ST         Z+, 27
	LDI        27, 122
	ST         Z+, 27
	LDI        27, 101
	ST         Z+, 27
	LDI        27, 0
	ST         Z+, 27
	MOVW       16, 28
	PUSH       17
	PUSH       16
	CALL       _UART1_Write_Text+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 2
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,36 :: 		UART1_Write(13)
	LDI        27, 13
	PUSH       27
	CALL       _UART1_Write+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 1
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,37 :: 		UART1_Write(10)
	LDI        27, 10
	PUSH       27
	CALL       _UART1_Write+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 1
	OUT        93, 26
	OUT        94, 27
L__main7:
;wattmeter.mbas,42 :: 		if((c1 = adc_rd1))then
	LDS        18, _c1+0
	LDS        19, _c1+1
	LDS        16, _adc_rd1+0
	LDS        17, _adc_rd1+1
	CP         18, 16
	CPC        19, 17
	BREQ       L__main26
	JMP        L__main10
L__main26:
;wattmeter.mbas,43 :: 		UART1_Write_Text("Voltage = ")
	MOVW       30, 28
	LDI        27, 86
	ST         Z+, 27
	LDI        27, 111
	ST         Z+, 27
	LDI        27, 108
	ST         Z+, 27
	LDI        27, 116
	ST         Z+, 27
	LDI        27, 97
	ST         Z+, 27
	LDI        27, 103
	ST         Z+, 27
	LDI        27, 101
	ST         Z+, 27
	LDI        27, 32
	ST         Z+, 27
	LDI        27, 61
	ST         Z+, 27
	LDI        27, 32
	ST         Z+, 27
	LDI        27, 0
	ST         Z+, 27
	MOVW       16, 28
	PUSH       17
	PUSH       16
	CALL       _UART1_Write_Text+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 2
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,44 :: 		UART1_Write_Text(d3dr)
	LDI        27, hi_addr(_d3dr+0)
	PUSH       27
	LDI        27, _d3dr+0
	PUSH       27
	CALL       _UART1_Write_Text+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 2
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,45 :: 		UART1_Write_Text("V")
	LDI        27, 86
	STD        Y+0, 27
	LDI        27, 0
	STD        Y+1, 27
	MOVW       16, 28
	PUSH       17
	PUSH       16
	CALL       _UART1_Write_Text+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 2
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,49 :: 		UART1_Write(13)
	LDI        27, 13
	PUSH       27
	CALL       _UART1_Write+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 1
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,50 :: 		UART1_Write(10)
	LDI        27, 10
	PUSH       27
	CALL       _UART1_Write+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 1
	OUT        93, 26
	OUT        94, 27
L__main10:
;wattmeter.mbas,53 :: 		if((c2 <> adc_rd2))then
	LDS        18, _c2+0
	LDS        19, _c2+1
	LDS        16, _adc_rd2+0
	LDS        17, _adc_rd2+1
	CP         18, 16
	CPC        19, 17
	BRNE       L__main27
	JMP        L__main13
L__main27:
;wattmeter.mbas,54 :: 		UART1_Write_Text("Let the current stabilize")
	MOVW       30, 28
	LDI        27, 76
	ST         Z+, 27
	LDI        27, 101
	ST         Z+, 27
	LDI        27, 116
	ST         Z+, 27
	LDI        27, 32
	ST         Z+, 27
	LDI        27, 116
	ST         Z+, 27
	LDI        27, 104
	ST         Z+, 27
	LDI        27, 101
	ST         Z+, 27
	LDI        27, 32
	ST         Z+, 27
	LDI        27, 99
	ST         Z+, 27
	LDI        27, 117
	ST         Z+, 27
	LDI        27, 114
	ST         Z+, 27
	LDI        27, 114
	ST         Z+, 27
	LDI        27, 101
	ST         Z+, 27
	LDI        27, 110
	ST         Z+, 27
	LDI        27, 116
	ST         Z+, 27
	LDI        27, 32
	ST         Z+, 27
	LDI        27, 115
	ST         Z+, 27
	LDI        27, 116
	ST         Z+, 27
	LDI        27, 97
	ST         Z+, 27
	LDI        27, 98
	ST         Z+, 27
	LDI        27, 105
	ST         Z+, 27
	LDI        27, 108
	ST         Z+, 27
	LDI        27, 105
	ST         Z+, 27
	LDI        27, 122
	ST         Z+, 27
	LDI        27, 101
	ST         Z+, 27
	LDI        27, 0
	ST         Z+, 27
	MOVW       16, 28
	PUSH       17
	PUSH       16
	CALL       _UART1_Write_Text+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 2
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,55 :: 		UART1_Write(13)
	LDI        27, 13
	PUSH       27
	CALL       _UART1_Write+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 1
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,56 :: 		UART1_Write(10)
	LDI        27, 10
	PUSH       27
	CALL       _UART1_Write+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 1
	OUT        93, 26
	OUT        94, 27
L__main13:
;wattmeter.mbas,59 :: 		if((c2 = adc_rd2))then
	LDS        18, _c2+0
	LDS        19, _c2+1
	LDS        16, _adc_rd2+0
	LDS        17, _adc_rd2+1
	CP         18, 16
	CPC        19, 17
	BREQ       L__main28
	JMP        L__main16
L__main28:
;wattmeter.mbas,60 :: 		UART1_Write_Text("Current = ")
	MOVW       30, 28
	LDI        27, 67
	ST         Z+, 27
	LDI        27, 117
	ST         Z+, 27
	LDI        27, 114
	ST         Z+, 27
	LDI        27, 114
	ST         Z+, 27
	LDI        27, 101
	ST         Z+, 27
	LDI        27, 110
	ST         Z+, 27
	LDI        27, 116
	ST         Z+, 27
	LDI        27, 32
	ST         Z+, 27
	LDI        27, 61
	ST         Z+, 27
	LDI        27, 32
	ST         Z+, 27
	LDI        27, 0
	ST         Z+, 27
	MOVW       16, 28
	PUSH       17
	PUSH       16
	CALL       _UART1_Write_Text+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 2
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,61 :: 		UART1_Write_Text(e3er)
	LDI        27, hi_addr(_e3er+0)
	PUSH       27
	LDI        27, _e3er+0
	PUSH       27
	CALL       _UART1_Write_Text+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 2
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,62 :: 		UART1_Write_Text("A")
	LDI        27, 65
	STD        Y+0, 27
	LDI        27, 0
	STD        Y+1, 27
	MOVW       16, 28
	PUSH       17
	PUSH       16
	CALL       _UART1_Write_Text+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 2
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,63 :: 		UART1_Write(13)
	LDI        27, 13
	PUSH       27
	CALL       _UART1_Write+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 1
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,64 :: 		UART1_Write(10)
	LDI        27, 10
	PUSH       27
	CALL       _UART1_Write+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 1
	OUT        93, 26
	OUT        94, 27
L__main16:
;wattmeter.mbas,67 :: 		if((c1 = adc_rd1))then
	LDS        18, _c1+0
	LDS        19, _c1+1
	LDS        16, _adc_rd1+0
	LDS        17, _adc_rd1+1
	CP         18, 16
	CPC        19, 17
	BREQ       L__main29
	JMP        L__main19
L__main29:
;wattmeter.mbas,68 :: 		if((c2 = adc_rd2))then
	LDS        18, _c2+0
	LDS        19, _c2+1
	LDS        16, _adc_rd2+0
	LDS        17, _adc_rd2+1
	CP         18, 16
	CPC        19, 17
	BREQ       L__main30
	JMP        L__main22
L__main30:
;wattmeter.mbas,69 :: 		UART1_Write_Text("Power = ")
	MOVW       30, 28
	LDI        27, 80
	ST         Z+, 27
	LDI        27, 111
	ST         Z+, 27
	LDI        27, 119
	ST         Z+, 27
	LDI        27, 101
	ST         Z+, 27
	LDI        27, 114
	ST         Z+, 27
	LDI        27, 32
	ST         Z+, 27
	LDI        27, 61
	ST         Z+, 27
	LDI        27, 32
	ST         Z+, 27
	LDI        27, 0
	ST         Z+, 27
	MOVW       16, 28
	PUSH       17
	PUSH       16
	CALL       _UART1_Write_Text+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 2
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,70 :: 		UART1_Write_Text(f3fr)
	LDI        27, hi_addr(_f3fr+0)
	PUSH       27
	LDI        27, _f3fr+0
	PUSH       27
	CALL       _UART1_Write_Text+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 2
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,71 :: 		UART1_Write_Text("W")
	LDI        27, 87
	STD        Y+0, 27
	LDI        27, 0
	STD        Y+1, 27
	MOVW       16, 28
	PUSH       17
	PUSH       16
	CALL       _UART1_Write_Text+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 2
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,72 :: 		UART1_Write(13)
	LDI        27, 13
	PUSH       27
	CALL       _UART1_Write+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 1
	OUT        93, 26
	OUT        94, 27
;wattmeter.mbas,73 :: 		UART1_Write(10)
	LDI        27, 10
	PUSH       27
	CALL       _UART1_Write+0
	IN         26, 93
	IN         27, 94
	ADIW       26, 1
	OUT        93, 26
	OUT        94, 27
L__main22:
;wattmeter.mbas,74 :: 		end if
L__main19:
;wattmeter.mbas,84 :: 		c1 = adc_rd1
	LDS        16, _adc_rd1+0
	LDS        17, _adc_rd1+1
	STS        _c1+0, 16
	STS        _c1+1, 17
;wattmeter.mbas,85 :: 		c2 = adc_rd2
	LDS        16, _adc_rd2+0
	LDS        17, _adc_rd2+1
	STS        _c2+0, 16
	STS        _c2+1, 17
;wattmeter.mbas,88 :: 		Delay_ms(2500)
	LDI        18, 13
	LDI        17, 175
	LDI        16, 183
	DEC        16
	BRNE       $-1
	DEC        17
	BRNE       $-3
	DEC        18
	BRNE       $-5
	NOP
	JMP        L__main2
;wattmeter.mbas,89 :: 		wend
L_end_main:
	JMP        L_end_main
; end of _main
