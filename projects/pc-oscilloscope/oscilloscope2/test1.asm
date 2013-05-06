
_main:
	LDI        R27, 255
	OUT        SPL+0, R27
	LDI        R27, 0
	OUT        SPL+1, R27

;test1.c,7 :: 		void main() {
;test1.c,8 :: 		ADC_Init();
	PUSH       R2
	PUSH       R3
	PUSH       R4
	PUSH       R5
	PUSH       R6
	PUSH       R7
	CALL       _ADC_Init+0
;test1.c,9 :: 		ADCSRA= (ADCSRA && 0xF8);
	IN         R16, ADCSRA+0
	TST        R16
	BRNE       L__main7
	JMP        L_main1
L__main7:
	LDI        R16, 1
	JMP        L_main0
L_main1:
	LDI        R16, 0
L_main0:
	OUT        ADCSRA+0, R16
;test1.c,10 :: 		setting = ADMUX;
	IN         R16, ADMUX+0
	STS        _setting+0, R16
	LDI        R27, 0
	STS        _setting+1, R27
;test1.c,12 :: 		UART1_Init(115200);               // Initialize UART module at 9600 bps
	IN         R27, U2X_bit+0
	SBR        R27, 2
	OUT        U2X_bit+0, R27
	LDI        R27, 8
	OUT        UBRRL+0, R27
	LDI        R27, 0
	OUT        UBRRH+0, R27
	CALL       _UART1_Init+0
;test1.c,14 :: 		Delay_ms(100);                  // Wait for UART module to stabilize
	LDI        R18, lo_addr(R5)
	LDI        R17, lo_addr(R15)
	LDI        R16, 242
L_main2:
	DEC        R16
	BRNE       L_main2
	DEC        R17
	BRNE       L_main2
	DEC        R18
	BRNE       L_main2
;test1.c,15 :: 		UART1_Write_Text("Init");
	LDI        R27, #lo_addr(?lstr1_test1+0)
	MOV        R2, R27
	LDI        R27, hi_addr(?lstr1_test1+0)
	MOV        R3, R27
	CALL       _UART1_Write_Text+0
;test1.c,16 :: 		UART1_Write(13);UART1_Write(10);
	LDI        R27, 13
	MOV        R2, R27
	CALL       _UART1_Write+0
	LDI        R27, 10
	MOV        R2, R27
	CALL       _UART1_Write+0
;test1.c,18 :: 		adc = ADC_Read(0);    // get ADC value from 1 channel
	CLR        R2
	CALL       _ADC_Read+0
	LDI        R18, 0
	MOV        R19, R18
	CALL       _float_ulong2fp+0
	STS        _adc+0, R16
	STS        _adc+1, R17
	STS        _adc+2, R18
	STS        _adc+3, R19
;test1.c,19 :: 		adc = adc * 5 /1024;
	LDI        R20, 0
	LDI        R21, 0
	LDI        R22, 160
	LDI        R23, 64
	CALL       _float_fpmul1+0
	LDI        R20, 0
	LDI        R21, 0
	LDI        R22, 128
	LDI        R23, 68
	CALL       _float_fpdiv1+0
	STS        _adc+0, R16
	STS        _adc+1, R17
	STS        _adc+2, R18
	STS        _adc+3, R19
;test1.c,20 :: 		FloatToStr(adc, adcr);
	LDI        R27, #lo_addr(_adcr+0)
	MOV        R6, R27
	LDI        R27, hi_addr(_adcr+0)
	MOV        R7, R27
	MOVW       R2, R16
	MOVW       R4, R18
	CALL       _FloatToStr+0
;test1.c,21 :: 		UART1_Write_Text(adcr);
	LDI        R27, #lo_addr(_adcr+0)
	MOV        R2, R27
	LDI        R27, hi_addr(_adcr+0)
	MOV        R3, R27
	CALL       _UART1_Write_Text+0
;test1.c,22 :: 		UART1_Write(13);UART1_Write(10);
	LDI        R27, 13
	MOV        R2, R27
	CALL       _UART1_Write+0
	LDI        R27, 10
	MOV        R2, R27
	CALL       _UART1_Write+0
;test1.c,24 :: 		UART1_Write_Text("ADCSRA");
	LDI        R27, #lo_addr(?lstr2_test1+0)
	MOV        R2, R27
	LDI        R27, hi_addr(?lstr2_test1+0)
	MOV        R3, R27
	CALL       _UART1_Write_Text+0
;test1.c,25 :: 		UART1_Write(13);UART1_Write(10);
	LDI        R27, 13
	MOV        R2, R27
	CALL       _UART1_Write+0
	LDI        R27, 10
	MOV        R2, R27
	CALL       _UART1_Write+0
;test1.c,26 :: 		DDB3_bit = 1;
	IN         R27, DDB3_bit+0
	SBR        R27, 8
	OUT        DDB3_bit+0, R27
;test1.c,27 :: 		DDD7_bit = 1;
	IN         R27, DDD7_bit+0
	SBR        R27, 128
	OUT        DDD7_bit+0, R27
;test1.c,28 :: 		PWM1_Init(_PWM1_FAST_MODE, _PWM1_PRESCALER_1, _PWM1_NON_INVERTED, 127);
	LDI        R27, 127
	MOV        R5, R27
	LDI        R27, 32
	MOV        R4, R27
	LDI        R27, 1
	MOV        R3, R27
	LDI        R27, 72
	MOV        R2, R27
	CALL       _PWM1_Init+0
;test1.c,29 :: 		PWM2_Init(_PWM1_FAST_MODE, _PWM1_PRESCALER_1, _PWM1_NON_INVERTED, 200);
	LDI        R27, 200
	MOV        R5, R27
	LDI        R27, 32
	MOV        R4, R27
	LDI        R27, 1
	MOV        R3, R27
	LDI        R27, 72
	MOV        R2, R27
	CALL       _PWM2_Init+0
;test1.c,30 :: 		PWM1_start();
	CALL       _PWM1_Start+0
;test1.c,31 :: 		PWM2_start();
	CALL       _PWM2_Start+0
;test1.c,33 :: 		while(1){
L_main4:
;test1.c,35 :: 		adc = ADC_Read(0);    // get ADC value from 1 channel
	CLR        R2
	CALL       _ADC_Read+0
	LDI        R18, 0
	MOV        R19, R18
	CALL       _float_ulong2fp+0
	STS        _adc+0, R16
	STS        _adc+1, R17
	STS        _adc+2, R18
	STS        _adc+3, R19
;test1.c,36 :: 		adc = adc * 5 /1024;
	LDI        R20, 0
	LDI        R21, 0
	LDI        R22, 160
	LDI        R23, 64
	CALL       _float_fpmul1+0
	LDI        R20, 0
	LDI        R21, 0
	LDI        R22, 128
	LDI        R23, 68
	CALL       _float_fpdiv1+0
	STS        _adc+0, R16
	STS        _adc+1, R17
	STS        _adc+2, R18
	STS        _adc+3, R19
;test1.c,37 :: 		FloatToStr(adc, adcr);
	LDI        R27, #lo_addr(_adcr+0)
	MOV        R6, R27
	LDI        R27, hi_addr(_adcr+0)
	MOV        R7, R27
	MOVW       R2, R16
	MOVW       R4, R18
	CALL       _FloatToStr+0
;test1.c,38 :: 		UART1_Write_Text(adcr);
	LDI        R27, #lo_addr(_adcr+0)
	MOV        R2, R27
	LDI        R27, hi_addr(_adcr+0)
	MOV        R3, R27
	CALL       _UART1_Write_Text+0
;test1.c,39 :: 		UART1_Write(13);UART1_Write(10);
	LDI        R27, 13
	MOV        R2, R27
	CALL       _UART1_Write+0
	LDI        R27, 10
	MOV        R2, R27
	CALL       _UART1_Write+0
;test1.c,42 :: 		}
	JMP        L_main4
;test1.c,43 :: 		}
L_end_main:
	JMP        L_end_main
; end of _main
